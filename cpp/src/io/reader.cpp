#include <cmath>
#include <stdexcept>

#include "copc-lib/copc/extents.hpp"
#include "copc-lib/hierarchy/internal/hierarchy.hpp"
#include "copc-lib/io/reader.hpp"
#include "copc-lib/las/header.hpp"
#include "copc-lib/laz/decompressor.hpp"

#include <lazperf/readers.hpp>
#include <lazperf/vlr.hpp>

namespace copc
{

void Reader::InitReader()
{

    if (!in_stream_->good())
        throw std::runtime_error("Invalid input stream!");

    reader_ = std::make_unique<lazperf::reader::generic_file>(*in_stream_);

    auto header = las::LasHeader::FromLazPerf(reader_->header());

    // Load vlrs and evlrs
    auto vlrs = ReadVlrHeaders();

    auto copc_info = ReadCopcInfoVlr();
    auto wkt = ReadWKTVlr(vlrs);
    auto eb = ReadExtraBytesVlr(vlrs);
    auto copc_extents = ReadCopcExtentsVlr(vlrs, eb);

    file_ = std::make_shared<CopcFile>(header, copc_info, copc_extents, wkt, eb);
    file_->vlrs = vlrs;

    hierarchy_ = std::make_shared<Internal::Hierarchy>(copc_info.root_hier_offset, copc_info.root_hier_size);
}

std::map<uint64_t, las::VlrHeader> Reader::ReadVlrHeaders()
{
    std::map<uint64_t, las::VlrHeader> out;

    // Move stream to beginning of VLRs
    in_stream_->seekg(reader_->header().header_size);

    // Iterate through all vlr's and add them to the `vlrs` list
    for (int i = 0; i < reader_->header().vlr_count; i++)
    {
        uint64_t cur_pos = in_stream_->tellg();
        auto h = las::VlrHeader(lazperf::vlr_header::create(*in_stream_));
        out.insert({cur_pos, h});

        in_stream_->seekg(h.data_length, std::ios::cur); // jump foward
    }

    // Move stream to beginning of EVLRs
    in_stream_->seekg(reader_->header().evlr_offset);

    // Iterate through all vlr's and add them to the `vlrs` list
    for (int i = 0; i < reader_->header().evlr_count; i++)
    {
        uint64_t cur_pos = in_stream_->tellg();
        auto h = las::VlrHeader(lazperf::evlr_header::create(*in_stream_));
        out.insert({cur_pos, h});

        in_stream_->seekg(h.data_length, std::ios::cur); // jump foward
    }

    return out;
}

las::CopcInfoVlr Reader::ReadCopcInfoVlr()
{
    in_stream_->seekg(COPC_OFFSET);
    return las::CopcInfoVlr::create(*in_stream_);
}

CopcExtents Reader::ReadCopcExtentsVlr(const std::map<uint64_t, las::VlrHeader> &vlrs, const las::EbVlr &eb_vlr)
{
    auto offset = FetchVlr(vlrs, "copc", 10000);
    auto pair = vlrs.find(offset);

    if (pair != vlrs.end())
    {
        in_stream_->seekg(offset + lazperf::vlr_header::Size);
        return CopcExtents(las::CopcExtentsVlr::create(*in_stream_, pair->second.data_length),
                           static_cast<int8_t>(reader_->header().point_format_id),
                           static_cast<uint16_t>(eb_vlr.items.size()));
    }
    return CopcExtents(static_cast<int8_t>(reader_->header().point_format_id),
                       static_cast<uint16_t>(eb_vlr.items.size()));
}

las::WktVlr Reader::ReadWKTVlr(const std::map<uint64_t, las::VlrHeader> &vlrs)
{
    auto offset = FetchVlr(vlrs, "LASF_Projection", 2112);
    auto pair = vlrs.find(offset);

    if (pair != vlrs.end())
    {
        in_stream_->seekg(offset + lazperf::vlr_header::Size);
        return las::WktVlr::create(*in_stream_, pair->second.data_length);
    }
    return {};
}

las::EbVlr Reader::ReadExtraBytesVlr(const std::map<uint64_t, las::VlrHeader> &vlrs)
{
    auto offset = FetchVlr(vlrs, "LASF_Spec", 4);
    auto pair = vlrs.find(offset);

    if (pair != vlrs.end())
    {
        in_stream_->seekg(offset + lazperf::vlr_header::Size);
        return las::EbVlr::create(*in_stream_, pair->second.data_length);
    }
    return {};
}

uint64_t Reader::FetchVlr(const std::map<uint64_t, las::VlrHeader> &vlrs, const std::string &user_id,
                          uint16_t record_id)
{
    for (auto &[offset, vlr_header] : vlrs)
    {
        if (vlr_header.user_id == user_id && vlr_header.record_id == record_id)
        {
            return offset;
        }
    }
    return {};
}

std::vector<Entry> Reader::ReadPage(std::shared_ptr<Internal::PageInternal> page)
{
    std::vector<Entry> out;
    if (!page->IsValid())
        throw std::runtime_error("Cannot load an invalid page.");

    // reset the stream to the page's offset
    in_stream_->seekg(page->offset);

    // Iterate through each Entry in the page
    int num_entries = int(page->byte_size / Entry::ENTRY_SIZE);
    for (int i = 0; i < num_entries; i++)
    {
        Entry e = Entry::Unpack(*in_stream_);
        if (!e.IsValid())
            throw std::runtime_error("Entry is invalid! " + e.ToString());

        out.push_back(e);
    }

    page->loaded = true;
    return out;
}

las::Points Reader::GetPoints(Node const &node)
{
    std::vector<char> point_data = GetPointData(node);
    return las::Points::Unpack(point_data, file_->GetLasHeader());
}

las::Points Reader::GetPoints(VoxelKey const &key)
{
    std::vector<char> point_data = GetPointData(key);

    if (point_data.empty())
        return las::Points(file_->GetLasHeader());

    return las::Points::Unpack(point_data, file_->GetLasHeader());
}

std::vector<char> Reader::GetPointData(Node const &node)
{
    if (!node.IsValid())
        throw std::runtime_error("Cannot load an invalid node.");

    in_stream_->seekg(node.offset);

    auto las_header = file_->GetLasHeader();
    std::vector<char> point_data = laz::Decompressor::DecompressBytes(*in_stream_, las_header, node.point_count);
    return point_data;
}

std::vector<char> Reader::GetPointData(VoxelKey const &key)
{
    std::vector<char> out;
    if (!key.IsValid())
        return out;

    auto node = FindNode(key);
    if (!node.IsValid())
        return out;

    return GetPointData(node);
}

std::vector<char> Reader::GetPointDataCompressed(Node const &node)
{
    if (!node.IsValid())
        throw std::runtime_error("Cannot load an invalid node.");

    in_stream_->seekg(node.offset);

    std::vector<char> out;
    out.resize(node.byte_size);
    in_stream_->read(&out[0], node.byte_size);
    return out;
}

std::vector<char> Reader::GetPointDataCompressed(VoxelKey const &key)
{
    std::vector<char> out;
    if (!key.IsValid())
        return out;

    auto node = FindNode(key);
    if (!node.IsValid())
        return out;

    return GetPointDataCompressed(node);
}

std::vector<Node> Reader::GetAllChildren(const VoxelKey &key)
{
    std::vector<Node> out;
    if (!key.IsValid())
        return out;

    // Load all pages upto the current key
    auto node = FindNode(key);
    // If a page with this key doesn't exist, check if the node itself exists and return it
    if (!hierarchy_->PageExists(key))
    {
        if (node.IsValid())
            out.push_back(node);
        return out;
    }

    // If the page does exist, we need to read all its children and subpages into memory recursively
    LoadPageHierarchy(hierarchy_->seen_pages_[key], out);
    return out;
}

las::Points Reader::GetAllPoints(double resolution)
{
    auto out = las::Points(GetLasHeader());

    auto max_depth = GetDepthAtResolution(resolution);

    // Get all nodes in octree
    for (const auto &node : GetAllChildren())
        if (node.key.d <= max_depth)
            out.AddPoints(GetPoints(node));
    return out;
}

std::vector<Node> Reader::GetNodesWithinBox(const Box &box, double resolution)
{
    std::vector<Node> out;

    auto header = GetLasHeader();
    auto copc_info = GetCopcInfo();
    auto max_depth = GetDepthAtResolution(resolution);

    for (const auto &node : GetAllChildren())
    {
        if (node.key.Within(header, box) && node.key.d <= max_depth)
            out.push_back(node);
    }

    return out;
}

std::vector<Node> Reader::GetNodesIntersectBox(const Box &box, double resolution)
{
    std::vector<Node> out;

    auto header = GetLasHeader();
    auto copc_info = GetCopcInfo();
    auto max_depth = GetDepthAtResolution(resolution);

    // Get all nodes in octree
    for (const auto &node : GetAllChildren())
    {
        if (node.key.Intersects(header, box) && node.key.d <= max_depth)
            out.push_back(node);
    }

    return out;
}

las::Points Reader::GetPointsWithinBox(const Box &box, double resolution)
{
    auto header = GetLasHeader();
    auto copc_info = GetCopcInfo();
    auto max_depth = GetDepthAtResolution(resolution);
    auto out = las::Points(header);

    // Get all nodes in octree
    for (const auto &node : GetAllChildren())
    {
        if (node.key.d <= max_depth)
        {
            // If node fits in Box
            if (node.key.Within(header, box))
            {
                // If the node is within the box add all points
                out.AddPoints(GetPoints(node));
            }
            else if (node.key.Intersects(header, box))
            {
                // If the node only crosses the box then get subset of points within box
                auto points = GetPoints(node);
                out.AddPoints(points.GetWithin(box));
            }
        }
    }
    return out;
}

int32_t Reader::GetDepthAtResolution(double resolution)
{
    // Compute max depth
    int32_t max_depth = -1;
    // Get all nodes in octree
    for (const auto &node : GetAllChildren())
    {
        if (node.key.d > max_depth)
            max_depth = node.key.d;
    }

    // If query resolution is <=0 return the octree's max depth
    if (resolution <= 0.0)
        return max_depth;

    auto current_resolution = GetCopcInfo().spacing;

    for (int32_t i = 0; i <= max_depth; i++)
    {
        if (current_resolution <= resolution)
            return i;
        current_resolution /= 2;
    }
    return max_depth;
}

std::vector<Node> Reader::GetNodesAtResolution(double resolution)
{
    auto target_depth = GetDepthAtResolution(resolution);

    std::vector<Node> out;

    for (const auto &node : GetAllChildren())
    {
        if (node.key.d == target_depth)
            out.push_back(node);
    }

    return out;
}

std::vector<Node> Reader::GetNodesWithinResolution(double resolution)
{
    auto target_depth = GetDepthAtResolution(resolution);

    std::vector<Node> out;

    for (const auto &node : GetAllChildren())
    {
        if (node.key.d <= target_depth)
            out.push_back(node);
    }

    return out;
}

} // namespace copc
