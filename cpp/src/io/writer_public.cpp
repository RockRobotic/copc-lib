#include "copc-lib/hierarchy/internal/hierarchy.hpp"
#include "copc-lib/hierarchy/internal/page.hpp"
#include "copc-lib/io/internal/writer_internal.hpp"
#include "copc-lib/io/writer.hpp"
#include "copc-lib/las/point.hpp"

namespace copc
{

Writer::LasHeaderConfig::LasHeaderConfig(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset)
    : LasHeaderBase(point_format_id, scale, offset)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("LasConfig: Supported point formats are 6 to 8.");
};

Writer::LasHeaderConfig::LasHeaderConfig(const las::LasHeader &config, const las::EbVlr &extra_bytes)
{
    file_source_id = config.file_source_id;
    global_encoding = config.global_encoding;
    creation_day = config.creation_day;
    creation_year = config.creation_year;
    point_format_id = config.point_format_id;

    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("LasConfig: Supported point formats are 6 to 8.");

    guid_ = config.GUID();
    system_identifier_ = config.SystemIdentifier();
    generating_software_ = config.GeneratingSoftware();

    offset = config.offset;
    scale = config.scale;

    max = config.max;
    min = config.min;

    points_by_return = config.points_by_return;
    this->extra_bytes = extra_bytes;
}

std::string Writer::LasHeaderConfig::ToString() const
{
    std::stringstream ss;
    ss << "LasConfig:" << std::endl;
    ss << "\tFile Source ID: " << file_source_id << std::endl;
    ss << "\tGlobal Encoding ID: " << global_encoding << std::endl;
    ss << "\tGUID: " << GUID() << std::endl;
    ss << "\tSystem Identifier: " << SystemIdentifier() << std::endl;
    ss << "\tGenerating Software: " << GeneratingSoftware() << std::endl;
    ss << "\tCreation (DD/YYYY): (" << creation_day << "/" << creation_year << ")" << std::endl;
    ss << "\tPoint Format ID: " << static_cast<int>(point_format_id) << std::endl;
    ss << "\tScale: " << scale.ToString() << std::endl;
    ss << "\tOffset: " << offset.ToString() << std::endl;
    ss << "\tMax: " << max.ToString() << std::endl;
    ss << "\tMin: " << min.ToString() << std::endl;
    ss << "\tPoints By Return:" << std::endl;
    for (int i = 0; i < points_by_return.size(); i++)
        ss << "\t\t [" << i << "]: " << points_by_return[i] << std::endl;
    ss << "\tNumber of Extra Bytes: " << NumExtraBytes() << std::endl;
    return ss.str();
}

void Writer::InitWriter(std::ostream &out_stream, LasHeaderConfig const &config, const int &spacing,
                        const std::string &wkt)
{
    auto header = HeaderFromConfig(config);
    this->file_ = std::make_shared<CopcFile>(header, spacing, wkt, config.extra_bytes);
    this->hierarchy_ = std::make_shared<Internal::Hierarchy>();
    this->writer_ = std::make_unique<Internal::WriterInternal>(out_stream, this->file_, this->hierarchy_);
}

Writer::~Writer() { writer_->Close(); }
void Writer::Close() { writer_->Close(); }

Page Writer::GetRootPage() { return *this->hierarchy_->seen_pages_[VoxelKey::BaseKey()]; }

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

las::LasHeader Writer::HeaderFromConfig(LasHeaderConfig const &config)
{
    las::LasHeader h;
    h.file_source_id = config.file_source_id;
    h.global_encoding = config.global_encoding;
    h.creation_day = config.creation_day;
    h.creation_year = config.creation_year;
    h.point_format_id = config.point_format_id;
    h.point_record_length =
        las::PointBaseByteSize(config.point_format_id) + NumBytesFromExtraBytes(config.extra_bytes.items);

    h.GUID(config.GUID());
    h.SystemIdentifier(config.SystemIdentifier());
    h.GeneratingSoftware(config.GeneratingSoftware());

    h.offset = config.offset;
    h.scale = config.scale;
    h.max = config.max;
    h.min = config.min;

    h.points_by_return = config.points_by_return;
    return h;
}

void FileWriter::Close()
{
    writer_->Close();
    f_stream_.close();
} // namespace copc

}
