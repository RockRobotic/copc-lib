#include "copc-lib/geometry/box.hpp"
#include "copc-lib/hierarchy/key.hpp"
#include "copc-lib/las/header.hpp"

namespace copc
{

// 3D box constructor
Box::Box(double x_min, double y_min, double z_min, double x_max, double y_max, double z_max)
    : x_min(x_min), y_min(y_min), z_min(z_min), x_max(x_max), y_max(y_max), z_max(z_max)
{
    if (x_max < x_min || y_max < y_min || z_max < z_min)
        throw std::runtime_error("One or more of min values is greater than a max value");
}

// 2D box constructor
Box::Box(double x_min, double y_min, double x_max, double y_max)
    : Box(x_min, y_min, -std::numeric_limits<double>::max(), x_max, y_max, std::numeric_limits<double>::max())
{
}

// Vector3 constructor
Box::Box(const Vector3 &min, const Vector3 &max) : Box(min.x, min.y, min.z, max.x, max.y, max.z) {}

// Constructor for tuple and list implicit conversion
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

// Constructor from Node
Box::Box(const VoxelKey &key, const las::LasHeader &header)
{

    // Step size accounts for depth level
    double step = header.Span() / std::pow(2, key.d);

    x_min = step * key.x + header.min.x;
    y_min = step * key.y + header.min.y;
    z_min = step * key.z + header.min.z;
    x_max = x_min + step;
    y_max = y_min + step;
    z_max = z_min + step;
}

Box Box::MaxBox()
{
    return Box(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(),
               -std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
               std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
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

std::string Box::ToString() const
{
    std::stringstream ss;
    ss.precision(std::numeric_limits<double>::max_digits10);
    ss << "Box: x_min=" << x_min << " y_min=" << y_min << " z_min=" << z_min << " x_max=" << x_max << " y_max=" << y_max
       << " z_max=" << z_max;
    return ss.str();
}

} // namespace copc
