#include "copc-lib/copc/config.hpp"

#include <stdexcept>

namespace copc
{

CopcConfigWriter::CopcConfigWriter(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset,
                                   const std::string &wkt, const las::EbVlr &extra_bytes_vlr)
    : CopcConfig(point_format_id, scale, offset, wkt, extra_bytes_vlr)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("LasConfig: Supported point formats are 6 to 8.");
}

CopcConfig::CopcConfig(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset,
                       const std::string &wkt, const las::EbVlr &extra_bytes_vlr)
{
}
} // namespace copc
