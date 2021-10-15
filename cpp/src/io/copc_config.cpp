#include "copc-lib/io/copc_config.hpp"

namespace copc
{

CopcConfig::CopcConfig(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset,
                       const las::EbVlr &extra_bytes_vlr)
    : point_format_id_(point_format_id), las_header_base(scale, offset),
      copc_extents_(point_format_id, extra_bytes_vlr.items.size()), extra_bytes_vlr_(extra_bytes_vlr)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("LasConfig: Supported point formats are 6 to 8.");
}

CopcConfig::CopcConfig(const las::LasHeader &config, const CopcInfo &copc_info, const CopcExtents &copc_extents,
                       const std::string &wkt, const las::EbVlr &extra_bytes_vlr)
    : copc_info(copc_info), copc_extents_(copc_extents), wkt(wkt), extra_bytes_vlr_(extra_bytes_vlr)
{
    las_header_base.file_source_id = config.file_source_id;
    las_header_base.global_encoding = config.global_encoding;
    las_header_base.creation_day = config.creation_day;
    las_header_base.creation_year = config.creation_year;
    point_format_id_ = config.point_format_id;

    if (point_format_id_ < 6 || point_format_id_ > 8)
        throw std::runtime_error("LasConfig: Supported point formats are 6 to 8.");

    las_header_base.GUID(config.GUID());
    las_header_base.SystemIdentifier(config.SystemIdentifier());
    las_header_base.GeneratingSoftware(config.GeneratingSoftware());

    las_header_base.offset = config.offset;
    las_header_base.scale = config.scale;

    las_header_base.max = config.max;
    las_header_base.min = config.min;

    las_header_base.points_by_return = config.points_by_return;
}

std::string CopcConfig::ToString() const
{
    std::stringstream ss;
    ss << "CopcConfig:" << std::endl;
    ss << las_header_base.ToString();
    ss << copc_info.ToString() << std::endl;
    ss << "\twkt: " << wkt << std::endl;
    ss << "\tNumber of Extra Bytes: " << NumExtraBytes() << std::endl;
    return ss.str();
}

} // namespace copc
