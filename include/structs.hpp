
#ifndef COPCLIB_COPC_STRUCTS_H_
#define COPCLIB_COPC_STRUCTS_H_

#include "types.hpp"
#include <cstdint>
#include <functional> // for hash
#include <sstream>

namespace copc
{

class VoxelKey
{
  public:
    VoxelKey(int32_t d, int32_t x, int32_t y, int32_t z) : d(d), x(x), y(y), z(z) {}
    VoxelKey() : VoxelKey(-1, -1, -1, -1) {}

    bool IsValid() const { return d >= 0 && x >= 0 && y >= 0 && z >= 0; }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << d << "-" << x << "-" << y << "-" << z;
        return ss.str();
    }

    int32_t &IdAt(uint64_t i)
    {
        switch (i)
        {
        case 0:
            return this->x;
        case 1:
            return this->y;
        case 2:
            return this->z;
        default:
            throw std::exception();
        }
    }

    // Returns the corresponding key depending on direction [0,7]
    VoxelKey Bisect(uint64_t direction) const
    {
        VoxelKey key(*this);
        ++key.d;

        auto step([&key, direction](uint8_t i) {
            key.IdAt(i) *= 2;
            const bool positive(direction & (((uint64_t)1) << i));
            if (positive)
                ++key.IdAt(i);
        });

        for (uint8_t i(0); i < 3; ++i)
            step(i);

        return key;
    }

    int32_t d;
    int32_t x;
    int32_t y;
    int32_t z;
};

inline bool operator==(const VoxelKey &a, const VoxelKey &b)
{
    return a.d == b.d && a.x == b.x && a.y == b.y && a.z == b.z;
}

inline bool operator!=(const VoxelKey &a, const VoxelKey &b) { return !(a == b); }

struct Entry
{
    Entry(VoxelKey key, uint64_t offset, int32_t byteSize, int32_t pointCount)
        : key(key), offset(offset), byteSize(byteSize), pointCount(pointCount)
    {
    }
    Entry() : Entry(VoxelKey(), 0, -1, -1) {}

    uint64_t GetDecompressedSize(uint16_t point_record_length) { return pointCount * point_record_length; }

    bool IsValid() const { return key.IsValid() && byteSize > -1; }

    bool IsSubpage() const { return pointCount == -1 && IsValid(); }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << key.ToString() << ": off=" << offset << ", size=" << byteSize << ", count=" << pointCount;
        return ss.str();
    }

    // EPT key of the data to which this entry corresponds
    VoxelKey key;

    // Absolute offset to the data chunk, or absolute Offset to a child hierarchy page
    // if the PointCount is -1
    uint64_t offset{};

    // Size of the data chunk in bytes (compressed size) or size of the child hierarchy page if
    // the PointCount is -1
    int32_t byteSize{};

    // Number of points in the data chunk, or -1 if the information
    // for this octree node and its descendants is contained in other hierarchy pages
    int32_t pointCount{};
};

struct CopcData
{
    CopcData() {}

    CopcData(io::CopcVlr vlr)
    {
        span = vlr.span;
        root_hier_offset = vlr.root_hier_offset;
        root_hier_size = vlr.root_hier_size;
        laz_vlr_offset = vlr.laz_vlr_offset;
        laz_vlr_size = vlr.laz_vlr_size;
        wkt_vlr_offset = vlr.wkt_vlr_offset;
        wkt_vlr_size = vlr.wkt_vlr_size;
        eb_vlr_offset = vlr.eb_vlr_offset;
        eb_vlr_size = vlr.eb_vlr_size;
    }

    int64_t span;              // Number of voxels in each spatial dimension
    uint64_t root_hier_offset; // File offset to the first hierarchy page
    uint64_t root_hier_size;   // Size of the first hierarchy page in bytes
    uint64_t laz_vlr_offset;   // File offset of the *data* of the LAZ VLR
    uint64_t laz_vlr_size;     // Size of the *data* of the LAZ VLR.
    uint64_t wkt_vlr_offset;   // File offset of the *data* of the WKT VLR if it exists, 0 otherwise
    uint64_t wkt_vlr_size;     // Size of the *data* of the WKT VLR if it exists, 0 otherwise
    uint64_t eb_vlr_offset;    // File offset of the *data* of the extra bytes VLR if it exists, 0 otherwise
    uint64_t eb_vlr_size;      // Size of the *data* of the extra bytes VLR if it exists, 0 otherwise
    uint64_t reserved[11];     // Reserved for future use. Must be 0.
};

struct Vector3
{
    float x, y, z;
};
} // namespace copc

namespace std
{
template <> struct hash<copc::VoxelKey>
{
    std::size_t operator()(copc::VoxelKey const &k) const noexcept
    {
        std::hash<uint64_t> h;

        uint64_t k1 = ((uint64_t)k.d << 32) | k.x;
        uint64_t k2 = ((uint64_t)k.y << 32) | k.z;
        return h(k1) ^ (h(k2) << 1);
    }
};
} // namespace std

#endif // COPCLIB_COPC_STRUCTS_H_