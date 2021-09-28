#ifndef COPCLIB_COPC_EXTENTS_H_
#define COPCLIB_COPC_EXTENTS_H_

#include <vector>

#include <copc-lib/las/utils.hpp>
#include <copc-lib/las/vlr.hpp>

namespace copc
{

class CopcExtents
{
  public:
    CopcExtents(int8_t point_format_id, uint16_t num_extra_bytes = 0)
        : point_format_id(point_format_id), extra_bytes(num_extra_bytes, {0, 0})
    {
        if (point_format_id < 6 || point_format_id > 8)
            throw std::runtime_error("Supported point formats are 6 to 8.");
    }

    CopcExtents(const las::CopcExtentsVlr &vlr, int8_t point_format_id, uint16_t num_extra_bytes = 0);

    las::CopcExtentsVlr ToCopcExtentsVlr() const;

    void SetCopcExtents(const std::vector<las::CopcExtent> &extents);

    static int NumberOfExtents(int8_t point_format_id, uint16_t num_extra_bytes);

    int8_t point_format_id;
    las::CopcExtent x{};
    las::CopcExtent y{};
    las::CopcExtent z{};
    las::CopcExtent intensity{};
    las::CopcExtent return_number{};
    las::CopcExtent number_of_returns{};
    las::CopcExtent scanner_channel{};
    las::CopcExtent scan_direction_flag{};
    las::CopcExtent edge_of_flight_line{};
    las::CopcExtent classification{};
    las::CopcExtent user_data{};
    las::CopcExtent scan_angle{};
    las::CopcExtent point_source_id{};
    las::CopcExtent gps_time{};
    las::CopcExtent red{};
    las::CopcExtent green{};
    las::CopcExtent blue{};
    las::CopcExtent nir{};
    std::vector<las::CopcExtent> extra_bytes;
};
} // namespace copc
#endif // COPCLIB_COPC_EXTENTS_H_
