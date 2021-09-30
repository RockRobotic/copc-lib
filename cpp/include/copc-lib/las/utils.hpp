#ifndef COPCLIB_LAS_UTILS_H_
#define COPCLIB_LAS_UTILS_H_

#include <cmath>
#include <iostream>

namespace copc::las
{
uint8_t PointBaseByteSize(const int8_t &point_format_id);
uint16_t ComputeNumExtraBytes(const int8_t &point_format_id, const uint32_t &point_record_length);
uint16_t ComputePointBytes(const int8_t &point_format_id, const uint16_t &num_extra_bytes);

bool FormatHasGPSTime(const uint8_t &point_format_id);
bool FormatHasRGB(const uint8_t &point_format_id);
bool FormatHasNIR(const uint8_t &point_format_id);

template <typename T> double ApplyScale(T value, double scale, double offset) { return (value * scale) + offset; }
template <typename T> T RemoveScale(double value, double scale, double offset)
{
    double val = std::round((value - offset) / scale);

    if (val < std::numeric_limits<T>::min() || val > std::numeric_limits<T>::max())
        throw std::runtime_error("The value " + std::to_string(value) +
                                 " is too large to save into the requested format." +
                                 " Your scale and/or offset may be incorrect.");

    return static_cast<T>(val);
}

template <typename T> T unpack(std::istream &in_stream)
{
    T value;
    in_stream.read(reinterpret_cast<char *>(&value), sizeof(T));
    return value;
}
template <typename T> void pack(const T &value, std::ostream &out_stream)
{
    out_stream.write(reinterpret_cast<const char *>(&value), sizeof(T));
}

} // namespace copc::las
#endif // COPCLIB_LAS_UTILS_H_
