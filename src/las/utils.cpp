#include <copc-lib/las/utils.hpp>

#include <iostream>

namespace copc::las
{
uint8_t PointBaseByteSize(const int8_t &point_format_id)
{
    switch (point_format_id)
    {
    case 0:
        return 20;
    case 1:
        return 28;
    case 2:
        return 26;
    case 3:
        return 34;
    case 4:
        //        return 28;
    case 5:
        //        return 34;
        throw std::runtime_error("Point formats with Wave Packets not yet supported");
    case 6:
        return 30;
    case 7:
        return 36;
    case 8:
        return 38;
    case 9:
        //        return 30;
    case 10:
        //        return 38;
        throw std::runtime_error("Point formats with Wave Packets not yet supported");
    default:
        throw std::runtime_error("Point format must be 0-10");
    }
}

uint16_t ComputeNumExtraBytes(const int8_t &point_format_id, const uint32_t &point_record_length)
{
    return point_record_length - PointBaseByteSize(point_format_id);
}

uint16_t ComputePointBytes(const int8_t &point_format_id, const uint16_t &num_extra_bytes_)
{
    return PointBaseByteSize(point_format_id) + num_extra_bytes_;
}

} // namespace copc::las
