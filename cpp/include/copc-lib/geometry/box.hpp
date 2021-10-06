#ifndef COPCLIB_GEOMETRY_BOX_H_
#define COPCLIB_GEOMETRY_BOX_H_

#include <sstream>
#include <vector>

#include "copc-lib/geometry/vector3.hpp"

namespace copc
{
namespace las
{
class LasHeader;
}
class VoxelKey;
class Box
{
  public:
    Box() = default;

    // 3D box constructor
    Box(const double &x_min, const double &y_min, const double &z_min, const double &x_max, const double &y_max,
        const double &z_max);

    // 2D box constructor
    Box(const double &x_min, const double &y_min, const double &x_max, const double &y_max);

    // Constructor for tuple and list implicit conversion
    Box(const std::vector<double> &vec);

    // Constructor from Node
    Box(const VoxelKey &key, const las::LasHeader &header);

    static Box ZeroBox() { return Box(); }
    static Box MaxBox();

    bool Intersects(const Box &box) const;
    bool Contains(const Box &box) const;
    bool Contains(const Vector3 &vec) const;
    bool Within(const Box &box) const;

    std::string ToString() const;

    Vector3 Min() const { return Vector3(x_min, y_min, z_min); }
    Vector3 Max() const { return Vector3(x_max, y_max, z_max); }

    double x_min{};
    double y_min{};
    double z_min{};
    double x_max{};
    double y_max{};
    double z_max{};
};

} // namespace copc

#endif // COPCLIB_GEOMETRY_BOX_H_
