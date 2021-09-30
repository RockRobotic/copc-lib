#include <copc-lib/copc/extents.hpp>
#include <copc-lib/io/reader.hpp>
#include <copc-lib/las/header.hpp>
#include <copc-lib/laz/decompressor.hpp>

#include <lazperf/readers.hpp>

namespace copc
{

void Reader::InitReader()
{

    if (!in_stream_->good())
        throw std::runtime_error("Invalid input stream!");

    reader_ = std::make_unique<lazperf::reader::generic_file>(*in_stream_);

    auto header = las::LasHeader::FromLazPerf(reader_->header());

    // Load vlrs and evlrs
    auto vlrs = ReadVlrs();
    auto evlrs = ReadEvlrs();

    auto copc_info = ReadCopcInfo();
    auto wkt = ReadWktData(vlrs);
    auto eb = ReadExtraByteVlr(vlrs);
    auto copc_extents = ReadCopcExtents(vlrs, eb);

    file_ = std::make_shared<CopcFile>(header, copc_info, copc_extents, wkt, eb);
    file_->vlrs = vlrs;
    file_->evlrs = evlrs;

    hierarchy_ = std::make_shared<Internal::Hierarchy>(copc_info.root_hier_offset, copc_info.root_hier_size);
}

std::map<uint64_t, las::VlrHeader> Reader::ReadVlrs()
{
    std::map<uint64_t, las::VlrHeader> out;

    // Move stream to beginning of VLRs
    in_stream_->seekg(reader_->header().header_size);

    // Iterate through all vlr's and add them to the `vlrs` list
    size_t count = 0;
    while (count < reader_->header().vlr_count && in_stream_->good() && !in_stream_->eof())
    {
        uint64_t cur_pos = in_stream_->tellg();
        las::VlrHeader h = las::VlrHeader::create(*in_stream_);
        out[cur_pos] = h;

        in_stream_->seekg(h.data_length, std::ios::cur); // jump foward
        count++;
    }
    return out;
}

std::map<uint64_t, las::EvlrHeader> Reader::ReadEvlrs()
{
    std::map<uint64_t, las::EvlrHeader> out;

    // Move stream to beginning of VLRs
    in_stream_->seekg(reader_->header().evlr_offset);

    // Iterate through all evlr's and add them to the `evlrs` list
    size_t count = 0;

    while (count < reader_->header().evlr_count && in_stream_->good() && !in_stream_->eof())
    {
        uint64_t cur_pos = in_stream_->tellg();
        las::EvlrHeader h = las::EvlrHeader::create(*in_stream_);
        out[cur_pos] = h;

        in_stream_->seekg(h.data_length, std::ios::cur); // jump foward
        count++;
    }
    return out;
}

las::CopcInfoVlr Reader::ReadCopcInfo()
{
    in_stream_->seekg(COPC_OFFSET);
    las::CopcInfoVlr copc_info = las::CopcInfoVlr::create(*in_stream_);
    return copc_info;
}

CopcExtents Reader::ReadCopcExtents(const std::map<uint64_t, las::VlrHeader> &vlrs, const las::EbVlr &eb_vlr)
{

    // TODO[Leo]: (Extents) Update this once we have updated copc test file
    if (reader_->header().point_format_id < 6 || reader_->header().point_format_id > 8)
        return {7, static_cast<uint16_t>(eb_vlr.items.size())};

    for (auto &[offset, vlr_header] : vlrs)
    {
        if (vlr_header.user_id == "copc" && vlr_header.record_id == 10000)
        {
            in_stream_->seekg(offset + las::VlrHeader::Size);
            return {las::CopcExtentsVlr::create(*in_stream_, vlr_header.data_length),
                    static_cast<int8_t>(reader_->header().point_format_id), static_cast<uint16_t>(eb_vlr.items.size())};
        }
    }
    return {static_cast<int8_t>(reader_->header().point_format_id), static_cast<uint16_t>(eb_vlr.items.size())};
}

las::WktVlr Reader::ReadWktData(const std::map<uint64_t, las::VlrHeader> &vlrs)
{
    for (auto &[offset, vlr_header] : vlrs)
    {
        if (vlr_header.user_id == "LASF_Projection" && vlr_header.record_id == 2112)
        {
            in_stream_->seekg(offset + las::VlrHeader::Size);
            return las::WktVlr::create(*in_stream_, vlr_header.data_length);
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

las::Points Reader::GetAllPoints()
{
    auto out = las::Points(GetLasHeader());

    // Get all nodes in octree
    for (const auto &node : GetAllChildren())
        out.AddPoints(GetPoints(node));
    return out;
}

std::vector<Node> Reader::GetNodesWithinBox(const Box &box)
{
    std::vector<Node> out;

    auto header = GetLasHeader();
    for (const auto &node : GetAllChildren())
    {
        if (node.key.Within(box, header))
            out.push_back(node);
    }

    return out;
}

std::vector<Node> Reader::GetNodesIntersectBox(const Box &box)
{
    std::vector<Node> out;

    auto header = GetLasHeader();
    for (const auto &node : GetAllChildren())
    {
        if (node.key.Intersects(box, header))
            out.push_back(node);
    }

    return out;
}

las::Points Reader::GetPointsWithinBox(const Box &box)
{
    auto header = GetLasHeader();
    auto out = las::Points(header);

    // Get all nodes in octree
    for (const auto &node : GetAllChildren())
    {
        // If node fits in Box
        if (node.key.Intersects(box, header))
        {
            // Add points that fit in the box
            auto points = GetPoints(node);
            out.AddPoints(points.GetWithin(box));
        }
    }
    return out;
}

las::EbVlr Reader::ReadExtraByteVlr(std::map<uint64_t, las::VlrHeader> &vlrs)
{
    for (auto &[offset, vlr_header] : vlrs)
    {
        if (vlr_header.user_id == "LASF_Spec" && vlr_header.record_id == 4)
        {
            in_stream_->seekg(offset + las::VlrHeader::Size);
            return las::EbVlr::create(*in_stream_, vlr_header.data_length);
        }
    }
    return {};
}

} // namespace copc
