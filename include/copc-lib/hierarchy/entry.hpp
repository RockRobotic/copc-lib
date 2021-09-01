#ifndef COPCLIB_HIERARCHY_ENTRY_H_
#define COPCLIB_HIERARCHY_ENTRY_H_

#include <ostream>
#include <vector>

#include <copc-lib/hierarchy/key.hpp>
#include <copc-lib/io/reader.hpp>
#include <copc-lib/las/point.hpp>

namespace copc::hierarchy
{
class Entry;
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

    void Pack(std::ostream &out_stream)
    {
        out_stream.write(reinterpret_cast<char *>(&key.d), sizeof(key.d));
        out_stream.write(reinterpret_cast<char *>(&key.x), sizeof(key.x));
        out_stream.write(reinterpret_cast<char *>(&key.y), sizeof(key.y));
        out_stream.write(reinterpret_cast<char *>(&key.z), sizeof(key.z));

        out_stream.write(reinterpret_cast<char *>(&offset_), sizeof(offset_));
        out_stream.write(reinterpret_cast<char *>(&size_), sizeof(size_));
        out_stream.write(reinterpret_cast<char *>(&point_count), sizeof(point_count));
    }

    static Entry Unpack(std::istream &in_stream)
    {
        VoxelKey key;
        in_stream.read(reinterpret_cast<char *>(&key.d), sizeof(key.d));
        in_stream.read(reinterpret_cast<char *>(&key.x), sizeof(key.x));
        in_stream.read(reinterpret_cast<char *>(&key.y), sizeof(key.y));
        in_stream.read(reinterpret_cast<char *>(&key.z), sizeof(key.z));

        int64_t offset_;
        in_stream.read(reinterpret_cast<char *>(&offset_), sizeof(offset_));
        int32_t size_;
        in_stream.read(reinterpret_cast<char *>(&size_), sizeof(size_));
        int32_t point_count;
        in_stream.read(reinterpret_cast<char *>(&point_count), sizeof(point_count));

        return Entry(key, offset_, size_, point_count);
    }

    int32_t point_count;
    VoxelKey key;

  protected:
    int64_t offset_;
    int32_t size_;
};

} // namespace copc::hierarchy

#endif // COPCLIB_HIERARCHY_ENTRY_H_