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
        VoxelKey key;
        reader_->in_stream.read(reinterpret_cast<char *>(&key.d), sizeof(key.d));
        reader_->in_stream.read(reinterpret_cast<char *>(&key.x), sizeof(key.x));
        reader_->in_stream.read(reinterpret_cast<char *>(&key.y), sizeof(key.y));
        reader_->in_stream.read(reinterpret_cast<char *>(&key.z), sizeof(key.z));

        int64_t offset;
        reader_->in_stream.read(reinterpret_cast<char *>(&offset), sizeof(offset));
        int32_t byteSize;
        reader_->in_stream.read(reinterpret_cast<char *>(&byteSize), sizeof(byteSize));
        int32_t pointCount;
        reader_->in_stream.read(reinterpret_cast<char *>(&pointCount), sizeof(pointCount));

        // Either create a Sub-Page or Node out of the entry
        if (pointCount == -1)
        {
            this->sub_pages[key] = std::make_shared<Page>(key, offset, byteSize, reader_);
        }
        else
        {
            this->nodes[key] = std::make_shared<Node>(key, offset, byteSize, pointCount, reader_);
        }
    }

    loaded = true;
}

} // namespace copc::hierarchy