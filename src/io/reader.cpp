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

    ReadVlrs();

    auto copc_data = GetCopcData();
    auto wkt = GetWktData(copc_data);

    this->file = std::make_unique<CopcFile>(header, copc_data, wkt);
    this->hierarchy = std::make_unique<Hierarchy>(copc_data.root_hier_offset, copc_data.root_hier_size);
}

void Reader::ReadVlrs()
{
    // Move stream to beginning of VLRs
    this->in_stream.seekg(this->reader_->header().header_size);

    // Iterate through all vlr's and add them to the `vlrs_` list
    size_t count = 0;
    while (count < this->reader_->header().vlr_count && this->in_stream.good() && !this->in_stream.eof())
    {
        uint64_t cur_pos = this->in_stream.tellg();
        las::VlrHeader h = las::VlrHeader::create(this->in_stream);
        vlrs_[cur_pos] = h;

        this->in_stream.seekg(h.data_length, std::ios::cur); // jump foward
        count++;
    }
}

las::CopcVlr Reader::GetCopcData()
{
    this->in_stream.seekg(COPC_OFFSET);
    las::CopcVlr copc = las::CopcVlr::create(this->in_stream);
    return copc;
}

las::WktVlr Reader::GetWktData(las::CopcVlr copc_data)
{
    this->in_stream.seekg(copc_data.wkt_vlr_offset);
    las::WktVlr wkt = las::WktVlr::create(this->in_stream, copc_data.wkt_vlr_size);
    return wkt;
}

std::vector<Entry> Reader::ReadPage(std::shared_ptr<Page> page)
{
    std::vector<Entry> out;
    if (!page->IsValid())
        throw std::runtime_error("Cannot load an invalid page.");

    // reset the stream to the page's offset
    in_stream.seekg(page->offset);

    // Iterate through each Entry in the page
    int num_entries = int(page->size / ENTRY_SIZE);
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

// Find a node object given a key
std::shared_ptr<Node> Reader::FindNode(VoxelKey key)
{
    // Check if the entry has already been loaded
    if (hierarchy->loaded_nodes_.find(key) != hierarchy->loaded_nodes_.end())
    {
        return hierarchy->loaded_nodes_[key];
    }

    // Get a list of the key's hierarchial parents, so we can see if any of them are loaded
    auto parents_list = key.GetParents(true);

    // Find if of the key's ancestors have been seen
    std::shared_ptr<Page> nearest_page = hierarchy->NearestLoadedPage(parents_list);
    // If none of the key's ancestors exist, then this key doesn't exist in the hierarchy
    // Or, if the nearest ancestor has already been loaded, that means the key isn't a node within that page.
    if (nearest_page == nullptr || nearest_page->loaded)
        return nullptr;

    // Load the page and add the subpages and page nodes
    auto children = ReadPage(nearest_page);
    for (Entry e : children)
    {
        if (e.IsPage())
            hierarchy->seen_pages_[e.key] = std::make_shared<Page>(e);
        else
            hierarchy->loaded_nodes_[e.key] = std::make_shared<Node>(e);
    }

    // Try to find the key again
    return FindNode(key);
}

std::vector<las::Point> Reader::GetPoints(Node node) 
{
    std::vector<char> point_data = GetPointData(node);
    return Node::UnpackPoints(point_data, file->GetLasHeader().point_format_id,
                              file->GetLasHeader().point_record_length);
}

std::vector<char> Reader::GetPointData(Node node)
{
    if (!node.IsValid())
        return {};

    in_stream.seekg(node.offset);

    auto las_header = file->GetLasHeader();
    std::vector<char> point_data = laz::Decompressor::DecompressBytes(in_stream, las_header, node.point_count);
    return point_data;
}



} // namespace copc::io