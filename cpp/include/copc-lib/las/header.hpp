#ifndef COPCLIB_LAS_HEADER_H_
#define COPCLIB_LAS_HEADER_H_

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <sstream>
#include <string>

#include "copc-lib/geometry/vector3.hpp"
#include <lazperf/header.hpp>
#include <lazperf/readers.hpp>

namespace copc
{
class Box;
namespace las
{

class LasHeader
{
  public:
    LasHeader() = default;
    uint16_t EbByteSize() const;
    LasHeader(int8_t point_format_id, uint16_t point_record_length, const Vector3 &scale, const Vector3 &offset)
        : point_format_id_(point_format_id), point_record_length_{point_record_length}, scale(scale), offset(offset){};

    // Constructor for python pickling
    // TODO: Add a CMAKE flag to only compute python-specific code when python is compiled
    LasHeader(int8_t point_format_id, uint16_t point_record_length, uint32_t point_offset, uint64_t point_count,
              uint32_t vlr_count, const Vector3 &scale, const Vector3 &offset, uint64_t evlr_offset,
              uint32_t evlr_count)
        : point_format_id_(point_format_id), point_record_length_(point_record_length), point_offset_(point_offset),
          point_count_(point_count), vlr_count_(vlr_count), scale(scale), offset(offset), evlr_offset_(evlr_offset),
          evlr_count_(evlr_count){};

    static LasHeader FromLazPerf(const lazperf::header14 &header);
    lazperf::header14 ToLazPerf(uint32_t point_offset, uint64_t point_count, uint64_t evlr_offset, uint32_t evlr_count,
                                bool eb_flag, bool extended_stats_flag) const;

    std::string ToString() const;

    // Getters/Setters for protected attributes

    uint8_t PointFormatId() const { return point_format_id_; }
    uint16_t PointRecordLength() const { return point_record_length_; }
    Vector3 Scale() const { return scale; }
    Vector3 Offset() const { return offset; }
    uint64_t PointCount() const { return point_count_; }
    uint32_t PointOffset() const { return point_offset_; }
    uint32_t VlrCount() const { return vlr_count_; }
    uint32_t EvlrCount() const { return evlr_count_; }
    uint64_t EvlrOffset() const { return evlr_offset_; }

    void GUID(const std::string &guid)
    {
        if (guid.size() > 16)
            throw std::runtime_error("GUID length must be <= 16.");
        guid_ = guid;
    }
    std::string GUID() const { return guid_; }

    void SystemIdentifier(const std::string &system_identifier)
    {
        if (system_identifier.size() > 32)
            throw std::runtime_error("System Identifier length must be <= 32.");
        system_identifier_ = system_identifier;
    }
    std::string SystemIdentifier() const { return system_identifier_; }

    void GeneratingSoftware(const std::string &generating_software)
    {
        if (generating_software.size() > 32)
            throw std::runtime_error("System Identifier length must be <= 32.");
        generating_software_ = generating_software;
    }
    std::string GeneratingSoftware() const { return generating_software_; }

    // Returns the scaled size of the maximum dimension of the point cloud
    double Span() const { return std::max({max.x - min.x, max.y - min.y, max.z - min.z}); }

    // Returns a box that fits the dimensions of the point cloud based on min and max
    Box Bounds() const;

    // Apply Las scale factors to Vector3 or double
    Vector3 ApplyScale(const Vector3 &unscaled_value) const { return unscaled_value * scale + offset; }
    Vector3 ApplyInverseScale(const Vector3 &scaled_value) const { return scaled_value / scale - offset; }
    double ApplyScaleX(double unscaled_value) const { return unscaled_value * scale.x + offset.x; }
    double ApplyScaleY(double unscaled_value) const { return unscaled_value * scale.y + offset.y; }
    double ApplyScaleZ(double unscaled_value) const { return unscaled_value * scale.z + offset.z; }
    double ApplyInverseScaleX(double scaled_value) const { return (scaled_value - offset.x) / scale.x; }
    double ApplyInverseScaleY(double scaled_value) const { return (scaled_value - offset.y) / scale.y; }
    double ApplyInverseScaleZ(double scaled_value) const { return (scaled_value - offset.z) / scale.z; }

    uint16_t file_source_id{};
    uint16_t global_encoding{};

    uint16_t creation_day{};
    uint16_t creation_year{};

    // xyz min/max for las header
    Vector3 max{};
    Vector3 min{};

    // xyz scale/offset
    Vector3 scale{Vector3::DefaultScale()};
    Vector3 offset{Vector3::DefaultOffset()};

    // # of points per return 0-14
    std::array<uint64_t, 15> points_by_return{};

  protected:
    int8_t point_format_id_{6};
    uint16_t point_record_length_{};

    uint64_t point_count_{};
    uint32_t point_offset_{};
    uint32_t vlr_count_{};

    uint64_t evlr_offset_{};
    uint32_t evlr_count_{};

    std::string guid_{};
    std::string system_identifier_{};
    std::string generating_software_{};

    const uint8_t version_major_{1};
    const uint8_t version_minor_{4};

    const uint16_t header_size_{375};
};

} // namespace las
} // namespace copc
#endif // COPCLIB_LAS_HEADER_H_
