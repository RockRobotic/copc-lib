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
    Box(double x_min, double y_min, double z_min, double x_max, double y_max, double z_max);

    // 2D box constructor
    Box(double x_min, double y_min, double x_max, double y_max);

    // Vector3 constructor
    Box(const Vector3 &min, const Vector3 &max);

    // Constructor for tuple and list implicit conversion
    Box(const std::vector<double> &vec);

    // Constructor from Node
    Box(const VoxelKey &key, const las::LasHeader &header);

    static Box EmptyBox() { return Box(); }
    static Box MaxBox();

    bool Intersects(const Box &box) const;
    bool Contains(const Box &box) const;
    bool Contains(const Vector3 &vec) const;
    bool Within(const Box &box) const;

    std::string ToString() const;
    friend std::ostream &operator<<(std::ostream &os, Box const &value)
    {
        os << value.ToString();
        return os;
    }

    double x_min{};
    double y_min{};
    double z_min{};
    double x_max{};
    double y_max{};
    double z_max{};
};

} // namespace copc

#endif // COPCLIB_GEOMETRY_BOX_H_
