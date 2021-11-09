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

bool Writer::PageExists(const VoxelKey &key)
{
    return !(hierarchy_->seen_pages_.find(key) == hierarchy_->seen_pages_.end());
}
Page Writer::GetPage(const VoxelKey &key)
{
    if (!PageExists(key))
        throw std::runtime_error("Writer::GetPage: Requested page does not exist.");
    return *hierarchy_->seen_pages_[key];
}
Page Writer::GetRootPage() { return GetPage(VoxelKey::RootKey()); }

// Create a page, add it to the hierarchy and reference it as a subpage in the parent
Page Writer::AddSubPage(const Page &parent, const VoxelKey &key)
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
Node Writer::DoAddNode(const Page &page, VoxelKey key, std::vector<char> in, uint64_t point_count, bool compressed)
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

Node Writer::AddNode(const Page &page, const VoxelKey &key, las::Points &points)
{
    if (points.PointFormatId() != config_->LasHeader()->PointFormatId() ||
        points.PointRecordLength() != config_->LasHeader()->PointRecordLength())
        throw std::runtime_error("Writer::AddNode: New points must be of same format and size.");

    std::vector<char> uncompressed = points.Pack();
    return AddNode(page, key, uncompressed);
}

Node Writer::AddNode(const Page &page, const VoxelKey &key, std::vector<char> const &uncompressed)
{
    int point_size = config_->LasHeader()->PointRecordLength();
    if (uncompressed.size() < point_size || uncompressed.size() % point_size != 0)
        throw std::runtime_error("Invalid point data array!");

    return DoAddNode(page, key, uncompressed, 0, false);
}

Node Writer::AddNodeCompressed(const Page &page, const VoxelKey &key, std::vector<char> const &compressed,
                               uint64_t point_count)
{
    if (point_count == 0)
        throw std::runtime_error("Point count must be >0!");

    return DoAddNode(page, key, compressed, point_count, true);
}

void FileWriter::Close()
{
    writer_->Close();
    f_stream_.close();
}

} // namespace copc
