#include "copc-lib/hierarchy/key.hpp"
#include "copc-lib/las/header.hpp"

namespace copc
{

std::string VoxelKey::ToString() const
{
    std::stringstream ss;
    ss << "(" << d << ", " << x << ", " << y << ", " << z << ")";
    return ss.str();
}

VoxelKey VoxelKey::Bisect(const uint64_t &direction) const
{
    VoxelKey key(*this);
    ++key.d;

    auto step(
        [&key, direction](uint8_t i)
        {
            key.IdAt(i) *= 2;
            const bool positive(direction & (((uint64_t)1) << i));
            if (positive)
                ++key.IdAt(i);
        });

    for (uint8_t i(0); i < 3; ++i)
        step(i);

    return key;
}

std::vector<VoxelKey> VoxelKey::GetChildren() const
{
    std::vector<VoxelKey> children(8);
    for (int i = 0; i < 8; i++)
        children[i] = Bisect(i);
    return children;
}

VoxelKey VoxelKey::GetParent() const
{
    // If key is valid, return parent, if not, return invalid key
    if (IsValid())
        return {d - 1, x / 2, y / 2, z / 2};
    else
        return {};
}

VoxelKey VoxelKey::GetParentAtDepth(int32_t depth) const
{
    if (!IsValid())
        return {};
    if (depth < 0 || depth > d)
        throw std::runtime_error("VoxelKey::GetParentAtDepth: Invalid depth requested.");

    int reduction_factor = (int)std::pow(2, d - depth);
    return {depth, x / reduction_factor, y / reduction_factor, z / reduction_factor};
}

std::vector<VoxelKey> VoxelKey::GetParents(bool include_self) const
{
    std::vector<VoxelKey> out;
    if (!IsValid())
        return out;

    if (include_self)
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

double VoxelKey::Resolution(const las::LasHeader &header, const CopcInfo &copc_info) const
{
    return copc_info.spacing / std::pow(2, d);
}

double VoxelKey::GetResolutionAtDepth(int32_t d, const las::LasHeader &header, const CopcInfo &copc_info)
{
    return VoxelKey(d, 0, 0, 0).Resolution(header, copc_info);
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
