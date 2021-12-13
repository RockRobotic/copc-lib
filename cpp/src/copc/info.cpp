#include "copc-lib/copc/info.hpp"

#include <sstream>

namespace copc
{
CopcInfo::CopcInfo(const lazperf::copc_info_vlr &copc_info_vlr)
{
    center_x = copc_info_vlr.center_x;
    center_y = copc_info_vlr.center_y;
    center_z = copc_info_vlr.center_z;
    halfsize = copc_info_vlr.halfsize;
    spacing = copc_info_vlr.spacing;
    root_hier_offset = copc_info_vlr.root_hier_offset;
    root_hier_size = copc_info_vlr.root_hier_size;
}

lazperf::copc_info_vlr CopcInfo::ToLazPerf(const CopcExtent &gps_time_extent) const
{
    lazperf::copc_info_vlr copc_info_vlr;

    copc_info_vlr.center_x = center_x;
    copc_info_vlr.center_y = center_y;
    copc_info_vlr.center_z = center_z;
    copc_info_vlr.halfsize = halfsize;
    copc_info_vlr.spacing = spacing;
    copc_info_vlr.gpstime_minimum = gps_time_extent.minimum;
    copc_info_vlr.gpstime_maximum = gps_time_extent.maximum;
    copc_info_vlr.root_hier_offset = root_hier_offset;
    copc_info_vlr.root_hier_size = root_hier_size;

    return copc_info_vlr;
}

std::string CopcInfo::ToString() const
{
    std::stringstream ss;
    ss << "CopcInfo:" << std::endl;
    ss << "\tcenter_x: " << center_x << std::endl;
    ss << "\tcenter_y: " << center_y << std::endl;
    ss << "\tcenter_z: " << center_z << std::endl;
    ss << "\thalfsize: " << halfsize << std::endl;
    ss << "\tspacing: " << spacing << std::endl;
    return ss.str();
}

} // namespace copc
