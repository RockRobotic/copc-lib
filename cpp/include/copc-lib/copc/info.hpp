#ifndef COPCLIB_COPC_INFO_H_
#define COPCLIB_COPC_INFO_H_
#include <string>

#include "copc-lib/copc/extents.hpp"
#include <lazperf/vlr.hpp>

namespace copc
{

class CopcInfo
{
  public:
    static const int VLR_OFFSET = 375;     // COPC Info VLR must be the first VLR after the LAS header block
    static const int VLR_SIZE_BYTES = 160; // COPC Info VLR payload is 160 bytes, https://copc.io/

    CopcInfo() = default;

    CopcInfo(const lazperf::copc_info_vlr &copc_info_vlr);

    lazperf::copc_info_vlr ToLazPerf(const CopcExtent &gps_time) const;

    std::string ToString() const;
    friend std::ostream &operator<<(std::ostream &os, CopcInfo const &value)
    {
        os << value.ToString();
        return os;
    }

    double center_x{0};
    double center_y{0};
    double center_z{0};
    double halfsize{0};
    double spacing{0};
    uint64_t root_hier_offset{0};
    uint64_t root_hier_size{0};
};
} // namespace copc
#endif // COPCLIB_COPC_INFO_H_
