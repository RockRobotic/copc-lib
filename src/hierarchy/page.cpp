#include <copc-lib/hierarchy/hierarchy.hpp>
#include <copc-lib/hierarchy/page.hpp>

namespace copc::hierarchy
{
void Page::Load()
{
    if (!this->IsValid())
        throw std::runtime_error("Cannot load an invalid page.");
    // reset the stream to the page's offset
    reader_->in_stream.seekg(offset_);

    // Iterate through each Entry in the page
    int num_entries = int(size_ / ENTRY_SIZE);
    for (int i = 0; i < num_entries; i++)
    {
        Entry e = Entry::Unpack(reader_->in_stream);

        // Either create a Sub-Page or Node out of the entry
        if (e.point_count == -1)
        {
            this->sub_pages[key] = std::make_shared<Page>(e, reader_);
        }
        else
        {
            this->nodes[key] = std::make_shared<Node>(e, reader_);
        }
    }

    loaded = true;
}

bool Page::InsertNode(VoxelKey key, std::vector<las::Point> points)
{
    std::vector<char> uncompressed = Node::PackPoints(points);
    auto entry = writer_->WriteNode(uncompressed);
    auto node = std::make_shared<Node>(key, entry.offset, entry.size, entry.point_count);
    this->nodes[key] = node;
}

// std::shared_ptr<Page> Page::GetSubPage(VoxelKey key) { return std::shared_ptr<Page>(); }
//
// std::shared_ptr<Node> Page::GetNode(VoxelKey key) { return std::shared_ptr<Node>(); }

} // namespace copc::hierarchy