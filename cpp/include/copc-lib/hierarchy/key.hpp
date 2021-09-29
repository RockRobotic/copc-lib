#ifndef COPCLIB_HIERARCHY_KEY_H_
#define COPCLIB_HIERARCHY_KEY_H_

#include <cstdint>
#include <functional> // for hash
#include <sstream>
#include <vector>

namespace copc
{

class VoxelKey
{
  public:
    VoxelKey(int32_t d, int32_t x, int32_t y, int32_t z) : d(d), x(x), y(y), z(z) {}
    VoxelKey() : VoxelKey(-1, -1, -1, -1) {}

    static VoxelKey InvalidKey() { return VoxelKey(); }
    static VoxelKey BaseKey() { return VoxelKey(0, 0, 0, 0); }

    bool IsValid() const { return d >= 0 && x >= 0 && y >= 0 && z >= 0; }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << d << "-" << x << "-" << y << "-" << z;
        return ss.str();
    }

    // Returns the corresponding key depending on direction [0,7]
    VoxelKey Bisect(uint64_t direction) const;

    // The hierarchial parent of this key
    VoxelKey GetParent() const;

    // A list of the key's parents from the key to the root node
    // optionally including the key itself
    std::vector<VoxelKey> GetParents(bool include_current = false) const;

    // Tests whether the current key is a child of a given key
    bool ChildOf(VoxelKey parent_key) const;

    int32_t d;
    int32_t x;
    int32_t y;
    int32_t z;

  private:
    // Used for Bisect function
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
};

// Equality operations
inline bool operator==(const VoxelKey &a, const VoxelKey &b)
{
    return a.d == b.d && a.x == b.x && a.y == b.y && a.z == b.z;
}
inline bool operator!=(const VoxelKey &a, const VoxelKey &b) { return !(a == b); }

} // namespace copc

// Hash function to allow VoxelKeys as unordered_map keys
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

#endif // COPCLIB_HIERARCHY_KEY_H_
