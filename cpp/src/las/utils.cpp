#include "copc-lib/las/utils.hpp"

namespace copc::las
{

uint8_t PointBaseByteSize(const int8_t &point_format_id)
{
    switch (point_format_id)
    {
    case 6:
        return 30;
    case 7:
        return 36;
    case 8:
        return 38;
    default:
        throw std::runtime_error("PointBaseByteSize: Point format must be 6-8");
    }
}

uint8_t PointBaseNumberDimensions(const int8_t &point_format_id)
{
    switch (point_format_id)
    {
    case 6:
        return 14;
    case 7:
        return 17;
    case 8:
        return 18;
    default:
        throw std::runtime_error("PointBaseNumberDimensions: Point format must be 6-8");
    }
}

bool FormatHasRgb(const uint8_t &point_format_id)
{
    switch (point_format_id)
    {
    case 6:
        return false;
    case 7:
    case 8:
        return true;
    default:
        throw std::runtime_error("FormatHasRgb: Point format must be 6-8");
    }
}

bool FormatHasNir(const uint8_t &point_format_id)
{
    switch (point_format_id)
    {
    case 6:
    case 7:
        return false;
    case 8:
        return true;
    default:
        throw std::runtime_error("FormatHasNir: Point format must be 6-8");
    }
}

uint16_t EbByteSize(const int8_t &point_format_id, const uint32_t &point_record_length)
{
    return point_record_length - PointBaseByteSize(point_format_id);
}

uint16_t PointByteSize(const int8_t &point_format_id, const uint16_t &eb_byte_size)
{
    return PointBaseByteSize(point_format_id) + eb_byte_size;
}

} // namespace copc::las
