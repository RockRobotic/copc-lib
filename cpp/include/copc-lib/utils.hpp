#ifndef COPCLIB_UTILS_H_
#define COPCLIB_UTILS_H_

#include <cmath>

namespace copc
{

bool AreClose(double a, double b, double epsilon) { return fabs(a - b) < epsilon; }

} // namespace copc
#endif // COPCLIB_UTILS_H_
