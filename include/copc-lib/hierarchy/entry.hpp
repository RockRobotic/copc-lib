#ifndef COPCLIB_HIERARCHY_ENTRY_H_
#define COPCLIB_HIERARCHY_ENTRY_H_

#include <ostream>
#include <vector>

#include <copc-lib/hierarchy/key.hpp>
#include <copc-lib/las/point.hpp>

namespace copc
{
class Entry;
class Entry
{
  public:
    Entry() : offset(-1), size(-1), key(VoxelKey()), point_count(-1){};
    Entry(VoxelKey key, int64_t offset, int32_t size, int32_t point_count)
        : offset(offset), size(size), key(key), point_count(point_count){};

    bool IsValid() const { return offset >= 0 && size >= 0 && key.IsValid(); }
    bool IsPage() const { return IsValid() && point_count == -1; }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "Entry " << key.ToString() << ": off=" << offset << ", size=" << size << ", count=" << point_count;
        return ss.str();
    }

    void Pack(std::ostream &out_stream)
    {
        out_stream.write(reinterpret_cast<char *>(&key.d), sizeof(key.d));
        out_stream.write(reinterpret_cast<char *>(&key.x), sizeof(key.x));
        out_stream.write(reinterpret_cast<char *>(&key.y), sizeof(key.y));
        out_stream.write(reinterpret_cast<char *>(&key.z), sizeof(key.z));

        out_stream.write(reinterpret_cast<char *>(&offset), sizeof(offset));
        out_stream.write(reinterpret_cast<char *>(&size), sizeof(size));
        out_stream.write(reinterpret_cast<char *>(&point_count), sizeof(point_count));
    }

    static Entry Unpack(std::istream &in_stream)
    {
        VoxelKey key;
        in_stream.read(reinterpret_cast<char *>(&key.d), sizeof(key.d));
        in_stream.read(reinterpret_cast<char *>(&key.x), sizeof(key.x));
        in_stream.read(reinterpret_cast<char *>(&key.y), sizeof(key.y));
        in_stream.read(reinterpret_cast<char *>(&key.z), sizeof(key.z));

        int64_t offset;
        in_stream.read(reinterpret_cast<char *>(&offset), sizeof(offset));
        int32_t size;
        in_stream.read(reinterpret_cast<char *>(&size), sizeof(size));
        int32_t point_count;
        in_stream.read(reinterpret_cast<char *>(&point_count), sizeof(point_count));

        return Entry(key, offset, size, point_count);
    }

    int32_t point_count;
    VoxelKey key;
    int64_t offset;
    int32_t size;
};

} // namespace copc

#endif // COPCLIB_HIERARCHY_ENTRY_H_