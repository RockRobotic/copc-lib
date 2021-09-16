#include <copc-lib/las/header.hpp>

#include <cstring>

#include <copc-lib/las/utils.hpp>
#include <lazperf/header.hpp>

namespace copc
{
namespace las
{

uint16_t LasHeader::NumExtraBytes() const { return ComputeNumExtraBytes(point_format_id, point_record_length); }

void LasHeader::FromLazPerf(const lazperf::header14 &header)
{
    magic = header.magic;
    file_source_id = header.file_source_id;
    global_encoding = header.global_encoding;
    guid_ = header.guid;
    version_major = header.version.major;
    version_minor = header.version.minor;
    system_identifier_ = header.system_identifier;
    generating_software_ = header.generating_software;
    creation_day = header.creation.day;
    creation_year = header.creation.year;
    header_size = header.header_size;
    point_offset = header.point_offset;
    vlr_count = header.vlr_count;
    point_format_id = static_cast<int8_t>(header.point_format_id);
    point_record_length = header.point_record_length;
    point_count = header.point_count;
    std::copy(std::begin(header.points_by_return), std::end(header.points_by_return), std::begin(points_by_return));
    scale.x = header.scale.x;
    scale.y = header.scale.y;
    scale.z = header.scale.z;
    offset.x = header.offset.x;
    offset.y = header.offset.y;
    offset.z = header.offset.z;
    max.x = header.maxx;
    min.x = header.minx;
    max.y = header.maxy;
    min.y = header.miny;
    max.z = header.maxz;
    min.z = header.minz;
    wave_offset = header.wave_offset;
    evlr_offset = header.evlr_offset;
    evlr_count = header.evlr_count;
    std::copy(std::begin(header.points_by_return_14), std::end(header.points_by_return_14),
              std::begin(points_by_return_14));
}
lazperf::header14 LasHeader::ToLazPerf() const
{
    lazperf::header14 h;
    std::strcpy(h.magic, magic.c_str());
    h.file_source_id = file_source_id;
    h.global_encoding = global_encoding;
    std::strcpy(h.guid, guid_.c_str());
    h.version.major = version_major;
    h.version.minor = version_minor;
    std::strcpy(h.system_identifier, system_identifier_.c_str());
    std::strcpy(h.generating_software, generating_software_.c_str());
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

    std::copy(std::begin(points_by_return_14), std::end(points_by_return_14), std::begin(h.points_by_return_14));
    return h;
}

} // namespace las
} // namespace copc
