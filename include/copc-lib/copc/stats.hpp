#ifndef COPCLIB_COPC_STATS_H_
#define COPCLIB_COPC_STATS_H_

#include <vector>

#include <copc-lib/las/utils.hpp>
#include <copc-lib/las/vlr.hpp>

namespace copc
{

class CopcStats
{
  public:
    CopcStats(int8_t point_format_id, uint16_t num_extra_bytes = 0)
        : point_format_id(point_format_id), extra_bytes(num_extra_bytes, {0, 0})
    {
        if (point_format_id < 6 || point_format_id > 8)
            throw std::runtime_error("Supported point formats are 6 to 8.");
    }
    CopcStats(const std::vector<las::CopcExtent> &extents, int8_t point_format_id, uint16_t num_extra_bytes = 0)
        : point_format_id(point_format_id), extra_bytes(num_extra_bytes, {0, 0})
    {
        if (point_format_id < 6 || point_format_id > 8)
            throw std::runtime_error("Supported point formats are 6 to 8.");
        FromCopcExtents(extents);
    }

    std::vector<las::CopcExtent> ToCopcExtents() const;

    void FromCopcExtents(const std::vector<las::CopcExtent> &extents);

    static int NumberOfExtents(int8_t point_format_id, uint16_t num_extra_bytes);

    int8_t point_format_id;
    las::CopcExtent x{0, 0};
    las::CopcExtent y{0, 0};
    las::CopcExtent z{0, 0};
    las::CopcExtent intensity{0, 0};
    las::CopcExtent return_number{0, 0};
    las::CopcExtent number_of_returns{0, 0};
    las::CopcExtent scanner_channel{0, 0};
    las::CopcExtent scan_direction_flag{0, 0};
    las::CopcExtent edge_of_flight_line{0, 0};
    las::CopcExtent classification{0, 0};
    las::CopcExtent user_data{0, 0};
    las::CopcExtent scan_angle{0, 0};
    las::CopcExtent point_source_id{0, 0};
    las::CopcExtent gps_time{0, 0};
    las::CopcExtent red{0, 0};
    las::CopcExtent green{0, 0};
    las::CopcExtent blue{0, 0};
    las::CopcExtent nir{0, 0};
    std::vector<las::CopcExtent> extra_bytes;
};
} // namespace copc
#endif // COPCLIB_COPC_STATS_H_
