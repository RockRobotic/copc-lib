#ifndef COPCLIB_HIERARCHY_KEY_H_
#define COPCLIB_HIERARCHY_KEY_H_

#include <cstdint>
#include <functional> // for hash
#include <limits>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "copc-lib/copc/info.hpp"
#include "copc-lib/geometry/box.hpp"
#include "copc-lib/geometry/vector3.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc
{

namespace las
{
class LasHeader;
}

class Box;
class VoxelKey
{
  public:
    VoxelKey(int32_t d, int32_t x, int32_t y, int32_t z) : d(d), x(x), y(y), z(z) {}
    VoxelKey() : VoxelKey(-1, -1, -1, -1) {}
    VoxelKey(const std::vector<int32_t> &vec)
    {
        if (vec.size() != 4)
            throw std::runtime_error("Vector size must be 4.");
        d = vec[0];
        x = vec[1];
        y = vec[2];
        z = vec[3];
    };

    static VoxelKey InvalidKey() { return VoxelKey(); }
    static VoxelKey RootKey() { return VoxelKey(0, 0, 0, 0); }

    bool IsValid() const { return d >= 0 && x >= 0 && y >= 0 && z >= 0; }

    std::string ToString() const;
    friend std::ostream &operator<<(std::ostream &os, VoxelKey const &value)
    {
        os << value.ToString();
        return os;
    }

    // Returns the corresponding key depending on direction [0,7]
    VoxelKey Bisect(const uint64_t &direction) const;
    std::vector<VoxelKey> GetChildren() const;

    // The hierarchial parent of this key
    VoxelKey GetParent() const;

    // The hierarchial parent of this key at requested depth
    VoxelKey GetParentAtDepth(int32_t depth) const;

    // A list of the key's parents from the key to the root node
    // optionally including the key itself
    std::vector<VoxelKey> GetParents(bool include_self = false) const;

    // Tests whether the current key is a child of a given key
    bool ChildOf(VoxelKey parent_key) const;

    // Spatial query functions
    // Definitions taken from https://shapely.readthedocs.io/en/stable/manual.html#binary-predicates
    bool Intersects(const las::LasHeader &header, const Box &box) const;
    bool Contains(const las::LasHeader &header, const Box &vec) const;
    bool Contains(const las::LasHeader &header, const Vector3 &point) const;
    bool Within(const las::LasHeader &header, const Box &box) const;
    bool Crosses(const las::LasHeader &header, const Box &box) const;

    double Resolution(const las::LasHeader &header, const CopcInfo &copc_info) const;
    static double GetResolutionAtDepth(int32_t d, const las::LasHeader &header, const CopcInfo &copc_info);

    int32_t d;
    int32_t x;
    int32_t y;
    int32_t z;

  private:
    // Used for Bisect function
    int32_t &IdAt(const uint8_t &i)
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
};

// Equality operations
inline bool operator==(const VoxelKey &a, const VoxelKey &b)
{
    return a.d == b.d && a.x == b.x && a.y == b.y && a.z == b.z;
}
inline bool operator!=(const VoxelKey &a, const VoxelKey &b) { return !(a == b); }

} // namespace copc

// Hash function to allow VoxelKeys as unordered_map keys
template <> struct std::hash<copc::VoxelKey>
{
    std::size_t operator()(copc::VoxelKey const &k) const noexcept
    {
        std::hash<uint64_t> h;

        uint64_t k1 = ((uint64_t)k.d << 32) | k.x;
        uint64_t k2 = ((uint64_t)k.y << 32) | k.z;
        return h(k1) ^ (h(k2) << 1);
    }
};

#endif // COPCLIB_HIERARCHY_KEY_H_
