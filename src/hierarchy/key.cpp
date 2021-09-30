#include <copc-lib/hierarchy/key.hpp>
#include <copc-lib/las/header.hpp>

namespace copc
{

std::string VoxelKey::ToString() const
{
    std::stringstream ss;
    ss << d << "-" << x << "-" << y << "-" << z;
    return ss.str();
}

VoxelKey VoxelKey::Bisect(const uint64_t &direction) const
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

double VoxelKey::Resolution(const las::LasHeader &header, const las::CopcVlr &copc_info) const
{
    if (copc_info.span <= 0)
        throw std::runtime_error("VoxelKey::Resolution: Octree span must be greater than 0.");
    return (header.max.x - header.min.x) / copc_info.span / std::pow(2, d);
}

bool VoxelKey::Intersects(const las::LasHeader &header, const Box &box) const
{
    return Box(*this, header).Intersects(box);
}
bool VoxelKey::Contains(const las::LasHeader &header, const Box &box) const { return Box(*this, header).Contains(box); }
bool VoxelKey::Contains(const las::LasHeader &header, const Vector3 &point) const
{
    return Box(*this, header).Contains(point);
}
bool VoxelKey::Within(const las::LasHeader &header, const Box &box) const { return Box(*this, header).Within(box); }
bool VoxelKey::Crosses(const las::LasHeader &header, const Box &box) const
{
    return Box(*this, header).Intersects(box) && !Box(*this, header).Within(box);
}

} // namespace copc
