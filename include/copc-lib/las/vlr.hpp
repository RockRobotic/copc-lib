#ifndef COPCLIB_LAS_VLR_H_
#define COPCLIB_LAS_VLR_H_

#include <cstring>
#include <stdexcept>
#include <vector>

#include "lazperf/vlr.hpp"

namespace copc::las
{

using WktVlr = lazperf::wkt_vlr;
using CopcInfoVlr = lazperf::copc_info_vlr;
using EbVlr = lazperf::eb_vlr;
using VlrHeader = lazperf::vlr_header;
using CopcExtentsVlr = lazperf::copc_extents_vlr;

class CopcExtent : public lazperf::copc_extents_vlr::CopcExtent
{

    CopcExtent(double minimum, double maximum)
    {
        if (minimum > maximum)
            throw std::runtime_error("CopcExtent: Minimum value must be less or equal than maximum value.");
        this->minimum = minimum;
        this->maximum = maximum;
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
};

} // namespace copc::las

namespace lazperf
{
// Equality operations
inline bool operator==(const eb_vlr::ebfield &a, const eb_vlr::ebfield &b)
{
    return std::memcmp(a.reserved, b.reserved, 2) == 0 && a.data_type == b.data_type && a.options == b.options &&
           a.name == b.name && std::memcmp(a.no_data, b.no_data, 3) == 0 && std::memcmp(a.minval, b.minval, 3) == 0 &&
           std::memcmp(a.maxval, b.maxval, 3) == 0 && std::memcmp(a.scale, b.scale, 3) == 0 &&
           std::memcmp(a.offset, b.offset, 3) == 0 && a.description == b.description;
}
} // namespace lazperf

#endif // COPCLIB_LAS_VLR_H_
