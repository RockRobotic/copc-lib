#include "copc-lib/las/laz_config.hpp"

#include <stdexcept>
#include <utility>

#include "copc-lib/copc/copc_config.hpp"

namespace copc::las
{
LazConfig::LazConfig(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset, std::string wkt,
                     const las::EbVlr &extra_bytes_vlr, bool copc_flag)
    : wkt_(std::move(wkt))
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("LasConfig: Supported point formats are 6 to 8.");
    header_ = std::make_shared<las::LasHeader>(
        point_format_id, las::PointBaseByteSize(point_format_id) + las::NumBytesFromExtraBytes(extra_bytes_vlr.items),
        scale, offset, copc_flag);
    eb_vlr_ = std::make_shared<las::EbVlr>(extra_bytes_vlr);
}

// Copy constructor from CopcConfig
LazConfig::LazConfig(const copc::CopcConfig &copc_config)
{
    header_ = std::make_shared<copc::las::LasHeader>(copc_config.LasHeader());
    wkt_ = copc_config.Wkt();
    eb_vlr_ = std::make_shared<copc::las::EbVlr>(copc_config.ExtraBytesVlr());
}

// Copy constructor from LazConfigWriter
LazConfig::LazConfig(const LazConfigWriter &laz_config_writer)
{
    header_ = std::make_shared<copc::las::LasHeader>(laz_config_writer.LasHeader());
    wkt_ = laz_config_writer.Wkt();
    eb_vlr_ = std::make_shared<copc::las::EbVlr>(laz_config_writer.ExtraBytesVlr());
}

LazConfigWriter::LazConfigWriter(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset,
                                 const std::string &wkt, const las::EbVlr &extra_bytes_vlr)
    : LazConfig(point_format_id, scale, offset, wkt, extra_bytes_vlr, false)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("LasConfig: Supported point formats are 6 to 8.");
}

} // namespace copc::las
