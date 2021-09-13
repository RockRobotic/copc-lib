#ifndef COPCLIB_LAS_UTILS_H_
#define COPCLIB_LAS_UTILS_H_

#include <iostream>

namespace copc::las
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

uint8_t PointBaseByteSize(const int8_t &point_format_id);
uint16_t ComputeNumExtraBytes(const int8_t &point_format_id, const uint32_t &point_record_length);
uint16_t ComputePointBytes(const int8_t &point_format_id, const uint16_t &num_extra_bytes_);

} // namespace copc::las
#endif // COPCLIB_LAS_UTILS_H_
