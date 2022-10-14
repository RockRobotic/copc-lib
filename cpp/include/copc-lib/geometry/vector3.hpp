#ifndef COPCLIB_GEOMETRY_VECTOR3_H_
#define COPCLIB_GEOMETRY_VECTOR3_H_

#include <limits>
#include <sstream>
#include <vector>

namespace copc
{

const double DEFAULT_SCALE = 0.01;
struct Vector3
{
    Vector3() : x(0), y(0), z(0) {}
    Vector3(const double &x, const double &y, const double &z) : x(x), y(y), z(z) {}
    static Vector3 DefaultScale() { return Vector3(DEFAULT_SCALE, DEFAULT_SCALE, DEFAULT_SCALE); }
    static Vector3 DefaultOffset() { return Vector3{}; }

    Vector3(const std::vector<double> &vec)
    {
        if (vec.size() != 3)
            throw std::runtime_error("Vector must be of size 3.");
        x = vec[0];
        y = vec[1];
        z = vec[2];
    }

    Vector3 operator*(const double &d) const { return Vector3(x * d, y * d, z * d); }
    Vector3 operator/(const double &d) const { return Vector3(x / d, y / d, z / d); }
    Vector3 operator+(const double &d) const { return Vector3(x + d, y + d, z + d); }
    Vector3 operator-(const double &d) const { return Vector3(x - d, y - d, z - d); }

    Vector3 operator*(const Vector3 &other) const { return Vector3(x * other.x, y * other.y, z * other.z); }
    Vector3 operator/(const Vector3 &other) const { return Vector3(x / other.x, y / other.y, z / other.z); }
    Vector3 operator+(const Vector3 &other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
    Vector3 operator-(const Vector3 &other) const { return Vector3(x - other.x, y - other.y, z - other.z); }

    std::string ToString() const
    {
        std::stringstream ss;
        ss.precision(std::numeric_limits<double>::max_digits10);
        ss << "(" << x << ", " << y << ", " << z << ")";
        return ss.str();
    }
    friend std::ostream &operator<<(std::ostream &os, Vector3 const &value)
    {
        os << value.ToString();
        return os;
    }

    bool operator==(Vector3 other) const { return x == other.x && y == other.y && z == other.z; }
    bool operator!=(Vector3 other) const { return !(*this == other); }

    double x{};
    double y{};
    double z{};
};

} // namespace copc

#endif // COPCLIB_GEOMETRY_VECTOR3_H_
