#include <copc-lib/io/reader.hpp>
#include <copc-lib/las/header.hpp>
#include <copc-lib/laz/decompressor.hpp>

#include <lazperf/readers.hpp>

namespace copc
{
Reader::Reader(std::istream &in_stream) : in_stream(in_stream)
{
    if (!this->in_stream.good())
        throw std::runtime_error("Invalid input stream!");

    this->reader_ = std::make_unique<lazperf::reader::generic_file>(this->in_stream);

    InitFile();
}

void Reader::InitFile()
{
    auto header = this->reader_->header();

    std::map<uint64_t, las::VlrHeader> vlrs = ReadVlrs();
    auto copc_data = ReadCopcData();
    auto wkt = ReadWktData(copc_data);
    auto eb = ReadExtraByteVlr(vlrs);

    this->file = std::make_shared<CopcFile>(header, copc_data, wkt, eb);
    this->file->vlrs = vlrs;

    this->hierarchy = std::make_shared<Internal::Hierarchy>(copc_data.root_hier_offset, copc_data.root_hier_size);
}

std::map<uint64_t, las::VlrHeader> Reader::ReadVlrs()
{
    std::map<uint64_t, las::VlrHeader> out;

    // Move stream to beginning of VLRs
    this->in_stream.seekg(this->reader_->header().header_size);

    // Iterate through all vlr's and add them to the `vlrs` list
    size_t count = 0;
    while (count < this->reader_->header().vlr_count && this->in_stream.good() && !this->in_stream.eof())
    {
        uint64_t cur_pos = this->in_stream.tellg();
        las::VlrHeader h = las::VlrHeader::create(this->in_stream);
        out[cur_pos] = h;

        this->in_stream.seekg(h.data_length, std::ios::cur); // jump foward
        count++;
    }
    return out;
}

las::CopcVlr Reader::ReadCopcData()
{
    this->in_stream.seekg(COPC_OFFSET);
    las::CopcVlr copc = las::CopcVlr::create(this->in_stream);
    return copc;
}

las::WktVlr Reader::ReadWktData(const las::CopcVlr &copc_data)
{
    this->in_stream.seekg(copc_data.wkt_vlr_offset);
    las::WktVlr wkt = las::WktVlr::create(this->in_stream, copc_data.wkt_vlr_size);
    return wkt;
}

std::vector<Entry> Reader::ReadPage(std::shared_ptr<Internal::PageInternal> page)
{
    std::vector<Entry> out;
    if (!page->IsValid())
        throw std::runtime_error("Cannot load an invalid page.");

    // reset the stream to the page's offset
    in_stream.seekg(page->offset);

    // Iterate through each Entry in the page
    int num_entries = int(page->size / Entry::ENTRY_SIZE);
    for (int i = 0; i < num_entries; i++)
    {
        Entry e = Entry::Unpack(in_stream);
        if (!e.IsValid())
            throw std::runtime_error("Entry is invalid! " + e.ToString());

        out.push_back(e);
    }

    page->loaded = true;
    return out;
}

std::vector<las::Point> Reader::GetPoints(Node const &node)
{
    std::vector<char> point_data = GetPointData(node);
    return Node::UnpackPoints(point_data, file->GetLasHeader().point_format_id,
                              file->GetLasHeader().point_record_length);
}

std::vector<char> Reader::GetPointData(Node const &node)
{
    if (!node.IsValid())
        throw std::runtime_error("Cannot load an invalid node.");

    in_stream.seekg(node.offset);

    auto las_header = file->GetLasHeader();
    std::vector<char> point_data = laz::Decompressor::DecompressBytes(in_stream, las_header, node.point_count);
    return point_data;
}

std::vector<char> Reader::GetPointDataCompressed(Node const &node)
{
    if (!node.IsValid())
        throw std::runtime_error("Cannot load an invalid node.");

    in_stream.seekg(node.offset);

    std::vector<char> out;
    out.resize(node.size);
    in_stream.read(&out[0], node.size);
    return out;
}

std::vector<Node> Reader::GetAllChildren(VoxelKey key)
{
    std::vector<Node> out;
    if (!key.IsValid())
        return out;

    // Load all pages upto the current key
    auto node = FindNode(key);
    // If a page with this key doesn't exist, check if the node itself exists and return it
    if (!hierarchy->PageExists(key))
    {
        if (node.IsValid())
            out.push_back(node);
        return out;
    }

    // If the page does exist, we need to read all its children and subpages into memory recursively
    LoadPageHierarchy(hierarchy->seen_pages_[key], out);
    return out;
}

las::EbVlr Reader::ReadExtraByteVlr(std::map<uint64_t, las::VlrHeader> &vlrs)
{
    for (auto &[offset, vlr_header] : vlrs)
    {
        if (vlr_header.user_id == "LASF_Spec" && vlr_header.record_id == 4)
        {
            in_stream.seekg(offset + vlr_header.Size);
            return las::EbVlr::create(in_stream, vlr_header.data_length);
        }
    }
    return {};
}

} // namespace copc
