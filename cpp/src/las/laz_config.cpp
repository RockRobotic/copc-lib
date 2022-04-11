#include "copc-lib/las/laz_config.hpp"

#include "copc-lib/copc/copc_config.hpp"

#include <stdexcept>
namespace copc::las
{
LazConfig::LazConfig(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset, const std::string &wkt,
                     const las::EbVlr &extra_bytes_vlr, bool copc_flag)
    : wkt_(wkt)
{
    header_ = std::make_shared<las::LasHeader>(
        point_format_id, las::PointBaseByteSize(point_format_id) + las::NumBytesFromExtraBytes(extra_bytes_vlr.items),
        scale, offset, copc_flag);
    eb_vlr_ = std::make_shared<las::EbVlr>(extra_bytes_vlr);
}

LazConfigWriter::LazConfigWriter(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset,
                                 const std::string &wkt, const las::EbVlr &extra_bytes_vlr)
    : LazConfig(point_format_id, scale, offset, wkt, extra_bytes_vlr, false)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("LasConfig: Supported point formats are 6 to 8.");
}

// Copy constructor from CopcConfig
LazConfigWriter::LazConfigWriter(const copc::CopcConfig &copc_config)
{
    header_ = std::make_shared<copc::las::LasHeader>(copc_config.LasHeader());
    wkt_ = copc_config.Wkt();
    eb_vlr_ = std::make_shared<copc::las::EbVlr>(copc_config.ExtraBytesVlr());
}

} // namespace copc::las
