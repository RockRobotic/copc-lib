#ifndef COPCLIB_COPC_EXTENTS_H_
#define COPCLIB_COPC_EXTENTS_H_

#include <vector>

#include <copc-lib/las/utils.hpp>
#include <copc-lib/las/vlr.hpp>

namespace copc
{

class CopcExtent : public las::CopcExtentsVlr::CopcExtent
{
  public:
    CopcExtent() : las::CopcExtentsVlr::CopcExtent(0, 0){};

    CopcExtent(double minimum, double maximum);

    CopcExtent(const std::vector<double> &vec);

    CopcExtent(const las::CopcExtentsVlr::CopcExtent &other);

    las::CopcExtentsVlr::CopcExtent ToLazPerf() const { return {minimum, maximum}; }
};

class CopcExtents
{
  public:
    CopcExtents(int8_t point_format_id, uint16_t num_extra_bytes = 0);

    CopcExtents(const las::CopcExtentsVlr &vlr, int8_t point_format_id, uint16_t num_extra_bytes = 0);

    las::CopcExtentsVlr ToCopcExtentsVlr() const;

    void SetCopcExtents(const std::vector<CopcExtent> &extents);

    static int NumberOfExtents(int8_t point_format_id, uint16_t num_extra_bytes);

    int8_t point_format_id;
    CopcExtent x{};
    CopcExtent y{};
    CopcExtent z{};
    CopcExtent intensity{};
    CopcExtent return_number{};
    CopcExtent number_of_returns{};
    CopcExtent scanner_channel{};
    CopcExtent scan_direction_flag{};
    CopcExtent edge_of_flight_line{};
    CopcExtent classification{};
    CopcExtent user_data{};
    CopcExtent scan_angle{};
    CopcExtent point_source_id{};
    CopcExtent gps_time{};
    CopcExtent red{};
    CopcExtent green{};
    CopcExtent blue{};
    CopcExtent nir{};
    std::vector<CopcExtent> extra_bytes;
};
} // namespace copc
#endif // COPCLIB_COPC_EXTENTS_H_
