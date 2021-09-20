#include <copc-lib/hierarchy/key.hpp>

#include <cmath>

namespace copc
{

VoxelKey VoxelKey::Bisect(uint64_t direction) const
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

VoxelKey VoxelKey::GetParent() const
{
    // If key is valid, return parent, if not, return invalid key
    if (IsValid())
        return {d - 1, x / 2, y / 2, z / 2};
    else
        return {};
}

std::vector<VoxelKey> VoxelKey::GetParents(bool include_current) const
{
    std::vector<VoxelKey> out;
    if (!IsValid())
        return out;

    if (include_current)
        out.push_back(*this);

    auto parentKey = this->GetParent();
    while (parentKey.IsValid())
    {
        out.push_back(parentKey);
        parentKey = parentKey.GetParent();
    }

    return out;
}

bool VoxelKey::ChildOf(VoxelKey parent_key) const
{
    for (auto parent : GetParents(true))
    {
        if (parent == parent_key)
            return true;
    }
    return false;
}

bool VoxelKey::Intersects(const Box &box, const las::LasHeader &header) const
{
    return Box(*this, header).Intersects(box);
}
bool VoxelKey::Contains(const Box &box, const las::LasHeader &header) const { return Box(*this, header).Contains(box); }
bool VoxelKey::Contains(const Vector3 &vec, const las::LasHeader &header) const
{
    return Box(*this, header).Contains(vec);
}
bool VoxelKey::Within(const Box &box, const las::LasHeader &header) const { return Box(*this, header).Within(box); }

Box::Box(const VoxelKey &key, const las::LasHeader &header)
{

    // Step size accounts for depth level
    double step = header.span / std::pow(2, key.d);

    x_min = step * key.x;
    y_min = step * key.y;
    z_min = step * key.z;
    x_max = x_min + step;
    y_max = y_min + step;
    z_max = z_min + step;
}

bool Box::Intersects(const Box &other) const
{
    return x_max >= other.x_min && x_min <= other.x_max && y_max >= other.y_min && y_min <= other.y_max &&
           z_max >= other.z_min && z_min <= other.z_max;
}
bool Box::Contains(const Box &other) const
{
    return x_max >= other.x_max && x_min <= other.x_min && y_max >= other.y_max && y_min <= other.y_min &&
           z_max >= other.z_max && z_min <= other.z_min;
}
bool Box::Contains(const Vector3 &vec) const
{
    return x_max >= vec.x && x_min <= vec.x && y_max >= vec.y && y_min <= vec.y && z_max >= vec.z && z_min <= vec.z;
}
bool Box::Within(const Box &other) const { return other.Contains(*this); }

} // namespace copc
