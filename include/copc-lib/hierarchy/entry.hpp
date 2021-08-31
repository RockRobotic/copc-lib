#ifndef COPCLIB_HIERARCHY_ENTRY_H_
#define COPCLIB_HIERARCHY_ENTRY_H_

#include <vector>

#include <copc-lib/hierarchy/key.hpp>
#include <copc-lib/io/reader.hpp>
#include <copc-lib/las/point.hpp>

namespace copc::hierarchy
{

class Entry
{
  public:
    Entry() : offset_(-1), size_(-1), key(VoxelKey()), point_count(-1){};
    Entry(VoxelKey key, int64_t offset, int32_t size, int32_t point_count)
        : offset_(offset), size_(size), key(key), point_count(point_count){};

    bool IsValid() const { return offset_ >= 0 && size_ >= 0 && key.IsValid(); }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "Entry " << key.ToString() << ": off=" << offset_ << ", size=" << size_ << ", count=" << point_count;
        return ss.str();
    }

    int32_t point_count;
    VoxelKey key;

  protected:
    int64_t offset_;
    int32_t size_;
};

} // namespace copc::hierarchy

#endif // COPCLIB_HIERARCHY_ENTRY_H_