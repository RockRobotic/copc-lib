#include <cstring>

#include <copc-lib/hierarchy/internal/page.hpp>
#include <copc-lib/io/writer.hpp>
#include <copc-lib/las/point.hpp>

namespace copc
{

Writer::Writer(std::ostream &out_stream, LasConfig const &config, int span, std::string wkt)
{
    auto header = HeaderFromConfig(config);
    this->file = std::make_shared<CopcFile>(header, span, wkt);
    this->hierarchy = std::make_shared<Hierarchy>();
    this->writer_ = std::make_unique<WriterInternal>(out_stream, this->file, this->hierarchy);
}

void Writer::Close() { this->writer_->Close(); }

Page Writer::AddPage(VoxelKey key)
{
    if (!key.IsValid())
        throw std::runtime_error("Invalid key!");

    auto page = std::make_shared<PageInternal>(key);
    page->loaded = true;
    hierarchy->seen_pages_[page->key] = page;
    return *page;
}

Node Writer::DoAddNode(Page &page, VoxelKey key, std::vector<char> in, uint64_t point_count, bool compressed)
{
    if (!page.IsPage() || !page.IsValid() || !key.IsValid())
        throw std::runtime_error("Invalid page or target key!");

    Entry e = writer_->WriteNode(in, point_count, compressed);
    e.key = key;

    auto node = std::make_shared<Node>(e);
    hierarchy->loaded_nodes_[key] = node;
    hierarchy->seen_pages_[page.key]->nodes.push_back(node);
    return *node;
}

Node Writer::AddNode(Page &page, VoxelKey key, std::vector<las::Point> const &points)
{
    std::vector<char> uncompressed = Node::PackPoints(points);
    return AddNode(page, key, uncompressed);
}

Node Writer::AddNode(Page &page, VoxelKey key, std::vector<char> const &uncompressed)
{
    return DoAddNode(page, key, uncompressed, 0, false);
}

Node Writer::AddNodeCompressed(Page &page, VoxelKey key, std::vector<char> const &compressed, uint64_t point_count)
{
    return DoAddNode(page, key, compressed, point_count, true);
}

las::LasHeader Writer::HeaderFromConfig(LasConfig const &config)
{
    las::LasHeader h;
    h.file_source_id = config.file_source_id;
    h.global_encoding = config.global_encoding;
    h.creation.day = config.creation.day;
    h.creation.year = config.creation.year;
    h.point_format_id = config.point_format_id;
    h.point_record_length = las::Point::BaseByteSize(config.point_format_id); // +extra_bytes;

    std::strcpy(h.guid, config.guid);
    std::strcpy(h.system_identifier, config.system_identifier);
    std::strcpy(h.generating_software, config.generating_software);

    h.offset.x = config.offset.x;
    h.offset.y = config.offset.y;
    h.offset.z = config.offset.z;

    h.scale.x = config.scale.x;
    h.scale.y = config.scale.y;
    h.scale.z = config.scale.z;

    h.maxx = 0;
    h.minx = 0;
    h.maxy = 0;
    h.miny = 0;
    h.maxz = 0;
    h.minz = 0;

    return h;
}
} // namespace copc