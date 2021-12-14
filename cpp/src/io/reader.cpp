#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "copc-lib/copc/config.hpp"
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
    vlrs_ = ReadVlrHeaders();

    // Check that required info and hierarchy VLRs are present
    if (FetchVlr(vlrs_, "copc", 1) == 0 || FetchVlr(vlrs_, "copc", 1000) == 0)
        throw std::runtime_error(
            "Reader::InitReader: Either Info or Hierarchy VLR missing, make sure you are loading a COPC file.");

    auto copc_info = ReadCopcInfoVlr();
    auto wkt = ReadWktVlr(vlrs_);
    auto eb = ReadExtraBytesVlr(vlrs_);
    auto copc_extents = ReadCopcExtentsVlr(vlrs_, eb);

    config_ = copc::CopcConfig(header, copc_info, copc_extents, wkt.wkt, eb);

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

CopcInfo Reader::ReadCopcInfoVlr()
{
    in_stream_->seekg(COPC_OFFSET);
    return {lazperf::copc_info_vlr::create(*in_stream_)};
}

CopcExtents Reader::ReadCopcExtentsVlr(std::map<uint64_t, las::VlrHeader> &vlrs, const las::EbVlr &eb_vlr) const
{
    auto offset = FetchVlr(vlrs, "copc", 10000);
    auto extended_offset = FetchVlr(vlrs, "rock_robotic", 10001);
    if (offset == 0)
        throw std::runtime_error("Reader::ReadCopcExtentsVlr: No COPC Extents VLR found in file.");

    in_stream_->seekg(offset + lazperf::vlr_header::Size);
    CopcExtents extents(las::CopcExtentsVlr::create(*in_stream_, vlrs[offset].data_length),
                        static_cast<int8_t>(reader_->header().point_format_id),
                        static_cast<uint16_t>(eb_vlr.items.size()), extended_offset != 0);

    // Load mean/var if they exist
    if (extended_offset != 0)
    {
        in_stream_->seekg(extended_offset + lazperf::vlr_header::Size);
        extents.SetExtendedStats(las::CopcExtentsVlr::create(*in_stream_, vlrs[extended_offset].data_length));
    }
    return extents;
}

las::WktVlr Reader::ReadWktVlr(std::map<uint64_t, las::VlrHeader> &vlrs)
{
    auto offset = FetchVlr(vlrs, "LASF_Projection", 2112);
    if (offset != 0)
    {
        in_stream_->seekg(offset + lazperf::evlr_header::Size);
        return las::WktVlr::create(*in_stream_, vlrs[offset].data_length);
    }
    return las::WktVlr();
}

las::EbVlr Reader::ReadExtraBytesVlr(std::map<uint64_t, las::VlrHeader> &vlrs)
{
    auto offset = FetchVlr(vlrs, "LASF_Spec", 4);
    if (offset != 0)
    {
        in_stream_->seekg(offset + lazperf::vlr_header::Size);
        return las::EbVlr::create(*in_stream_, vlrs[offset].data_length);
    }
    return las::EbVlr();
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
    return 0;
}

std::vector<Entry> Reader::ReadPage(std::shared_ptr<Internal::PageInternal> page)
{
    std::vector<Entry> out;
    if (!page->IsValid())
        throw std::runtime_error("Reader::ReadPage: Cannot load an invalid page.");

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
    return las::Points::Unpack(point_data, config_.LasHeader());
}

las::Points Reader::GetPoints(VoxelKey const &key)
{
    std::vector<char> point_data = GetPointData(key);

    if (point_data.empty())
        return las::Points(config_.LasHeader());

    return las::Points::Unpack(point_data, config_.LasHeader());
}

std::vector<char> Reader::GetPointData(Node const &node)
{
    if (!node.IsValid())
        throw std::runtime_error("Reader::GetPointData: Cannot load an invalid node.");

    in_stream_->seekg(node.offset);

    auto las_header = config_.LasHeader();
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
        throw std::runtime_error("Reader::GetPointDataCompressed: Cannot load an invalid node.");

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

std::vector<Node> Reader::GetAllChildrenOfPage(const VoxelKey &key)
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

std::vector<VoxelKey> Reader::GetPageList()
{
    // Load all nodes and pages in hierarchy
    GetAllNodes();

    std::vector<VoxelKey> page_keys;
    page_keys.reserve(hierarchy_->seen_pages_.size());

    for (auto &seen_page : hierarchy_->seen_pages_)
    {
        page_keys.push_back(seen_page.second->key);
    }
    return page_keys;
}

las::Points Reader::GetAllPoints(double resolution)
{
    auto out = las::Points(config_.LasHeader());

    auto max_depth = GetDepthAtResolution(resolution);

    // Get all nodes in octree
    for (const auto &node : GetAllNodes())
        if (node.key.d <= max_depth)
            out.AddPoints(GetPoints(node));
    return out;
}

std::vector<Node> Reader::GetNodesWithinBox(const Box &box, double resolution)
{
    std::vector<Node> out;

    auto max_depth = GetDepthAtResolution(resolution);

    for (const auto &node : GetAllNodes())
    {
        if (node.key.Within(config_.LasHeader(), box) && node.key.d <= max_depth)
            out.push_back(node);
    }

    return out;
}

std::vector<Node> Reader::GetNodesIntersectBox(const Box &box, double resolution)
{
    std::vector<Node> out;

    auto max_depth = GetDepthAtResolution(resolution);

    // Get all nodes in octree
    for (const auto &node : GetAllNodes())
    {
        if (node.key.Intersects(config_.LasHeader(), box) && node.key.d <= max_depth)
            out.push_back(node);
    }

    return out;
}

las::Points Reader::GetPointsWithinBox(const Box &box, double resolution)
{
    auto max_depth = GetDepthAtResolution(resolution);
    auto out = las::Points(config_.LasHeader());

    // Get all nodes in octree
    for (const auto &node : GetAllNodes())
    {
        if (node.key.d <= max_depth)
        {
            // If node fits in Box
            if (node.key.Within(config_.LasHeader(), box))
            {
                // If the node is within the box add all points
                out.AddPoints(GetPoints(node));
            }
            else if (node.key.Intersects(config_.LasHeader(), box))
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
    for (const auto &node : GetAllNodes())
    {
        if (node.key.d > max_depth)
            max_depth = node.key.d;
    }

    // If query resolution is <=0 return the octree's max depth
    if (resolution <= 0.0)
        return max_depth;

    auto current_resolution = config_.CopcInfo().spacing;

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

    for (const auto &node : GetAllNodes())
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

    for (const auto &node : GetAllNodes())
    {
        if (node.key.d <= target_depth)
            out.push_back(node);
    }

    return out;
}

bool Reader::ValidateSpatialBounds(bool verbose)
{
    bool is_valid = true;
    auto header = config_.LasHeader();

    int total_points_outside_header_bounds{0};
    int total_points_outside_node_bounds{0};

    // If verbose, set precision and print the las header.
    if (verbose)
    {
        std::cout << std::setprecision(std::numeric_limits<double>::max_digits10);
        std::cout << "Validating Spatial Bounds" << std::endl;
        std::cout << "File info:" << std::endl;
        std::cout << "\tPoint Count: " << header.PointCount() << std::endl;
        std::cout << "\tScale: " << header.Scale().ToString() << std::endl;
        std::cout << "\tOffset: " << header.Offset().ToString() << std::endl;
        std::cout << "\tMin Bounds: " << header.min.ToString() << std::endl;
        std::cout << "\tMax Bounds: " << header.max.ToString() << std::endl;
        std::cout << std::endl << "Validating bounds..." << std::endl << std::endl;
    }

    for (const auto &node : GetAllNodes())
    {

        // Check if node intersects las header bounds
        if (!Box(node.key, header).Intersects(header.Bounds()))
        {
            is_valid = false;
            if (!verbose)
                return false;
            std::cout << "Node " << node.key.ToString() << " is outside of las header bounds ("
                      << header.Bounds().ToString() << ")." << std::endl;
            total_points_outside_header_bounds += node.point_count;
        }
        else
        {
            auto points = GetPoints(node);
            // If node not within las header bounds then check individual points
            if (!Box(node.key, header).Within(header.Bounds()))
            {
                for (auto const &point : points)
                {
                    if (!point->Within(header.Bounds()))
                    {
                        is_valid = false;
                        if (!verbose)
                            return false;
                        std::cout << "Point (" << point->X() << "," << point->Y() << "," << point->Z() << ") from node "
                                  << node.key.ToString() << " is outside of las header bounds ("
                                  << header.Bounds().ToString() << ")." << std::endl;
                        total_points_outside_header_bounds++;
                    }
                }
            }
            // Check that points fall within the node bounds
            auto box = Box(node.key, header);
            for (auto const &point : points)
            {
                if (!point->Within(box))
                {
                    is_valid = false;
                    if (!verbose)
                        return false;
                    std::cout << "Point (" << point->X() << "," << point->Y() << "," << point->Z()
                              << ") is outside of node " << node.key.ToString() << " bounds (" << box.ToString() << ")."
                              << std::endl;
                    total_points_outside_node_bounds++;
                }
            }
        }
    }

    if (verbose)
    {
        std::cout << std::endl;
        std::cout << "...Bounds validation done." << std::endl << std::endl;
        std::cout << "Number of points outside header bounds: " << total_points_outside_header_bounds << std::endl;
        std::cout << "Number of points outside node bounds: " << total_points_outside_node_bounds << std::endl;
        std::cout << std::endl;
        is_valid ? std::cout << "Spatial bounds are valid!" : std::cout << "Spatial bounds are invalid!";
        std::cout << std::endl;
    }
    return is_valid;
}

} // namespace copc
