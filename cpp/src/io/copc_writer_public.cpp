#include "copc-lib/hierarchy/internal/hierarchy.hpp"
#include "copc-lib/hierarchy/internal/page.hpp"
#include "copc-lib/io/copc_writer.hpp"
#include "copc-lib/io/internal/copc_writer_internal.hpp"
#include "copc-lib/las/point.hpp"
#include "copc-lib/laz/decompressor.hpp"

namespace copc
{

void Writer::InitWriter(std::ostream &out_stream, const CopcConfigWriter &copc_config_writer,
                        const std::optional<int8_t> &point_format_id, const std::optional<Vector3> &scale,
                        const std::optional<Vector3> &offset, const std::optional<std::string> &wkt,
                        const std::optional<las::EbVlr> &extra_bytes_vlr, const std::optional<bool> &has_extended_stats)
{

    if (point_format_id || scale || offset || wkt || extra_bytes_vlr || has_extended_stats)
    {
        // If we need to update either parameter we need to create a new ConfigFileWriter
        auto new_point_format_id = copc_config_writer.LasHeader().PointFormatId();
        if (point_format_id)
            new_point_format_id = *point_format_id;

        auto new_scale = copc_config_writer.LasHeader().Scale();
        if (scale)
            new_scale = *scale;

        auto new_offset = copc_config_writer.LasHeader().Offset();
        if (offset)
            new_offset = *offset;

        auto new_wkt = copc_config_writer.Wkt();
        if (wkt)
            new_wkt = *wkt;

        auto new_extra_bytes_vlr = copc_config_writer.ExtraBytesVlr();
        if (extra_bytes_vlr)
            new_extra_bytes_vlr = *extra_bytes_vlr;

        auto new_has_extended_stats = copc_config_writer.CopcExtents().HasExtendedStats();
        if (has_extended_stats)
            new_has_extended_stats = *has_extended_stats;

        las::LasHeader new_header(copc_config_writer.LasHeader(), new_point_format_id,
                                  las::PointBaseByteSize(new_point_format_id) +
                                      las::NumBytesFromExtraBytes(new_extra_bytes_vlr.items),
                                  new_scale, new_offset);

        copc::CopcExtents new_extents(copc_config_writer.CopcExtents(), new_point_format_id,
                                      new_extra_bytes_vlr.items.size(), new_has_extended_stats);

        copc::CopcConfigWriter new_copc_config_writer(new_header, copc_config_writer.CopcInfo(), new_extents, new_wkt,
                                                      new_extra_bytes_vlr);

        this->config_ = std::make_shared<CopcConfigWriter>(new_copc_config_writer);
    }
    else
    {
        // If not we use the copc_config_writer provided
        this->config_ = std::make_shared<CopcConfigWriter>(copc_config_writer);
    }
    this->hierarchy_ = std::make_shared<Internal::Hierarchy>();
    this->writer_ = std::make_unique<Internal::WriterInternal>(out_stream, this->config_, this->hierarchy_);
}

void Writer::Close()
{
    if (writer_ != nullptr)
        writer_->Close();
}

bool Writer::PageExists(const VoxelKey &key) { return hierarchy_->PageExists(key); }

// Writes a node to the file and reference it in the hierarchy and in the parent page
Node Writer::DoAddNode(const VoxelKey &key, const std::vector<char> &in, int32_t point_count, bool compressed_data,
                       const VoxelKey &page_key)
{
    if (!page_key.IsValid() || !key.IsValid())
        throw std::runtime_error("Invalid page or node key!");
    // TODO[leo]: Check if node already loaded

    if (!key.ChildOf(page_key))
        throw std::runtime_error("Target key " + key.ToString() + " is not a child of page node " + key.ToString());

    Entry e = writer_->WriteNode(in, point_count, compressed_data);
    e.key = key;

    auto node = std::make_shared<Node>(e, page_key);
    hierarchy_->loaded_nodes_[key] = node;
    // If page doesn't exist then create it
    if (!PageExists(page_key))
    {
        auto new_page = std::make_shared<Internal::PageInternal>(page_key);
        new_page->loaded = true;
        hierarchy_->seen_pages_[page_key] = new_page;
    }
    // Add node to page
    hierarchy_->seen_pages_[page_key]->nodes[node->key] = node;
    return *node;
}

Node Writer::AddNode(const VoxelKey &key, const las::Points &points, const VoxelKey &page_key)
{
    if (points.Size() == 0)
        throw std::runtime_error("Writer::AddNode: Cannot add empty las::Points.");
    if (points.PointFormatId() != config_->LasHeader()->PointFormatId() ||
        points.PointRecordLength() != config_->LasHeader()->PointRecordLength())
        throw std::runtime_error("Writer::AddNode: New points must be of same format and size.");

    std::vector<char> uncompressed_data = points.Pack(*config_->LasHeader());
    return AddNode(key, uncompressed_data, page_key);
}

Node Writer::AddNode(const VoxelKey &key, std::vector<char> const &uncompressed_data, const VoxelKey &page_key)
{
    int point_size = config_->LasHeader()->PointRecordLength();

    if (uncompressed_data.empty())
        throw std::runtime_error("Writer::AddNode: Empty point data array.");
    if (uncompressed_data.size() % point_size != 0)
        throw std::runtime_error("Writer::AddNode: Invalid point data array.");

    return DoAddNode(key, uncompressed_data, 0, false, page_key);
}

Node Writer::AddNodeCompressed(const VoxelKey &key, std::vector<char> const &compressed_data, int32_t point_count,
                               const VoxelKey &page_key)
{
    if (point_count == 0)
        throw std::runtime_error("Point count must be >0!");

    return DoAddNode(key, compressed_data, point_count, true, page_key);
}

void Writer::ChangeNodePage(const VoxelKey &node_key, const VoxelKey &new_page_key)
{
    if (!node_key.IsValid())
        throw std::runtime_error("Writer::ChangeNodePage: Node Key " + node_key.ToString() + " is invalid.");
    if (!new_page_key.IsValid())
        throw std::runtime_error("Writer::ChangeNodePage: New Page Key " + node_key.ToString() + " is invalid.");
    if (hierarchy_->loaded_nodes_.find(node_key) == hierarchy_->loaded_nodes_.end())
        throw std::runtime_error("Writer::ChangeNodePage: Node Key " + node_key.ToString() + " does not exist.");
    if (!node_key.ChildOf(new_page_key))
        throw std::runtime_error("Writer::ChangeNodePage: Node Key " + node_key.ToString() +
                                 " is not a child of New Page Key " + new_page_key.ToString() + ".");

    // Load the Node object based on the key
    auto node = hierarchy_->loaded_nodes_[node_key];

    // If new page is current page then do nothing
    if (node->page_key == new_page_key)
        return;

    // If new page doesn't exist then create it
    if (!PageExists(new_page_key))
    {
        auto new_page = std::make_shared<Internal::PageInternal>(new_page_key);
        new_page->loaded = true;
        hierarchy_->seen_pages_[new_page_key] = new_page;
    }

    // Add node to new page
    hierarchy_->seen_pages_[new_page_key]->nodes[node_key] = node;

    // Remove node from old page
    hierarchy_->seen_pages_[node->page_key]->nodes.erase(node_key);

    // If old page has no nodes left then remove it, unless it is root page
    if (node->page_key != VoxelKey::RootKey() && hierarchy_->seen_pages_[node->page_key]->nodes.empty())
        hierarchy_->seen_pages_.erase(node->page_key);
}

void FileWriter::Close()
{
    if (writer_ != nullptr)
        writer_->Close();
    laz::BaseFileWriter::Close();
}

} // namespace copc
