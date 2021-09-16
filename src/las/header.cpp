#include <copc-lib/las/header.hpp>

#include <cstring>

#include <copc-lib/las/utils.hpp>
#include <lazperf/header.hpp>

namespace copc
{
namespace las
{

uint16_t LasHeader::NumExtraBytes() const { return ComputeNumExtraBytes(point_format_id, point_record_length); }

void LasHeader::Unpack(std::istream &in)
{
    //    in.read(reinterpret_cast<const char *>(&magic), 4);
    //    las::pack(file_source_id,in);
    //    las::pack(global_encoding,in);
    //    in.read(reinterpret_cast<const char *>(&guid_), 16);
    //    las::pack(version.major,in);
    //    las::pack(version.minor,in);
    //    in.read(reinterpret_cast<const char *>(&system_identifier_), 32);
    //    in.read(reinterpret_cast<const char *>(&generating_software_), 32);
    //    las::pack(creation.day,in);
    //    las::pack(creation.year,in);
    //    las::pack(header_size,in);
    //    las::pack(point_offset,in);
    //    las::pack(vlr_count,in);
    //    las::pack(point_format_id,in);
    //    las::pack(point_record_length,in);
    //    las::pack(point_count,in);
    //    for (int i = 0; i < 5; ++i)
    //        las::pack(points_by_return[i],in);
    //    las::pack(scale.x,in);
    //    las::pack(scale.y,in);
    //    las::pack(scale.z,in);
    //    las::pack(offset.x,in);
    //    las::pack(offset.y,in);
    //    las::pack(offset.z,in);
    //    las::pack(max.x,in);
    //    las::pack(min.x,in);
    //    las::pack(max.y,in);
    //    las::pack(min.y,in);
    //    las::pack(max.z,in);
    //    las::pack(min.z,in);
    //    las::pack(wave_offset,in);
    //    las::pack(evlr_offset,in);
    //    las::pack(evlr_count,in);
    //    las::pack(point_count_14,in);
    //    for (int i = 0; i < 15; ++i)
    //        las::pack(points_by_return_14[i],in);
}

void LasHeader::Pack(std::ostream &out) const
{
    out.write(reinterpret_cast<const char *>(&magic), 4);
    las::pack(file_source_id, out);
    las::pack(global_encoding, out);
    out.write(reinterpret_cast<const char *>(&guid_), 16);
    las::pack(version.major, out);
    las::pack(version.minor, out);
    out.write(reinterpret_cast<const char *>(&system_identifier_), 32);
    out.write(reinterpret_cast<const char *>(&generating_software_), 32);
    las::pack(creation.day, out);
    las::pack(creation.year, out);
    las::pack(header_size, out);
    las::pack(point_offset, out);
    las::pack(vlr_count, out);
    las::pack(point_format_id, out);
    las::pack(point_record_length, out);
    las::pack(point_count, out);
    for (int i = 0; i < 5; ++i)
        las::pack(points_by_return[i], out);
    las::pack(scale.x, out);
    las::pack(scale.y, out);
    las::pack(scale.z, out);
    las::pack(offset.x, out);
    las::pack(offset.y, out);
    las::pack(offset.z, out);
    las::pack(max.x, out);
    las::pack(min.x, out);
    las::pack(max.y, out);
    las::pack(min.y, out);
    las::pack(max.z, out);
    las::pack(min.z, out);
    las::pack(wave_offset, out);
    las::pack(evlr_offset, out);
    las::pack(evlr_count, out);
    las::pack(point_count_14, out);
    for (int i = 0; i < 15; ++i)
        las::pack(points_by_return_14[i], out);
}

void LasHeader::FromLazPerf(const lazperf::header14 &header)
{
    magic = header.magic;
    file_source_id = header.file_source_id;
    global_encoding = header.global_encoding;
    guid_ = header.guid;
    version.major = header.version.major;
    version.minor = header.version.minor;
    system_identifier_ = header.system_identifier;
    generating_software_ = header.generating_software;
    creation.day = header.creation.day;
    creation.year = header.creation.year;
    header_size = header.header_size;
    point_offset = header.point_offset;
    vlr_count = header.vlr_count;
    point_format_id = header.point_format_id;
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
    h.version.major = version.major;
    h.version.minor = version.minor;
    std::strcpy(h.system_identifier, system_identifier_.c_str());
    std::strcpy(h.generating_software, generating_software_.c_str());
    h.creation.day = creation.day;
    h.creation.year = creation.year;
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
