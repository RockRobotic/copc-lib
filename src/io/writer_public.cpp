#include <cstring>

#include <copc-lib/hierarchy/internal/page.hpp>
#include <copc-lib/io/writer.hpp>
#include <copc-lib/las/point.hpp>

namespace copc
{

void Writer::InitWriter(std::ostream &out_stream, LasConfig const &config, const int &span, const std::string &wkt)
{
    auto header = HeaderFromConfig(config);
    this->file_ = std::make_shared<CopcFile>(header, span, wkt, config.extra_bytes);
    this->hierarchy_ = std::make_shared<Internal::Hierarchy>();
    this->writer_ = std::make_unique<Internal::WriterInternal>(out_stream, this->file_, this->hierarchy_);
}

// Create a page, add it to the hierarchy and reference it as a subpage in the parent
Page Writer::AddSubPage(Page &parent, VoxelKey key)
{
    if (!key.IsValid())
        throw std::runtime_error("Invalid key!");
    if (hierarchy_->PageExists(key))
        throw std::runtime_error("Page already exists!");
    if (!hierarchy_->PageExists(parent.key))
        throw std::runtime_error("Parent page does not exist!");
    if (!key.ChildOf(parent.key))
        throw std::runtime_error("Target key " + key.ToString() + " is not a child of page node " +
                                 parent.key.ToString());

    auto child_page = std::make_shared<Internal::PageInternal>(key);
    child_page->loaded = true;

    auto parent_page = hierarchy_->seen_pages_[parent.key];
    parent_page->sub_pages.push_back(child_page);
    hierarchy_->seen_pages_[key] = child_page;

    return *child_page;
}

// Writes a node to the file and reference it in the hierarchy and in the parent page
Node Writer::DoAddNode(Page &page, VoxelKey key, std::vector<char> in, uint64_t point_count, bool compressed)
{
    if (!page.IsPage() || !page.IsValid() || !key.IsValid())
        throw std::runtime_error("Invalid page or target key!");

    if (!key.ChildOf(page.key))
        throw std::runtime_error("Target key " + key.ToString() + " is not a child of page node " +
                                 page.key.ToString());

    Entry e = writer_->WriteNode(std::move(in), point_count, compressed);
    e.key = key;

    auto node = std::make_shared<Node>(e);
    hierarchy_->loaded_nodes_[key] = node;
    hierarchy_->seen_pages_[page.key]->nodes.push_back(node);
    return *node;
}

Node Writer::AddNode(Page &page, const VoxelKey &key, las::Points &points)
{
    auto header = file_->GetLasHeader();
    if (points.PointFormatID() != header.point_format_id || points.PointRecordLength() != header.point_record_length)
        throw std::runtime_error("New points must be of same format and size.");

    std::vector<char> uncompressed = points.Pack();
    return AddNode(page, key, uncompressed);
}

Node Writer::AddNode(Page &page, const VoxelKey &key, std::vector<char> const &uncompressed)
{
    int point_size = file_->GetLasHeader().point_record_length;
    if (uncompressed.size() < point_size || uncompressed.size() % point_size != 0)
        throw std::runtime_error("Invalid point data array!");

    return DoAddNode(page, key, uncompressed, 0, false);
}

Node Writer::AddNodeCompressed(Page &page, const VoxelKey &key, std::vector<char> const &compressed,
                               uint64_t point_count)
{
    if (point_count == 0)
        throw std::runtime_error("Point count must be >0!");

    return DoAddNode(page, key, compressed, point_count, true);
}

uint8_t EXTRA_BYTE_DATA_TYPE[31]{0, 1,  1,  2, 2,  4, 4, 8, 8, 4,  8,  2,  2,  4,  4, 8,
                                 8, 16, 16, 8, 16, 3, 3, 6, 6, 12, 12, 24, 24, 12, 24};

int Writer::NumBytesFromExtraBytes(const std::vector<las::EbVlr::ebfield> &items)
{
    int out = 0;
    for (const auto &item : items)
    {
        if (item.data_type == 0)
            out += item.options;
        else
            out += EXTRA_BYTE_DATA_TYPE[item.data_type];
    }
    return out;
}

las::LasHeader Writer::HeaderFromConfig(LasConfig const &config)
{
    las::LasHeader h;
    h.file_source_id = config.file_source_id;
    h.global_encoding = config.global_encoding;
    h.creation.day = config.creation.day;
    h.creation.year = config.creation.year;
    h.point_format_id = config.point_format_id;
    h.point_record_length =
        las::PointBaseByteSize(config.point_format_id) + NumBytesFromExtraBytes(config.extra_bytes.items);

    std::strcpy(h.guid, config.GUID().c_str());
    std::strcpy(h.system_identifier, config.SystemIdentifier().c_str());
    std::strcpy(h.generating_software, config.GeneratingSoftware().c_str());

    h.offset.x = config.offset.x;
    h.offset.y = config.offset.y;
    h.offset.z = config.offset.z;

    h.scale.x = config.scale.x;
    h.scale.y = config.scale.y;
    h.scale.z = config.scale.z;

    h.maxx = config.max.x;
    h.minx = config.min.x;
    h.maxy = config.max.y;
    h.miny = config.min.y;
    h.maxz = config.max.z;
    h.minz = config.min.z;

    std::copy(std::begin(config.points_by_return_14), std::end(config.points_by_return_14),
              std::begin(h.points_by_return_14));
    return h;
}

copc::Writer::LasConfig::LasConfig(const las::LasHeader &config, const las::EbVlr &extra_bytes_)
{
    file_source_id = config.file_source_id;
    global_encoding = config.global_encoding;
    creation.day = config.creation.day;
    creation.year = config.creation.year;
    point_format_id = config.point_format_id;

    guid_ = config.guid;
    system_identifier_ = config.system_identifier;
    generating_software_ = config.generating_software;

    offset.x = config.offset.x;
    offset.y = config.offset.y;
    offset.z = config.offset.z;

    scale.x = config.scale.x;
    scale.y = config.scale.y;
    scale.z = config.scale.z;

    max.x = config.maxx;
    min.x = config.minx;
    max.y = config.maxy;
    min.y = config.miny;
    max.z = config.maxz;
    min.z = config.minz;

    std::copy(std::begin(config.points_by_return_14), std::end(config.points_by_return_14),
              std::begin(points_by_return_14));
    extra_bytes = extra_bytes_;
}
} // namespace copc
