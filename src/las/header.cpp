#include <copc-lib/las/header.hpp>

#include <cstring>

#include <copc-lib/las/utils.hpp>
#include <lazperf/header.hpp>

namespace copc::las
{

uint16_t LasHeader::NumExtraBytes() const { return ComputeNumExtraBytes(point_format_id, point_record_length); }

LasHeader LasHeader::FromLazPerf(const lazperf::header14 &header)
{
    LasHeader h;
    h.file_source_id = header.file_source_id;
    h.global_encoding = header.global_encoding;
    h.guid_ = header.guid;
    h.version_major = header.version.major;
    h.version_minor = header.version.minor;
    h.system_identifier_ = header.system_identifier;
    h.generating_software_ = header.generating_software;
    h.creation_day = header.creation.day;
    h.creation_year = header.creation.year;
    h.header_size = header.header_size;
    h.point_offset = header.point_offset;
    h.vlr_count = header.vlr_count;
    h.point_format_id = static_cast<int8_t>(header.point_format_id);
    h.point_record_length = header.point_record_length;
    h.point_count = header.point_count;
    std::copy(std::begin(header.points_by_return), std::end(header.points_by_return), std::begin(h.points_by_return));
    h.scale.x = header.scale.x;
    h.scale.y = header.scale.y;
    h.scale.z = header.scale.z;
    h.offset.x = header.offset.x;
    h.offset.y = header.offset.y;
    h.offset.z = header.offset.z;
    h.max.x = header.maxx;
    h.min.x = header.minx;
    h.max.y = header.maxy;
    h.min.y = header.miny;
    h.max.z = header.maxz;
    h.min.z = header.minz;
    h.wave_offset = header.wave_offset;
    h.evlr_offset = header.evlr_offset;
    h.evlr_count = header.evlr_count;
    h.point_count_14 = header.point_count_14;
    std::copy(std::begin(header.points_by_return_14), std::end(header.points_by_return_14),
              std::begin(h.points_by_return_14));
    return h;
}
lazperf::header14 LasHeader::ToLazPerf() const
{
    lazperf::header14 h;
    h.file_source_id = file_source_id;
    h.global_encoding = global_encoding;
    std::strncpy(h.guid, guid_.c_str(), 16);
    h.version.major = version_major;
    h.version.minor = version_minor;
    std::strncpy(h.system_identifier, system_identifier_.c_str(), 32);
    std::strncpy(h.generating_software, generating_software_.c_str(), 32);
    h.creation.day = creation_day;
    h.creation.year = creation_year;
    h.header_size = header_size;
    h.point_offset = point_offset;
    h.vlr_count = vlr_count;
    h.point_format_id = point_format_id;
    h.point_record_length = point_record_length;
    h.point_count = point_count;
    std::copy(std::begin(points_by_return), std::end(points_by_return), std::begin(h.points_by_return));

    h.offset.x = offset.x;
    h.offset.y = offset.y;
    h.offset.z = offset.z;

    h.scale.x = scale.x;
    h.scale.y = scale.y;
    h.scale.z = scale.z;

    h.maxx = max.x;
    h.minx = min.x;
    h.maxy = max.y;
    h.miny = min.y;
    h.maxz = max.z;
    h.minz = min.z;

    h.wave_offset = wave_offset;
    h.evlr_offset = evlr_offset;
    h.evlr_count = evlr_count;
    h.point_count_14 = point_count_14;

    std::copy(std::begin(points_by_return_14), std::end(points_by_return_14), std::begin(h.points_by_return_14));
    return h;
}

} // namespace copc::las
