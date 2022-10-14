#ifndef COPCLIB_HIERARCHY_ENTRY_H_
#define COPCLIB_HIERARCHY_ENTRY_H_

#include <ostream>
#include <vector>

#include "copc-lib/hierarchy/key.hpp"
#include "copc-lib/las/point.hpp"

namespace copc
{
class Entry;
// Entry class is a base class for Node and Page objects
class Entry
{
  public:
    static const int ENTRY_SIZE = 32;

    Entry() : offset(-1), byte_size(-1), key(VoxelKey::InvalidKey()), point_count(-1){};
    Entry(VoxelKey key, uint64_t offset, int32_t size, int32_t point_count)
        : offset(offset), byte_size(size), key(key), point_count(point_count){};

    virtual bool IsValid() const { return offset >= 0 && byte_size >= 0 && key.IsValid(); }
    virtual bool IsPage() const { return IsValid() && point_count == -1; }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "Entry " << key.ToString() << ": off=" << offset << ", size=" << byte_size << ", count=" << point_count
           << ", is_valid=" << IsValid();
        return ss.str();
    }
    friend std::ostream &operator<<(std::ostream &os, Entry const &value)
    {
        os << value.ToString();
        return os;
    }

    void Pack(std::ostream &out_stream)
    {
        out_stream.write(reinterpret_cast<char *>(&key.d), sizeof(key.d));
        out_stream.write(reinterpret_cast<char *>(&key.x), sizeof(key.x));
        out_stream.write(reinterpret_cast<char *>(&key.y), sizeof(key.y));
        out_stream.write(reinterpret_cast<char *>(&key.z), sizeof(key.z));

        out_stream.write(reinterpret_cast<char *>(&offset), sizeof(offset));
        out_stream.write(reinterpret_cast<char *>(&byte_size), sizeof(byte_size));
        out_stream.write(reinterpret_cast<char *>(&point_count), sizeof(point_count));
    }

    static Entry Unpack(std::istream &in_stream)
    {
        VoxelKey key;
        in_stream.read(reinterpret_cast<char *>(&key.d), sizeof(key.d));
        in_stream.read(reinterpret_cast<char *>(&key.x), sizeof(key.x));
        in_stream.read(reinterpret_cast<char *>(&key.y), sizeof(key.y));
        in_stream.read(reinterpret_cast<char *>(&key.z), sizeof(key.z));

        uint64_t offset;
        in_stream.read(reinterpret_cast<char *>(&offset), sizeof(offset));
        int32_t size;
        in_stream.read(reinterpret_cast<char *>(&size), sizeof(size));
        int32_t point_count;
        in_stream.read(reinterpret_cast<char *>(&point_count), sizeof(point_count));

        return Entry(key, offset, size, point_count);
    }

    VoxelKey key;
    uint64_t offset;
    int32_t byte_size;
    int32_t point_count;

  protected:
    // Helper function for the equality comparisons of children classes
    bool IsEqual(const Entry &rhs) const
    {
        return offset == rhs.offset && byte_size == rhs.byte_size && point_count == rhs.point_count && key == rhs.key;
    }
};

} // namespace copc

#endif // COPCLIB_HIERARCHY_ENTRY_H_
