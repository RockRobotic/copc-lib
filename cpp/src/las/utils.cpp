#include "copc-lib/las/utils.hpp"

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

uint8_t PointBaseNumberDimensions(const int8_t &point_format_id)
{
    switch (point_format_id)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        throw std::runtime_error("Point format must be 6-8");
    case 6:
        return 14;
    case 7:
        return 17;
    case 8:
        return 18;
    default:
        throw std::runtime_error("Point format must be 6-8");
    }
}

bool FormatHasGPSTime(const uint8_t &point_format_id)
{
    switch (point_format_id)
    {
    case 0:
        return false;
    case 1:
        return true;
    case 2:
        return false;
    case 3:
        return true;
    case 4:
    case 5:
        throw std::runtime_error("Point formats with Wave Packets not yet supported");
    case 6:
    case 7:
    case 8:
        return true;
    case 9:
    case 10:
        throw std::runtime_error("Point formats with Wave Packets not yet supported");

    default:
        throw std::runtime_error("Point format must be 0-10");
    }
}

bool FormatHasRGB(const uint8_t &point_format_id)
{
    switch (point_format_id)
    {
    case 0:
    case 1:
        return false;
    case 2:
    case 3:
        return true;
    case 4:
    case 5:
        throw std::runtime_error("Point formats with Wave Packets not yet supported");
    case 6:
        return false;
    case 7:
    case 8:
        return true;
    case 9:
    case 10:
        throw std::runtime_error("Point formats with Wave Packets not yet supported");
    default:
        throw std::runtime_error("Point format must be 0-10");
    }
}

bool FormatHasNIR(const uint8_t &point_format_id)
{
    switch (point_format_id)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        return false;
    case 4:
    case 5:
        throw std::runtime_error("Point formats with Wave Packets not yet supported");
    case 6:
    case 7:
        return false;
    case 8:
        return true;
    case 9:
    case 10:
        throw std::runtime_error("Point formats with Wave Packets not yet supported");
    default:
        throw std::runtime_error("Point format must be 0-10");
    }
}

uint16_t ComputeEbByteSize(const int8_t &point_format_id, const uint32_t &point_record_length)
{
    return point_record_length - PointBaseByteSize(point_format_id);
}

uint16_t ComputePointBytes(const int8_t &point_format_id, const uint16_t &eb_byte_size)
{
    return PointBaseByteSize(point_format_id) + eb_byte_size;
}

} // namespace copc::las
