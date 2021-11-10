#include "copc-lib/hierarchy/internal/hierarchy.hpp"
#include "copc-lib/hierarchy/internal/page.hpp"
#include "copc-lib/io/internal/writer_internal.hpp"
#include "copc-lib/io/writer.hpp"
#include "copc-lib/las/point.hpp"
#include "copc-lib/laz/decompressor.hpp"

namespace copc
{

void Writer::InitWriter(std::ostream &out_stream, const CopcConfigWriter &copc_file_writer)
{
    this->config_ = std::make_shared<CopcConfigWriter>(copc_file_writer);
    this->hierarchy_ = std::make_shared<Internal::Hierarchy>();
    this->writer_ = std::make_unique<Internal::WriterInternal>(out_stream, this->config_, this->hierarchy_);
}

Writer::~Writer() { writer_->Close(); }
void Writer::Close() { writer_->Close(); }

// Create a page, add it to the hierarchy and reference it as a subpage in the parent
Page Writer::AddSubPage(const VoxelKey &parent_key, const VoxelKey &key)
{
    if (!key.IsValid())
        throw std::runtime_error("Invalid key!");
    if (hierarchy_->PageExists(key))
        throw std::runtime_error("Page already exists!");
    if (!hierarchy_->PageExists(parent_key))
        throw std::runtime_error("Parent page does not exist!");
    if (!key.ChildOf(parent_key))
        throw std::runtime_error("Target key " + key.ToString() + " is not a child of page node " +
                                 parent_key.ToString());

    auto child_page = std::make_shared<Internal::PageInternal>(key);
    child_page->loaded = true;

    auto parent_page = hierarchy_->seen_pages_[parent_key];
    parent_page->sub_pages.push_back(child_page);
    hierarchy_->seen_pages_[key] = child_page;

    return *child_page;
}

bool Writer::PageExists(const VoxelKey &key) { return hierarchy_->PageExists(key); }

// Writes a node to the file and reference it in the hierarchy and in the parent page
Node Writer::DoAddNode(const VoxelKey &key, std::vector<char> in, uint64_t point_count, bool compressed_data,
                       const VoxelKey &page_key)
{
    if (!page_key.IsValid() || !key.IsValid())
        throw std::runtime_error("Invalid page or node key!");
    // TODO[leo]: Check if node already loaded

    if (!key.ChildOf(page_key))
        throw std::runtime_error("Target key " + key.ToString() + " is not a child of page node " + key.ToString());

    Entry e = writer_->WriteNode(std::move(in), point_count, compressed_data);
    e.key = key;

    auto node = std::make_shared<Node>(e);
    hierarchy_->loaded_nodes_[key] = node;
    // If page doesn't exist then create it with the nearest existing parent as parent page
    if (!PageExists(page_key))
    {
        // Find the nearest existing parent page
        auto parent = key.GetParent();
        while (!PageExists(parent))
            parent = parent.GetParent();
        // Add page
        AddSubPage(parent, page_key);
    }
    // Add node to page
    hierarchy_->seen_pages_[page_key]->nodes.push_back(node);
    return *node;
}

Node Writer::AddNode(const VoxelKey &key, las::Points &points, const VoxelKey &page_key)
{
    if (points.PointFormatId() != config_->LasHeader()->PointFormatId() ||
        points.PointRecordLength() != config_->LasHeader()->PointRecordLength())
        throw std::runtime_error("Writer::AddNode: New points must be of same format and size.");

    std::vector<char> uncompressed_data = points.Pack();
    return AddNode(key, uncompressed_data, page_key);
}

Node Writer::AddNode(const VoxelKey &key, std::vector<char> const &uncompressed_data, const VoxelKey &page_key)
{
    int point_size = config_->LasHeader()->PointRecordLength();
    if (uncompressed_data.size() < point_size || uncompressed_data.size() % point_size != 0)
        throw std::runtime_error("Invalid point data array!");

    return DoAddNode(key, uncompressed_data, 0, false, page_key);
}

Node Writer::AddNodeCompressed(const VoxelKey &key, std::vector<char> const &compressed_data, uint64_t point_count,
                               const VoxelKey &page_key)
{
    if (point_count == 0)
        throw std::runtime_error("Point count must be >0!");

    return DoAddNode(key, compressed_data, point_count, true, page_key);
}

void FileWriter::Close()
{
    writer_->Close();
    f_stream_.close();
}

} // namespace copc
