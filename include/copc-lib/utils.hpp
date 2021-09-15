#ifndef COPCLIB_UTILS_H_
#define COPCLIB_UTILS_H_

#include <lazperf/header.hpp>

namespace copc
{

struct vector3
{
    vector3() : x(0), y(0), z(0) {}
    vector3(const double &x, const double &y, const double &z) : x(x), y(y), z(z) {}

    double x;
    double y;
    double z;

    vector3 &operator=(const lazperf::vector3 &a)
    {
        x = a.x;
        y = a.y;
        z = a.z;

        return *this; // Return a reference to myself.
    }
};
} // namespace copc

#endif // COPCLIB_UTILS_H_
