#ifndef COPCLIB_LAS_UTILS_H_
#define COPCLIB_LAS_UTILS_H_

#include <iostream>

namespace copc::las::internal
{
template <typename T> T unpack(std::istream &in_stream)
{
    T value;
    in_stream.read(reinterpret_cast<char *>(&value), sizeof(T));
    return value;
}

template <typename T> void pack(const T &value, std::ostream &out_stream)
{
    out_stream.write(reinterpret_cast<const char *>(&value), sizeof(T));
};

} // namespace copc::las::internal
#endif // COPCLIB_LAS_UTILS_H_
