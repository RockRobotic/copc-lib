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

    CopcExtent(double minimum, double maximum) : las::CopcExtentsVlr::CopcExtent(minimum, maximum)
    {
        if (minimum > maximum)
            throw std::runtime_error("CopcExtent: Minimum value must be less or equal than maximum value.");
    };

    CopcExtent(const std::vector<double> &vec)
    {
        if (vec.size() != 2)
            throw std::runtime_error("CopcExtent: Vector size must be 2.");

        if (vec[0] > vec[1])
            throw std::runtime_error("CopcExtent: Minimum value must be less or equal than maximum value.");
        minimum = vec[0];
        maximum = vec[1];
    };

    CopcExtent(const las::CopcExtentsVlr::CopcExtent &other)
    {
        if (other.minimum > other.maximum)
            throw std::runtime_error("CopcExtent: Minimum value must be less or equal than maximum value.");
        minimum = other.minimum;
        maximum = other.maximum;
    };

    las::CopcExtentsVlr::CopcExtent ToLazPerf() const { return {minimum, maximum}; }
};

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
