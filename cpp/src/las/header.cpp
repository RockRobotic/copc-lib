#include "copc-lib/las/header.hpp"

#include <cstring>
#include <limits>
#include <stdexcept>

#include "copc-lib/geometry/box.hpp"
#include "copc-lib/las/utils.hpp"
#include <lazperf/header.hpp>

namespace copc::las
{
Box LasHeader::Bounds() const { return Box(min, max); }

uint16_t LasHeader::EbByteSize() const { return copc::las::EbByteSize(point_format_id_, point_record_length_); }

LasHeader LasHeader::FromLazPerf(const lazperf::header14 &header)
{
    LasHeader h;
    h.file_source_id = header.file_source_id;
    // Check WKT bit
    if (!(header.global_encoding & 0x10))
        throw std::runtime_error("LasHeader::FromLazPerf: WKT bit must be set.");
    h.global_encoding = header.global_encoding;
    h.guid_ = header.guid;
    if (header.version.major != 1 || header.version.minor != 4)
        throw std::runtime_error("LasHeader::FromLazPerf: Header version needs to be 1.4");
    h.system_identifier_ = header.system_identifier;
    h.generating_software_ = header.generating_software;
    h.creation_day = header.creation.day;
    h.creation_year = header.creation.year;
    if (header.header_size != 375)
        throw std::runtime_error("LasHeader::FromLazPerf: Header size must be 375.");
    h.point_offset_ = header.point_offset;
    h.vlr_count_ = header.vlr_count;
    if (header.point_format_id < 6 || header.point_format_id > 8)
        throw std::runtime_error("LasHeader::FromLazPerf: Supported point formats are 6 to 8.");
    h.point_format_id_ = static_cast<int8_t>(header.point_format_id);
    h.point_record_length_ = header.point_record_length;
    std::copy(std::begin(header.points_by_return), std::end(header.points_by_return), std::begin(h.points_by_return));
    h.scale_.x = header.scale.x;
    h.scale_.y = header.scale.y;
    h.scale_.z = header.scale.z;
    h.offset_.x = header.offset.x;
    h.offset_.y = header.offset.y;
    h.offset_.z = header.offset.z;
    h.max.x = header.maxx;
    h.min.x = header.minx;
    h.max.y = header.maxy;
    h.min.y = header.miny;
    h.max.z = header.maxz;
    h.min.z = header.minz;
    h.evlr_offset_ = header.evlr_offset;
    h.evlr_count_ = header.evlr_count;
    h.point_count_ = header.point_count_14;
    std::copy(std::begin(header.points_by_return_14), std::end(header.points_by_return_14),
              std::begin(h.points_by_return));

    return h;
}
lazperf::header14 LasHeader::ToLazPerf(uint32_t point_offset, uint64_t point_count, uint64_t evlr_offset,
                                       uint32_t evlr_count, bool eb_flag, bool extended_stats_flag) const
{
    lazperf::header14 h;
    h.file_source_id = file_source_id;
    h.global_encoding = global_encoding;
    h.global_encoding |= (1 << 4); // Set the WKT bit.
    std::strncpy(h.guid, guid_.c_str(), 16);
    h.version.major = version_major_;
    h.version.minor = version_minor_;
    std::strncpy(h.system_identifier, system_identifier_.c_str(), 32);
    std::strncpy(h.generating_software, generating_software_.c_str(), 32);
    h.creation.day = creation_day;
    h.creation.year = creation_year;
    h.header_size = header_size_;
    h.point_offset = point_offset;
    h.vlr_count = 3; // copc_info + copc_extent + laz;
    // If there are Extra Bytes, count an extra VLR
    if (eb_flag)
        h.vlr_count++;
    // If there are Extended Stats, count an extra VLR
    if (extended_stats_flag)
        h.vlr_count++;
    h.point_format_id = point_format_id_;
    h.point_format_id |= (1 << 7); // Do the lazperf trick
    h.point_record_length = point_record_length_;
    // Set the legacy point count as per LAS specs
    if (point_count > (std::numeric_limits<uint32_t>::max)())
        h.point_count = 0;
    else
        h.point_count = (uint32_t)point_count;
    std::fill(h.points_by_return, h.points_by_return + 5, 0); // Fill with zeros

    h.offset.x = offset_.x;
    h.offset.y = offset_.y;
    h.offset.z = offset_.z;

    h.scale.x = scale_.x;
    h.scale.y = scale_.y;
    h.scale.z = scale_.z;

    h.maxx = max.x;
    h.minx = min.x;
    h.maxy = max.y;
    h.miny = min.y;
    h.maxz = max.z;
    h.minz = min.z;

    h.evlr_offset = evlr_offset;
    h.evlr_count = evlr_count;
    h.point_count_14 = point_count;

    std::copy(std::begin(points_by_return), std::end(points_by_return), std::begin(h.points_by_return_14));
    return h;
}

std::string LasHeader::ToString() const
{
    std::stringstream ss;
    ss << "LasHeader:" << std::endl;
    ss << "\tFile Source ID: " << file_source_id << std::endl;
    ss << "\tGlobal Encoding ID: " << global_encoding << std::endl;
    ss << "\tGUID: " << GUID() << std::endl;
    ss << "\tVersion: " << static_cast<int>(version_major_) << "." << static_cast<int>(version_minor_) << std::endl;
    ss << "\tSystem Identifier: " << SystemIdentifier() << std::endl;
    ss << "\tGenerating Software: " << GeneratingSoftware() << std::endl;
    ss << "\tCreation (Day/Year): (" << creation_day << "/" << creation_year << ")" << std::endl;
    ss << "\tHeader Size: " << header_size_ << std::endl;
    ss << "\tPoint Offset: " << point_offset_ << std::endl;
    ss << "\tVLR Count: " << vlr_count_ << std::endl;
    ss << "\tPoint Format ID: " << static_cast<int>(point_format_id_) << std::endl;
    ss << "\tPoint Record Length: " << point_record_length_ << std::endl;
    ss << "\tPoint Count: " << point_count_ << std::endl;
    ss << "\tScale: " << scale_.ToString() << std::endl;
    ss << "\tOffset: " << offset_.ToString() << std::endl;
    ss << "\tMax: " << max.ToString() << std::endl;
    ss << "\tMin: " << min.ToString() << std::endl;
    ss << "\tEVLR Offset: " << evlr_offset_ << std::endl;
    ss << "\tEVLR count: " << evlr_count_ << std::endl;
    ss << "\tPoints By Return:" << std::endl;
    for (int i = 0; i < points_by_return.size(); i++)
        ss << "\t\t[" << i << "]: " << points_by_return[i] << std::endl;
    return ss.str();
}

} // namespace copc::las
