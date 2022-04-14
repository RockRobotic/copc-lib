#include "copc-lib/copc/copc_config.hpp"

#include <stdexcept>

namespace copc
{
CopcConfig::CopcConfig(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset,
                       const std::string &wkt, const las::EbVlr &extra_bytes_vlr, bool has_extended_stats)
    : LazConfig(point_format_id, scale, offset, wkt, extra_bytes_vlr, true)
{
    copc_info_ = std::make_shared<copc::CopcInfo>();
    copc_extents_ =
        std::make_shared<copc::CopcExtents>(point_format_id, extra_bytes_vlr.items.size(), has_extended_stats);
}

CopcConfigWriter::CopcConfigWriter(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset,
                                   const std::string &wkt, const las::EbVlr &extra_bytes_vlr, bool has_extended_stats)
    : CopcConfig(point_format_id, scale, offset, wkt, extra_bytes_vlr, has_extended_stats)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("LasConfig: Supported point formats are 6 to 8.");
}

} // namespace copc
