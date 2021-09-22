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
bool VoxelKey::Contains(const Vector3 &point, const las::LasHeader &header) const
{
    return Box(*this, header).Contains(point);
}
bool VoxelKey::Within(const Box &box, const las::LasHeader &header) const { return Box(*this, header).Within(box); }

Box::Box(const std::vector<double> &vec)
{
    if (vec.size() == 4)
    {
        x_min = vec[0];
        y_min = vec[1];
        z_min = -std::numeric_limits<double>::max();
        x_max = vec[2];
        y_max = vec[3];
        z_max = std::numeric_limits<double>::max();
    }
    else if (vec.size() == 6)
    {
        x_min = vec[0];
        y_min = vec[1];
        z_min = vec[2];
        x_max = vec[3];
        y_max = vec[4];
        z_max = vec[5];
    }
    else
    {
        throw std::runtime_error("Vector must be of size 4 or 6.");
    }
    if (x_max < x_min || y_max < y_min || z_max < z_min)
        throw std::runtime_error("One or more of min values is greater than a value");
}

Box::Box(const VoxelKey &key, const las::LasHeader &header)
{

    // Step size accounts for depth level
    double step = header.GetSpan() / std::pow(2, key.d);

    x_min = step * key.x + header.min.x;
    y_min = step * key.y + header.min.y;
    z_min = step * key.z + header.min.z;
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
