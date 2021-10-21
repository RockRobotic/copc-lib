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
    LasHeader(int8_t point_format_id, const Vector3 &scale, const Vector3 &offset)
        : point_format_id(point_format_id), scale(scale), offset(offset){};

    static LasHeader FromLazPerf(const lazperf::header14 &header);
    lazperf::header14 ToLazPerf() const;

    std::string ToString() const;

    // Getters/Setters for string attributes
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

    double GetSpan() const { return std::max({max.x - min.x, max.y - min.y, max.z - min.z}); }
    Box GetBounds() const;

    // Apply Las scale factors to Vector3 or double
    Vector3 ApplyScale(const Vector3 &unscaled_value) const { return unscaled_value * scale + offset; }
    Vector3 ApplyInverseScale(const Vector3 &scaled_value) const { return scaled_value / scale - offset; }
    double ApplyScaleX(double unscaled_value) const { return unscaled_value * scale.x + offset.x; }
    double ApplyScaleY(double unscaled_value) const { return unscaled_value * scale.y + offset.y; }
    double ApplyScaleZ(double unscaled_value) const { return unscaled_value * scale.z + offset.z; }
    double ApplyInverseScaleX(double scaled_value) const { return (scaled_value - offset.x) / scale.x; }
    double ApplyInverseScaleY(double scaled_value) const { return (scaled_value - offset.y) / scale.y; }
    double ApplyInverseScaleZ(double scaled_value) const { return (scaled_value - offset.z) / scale.z; }

    int8_t point_format_id{6};
    uint64_t point_count{};
    uint16_t point_record_length{};

    uint16_t file_source_id{};
    uint16_t global_encoding{};

    uint16_t creation_day{};
    uint16_t creation_year{};

    // xyz scale/offset
    Vector3 scale{Vector3::DefaultScale()};
    Vector3 offset{Vector3::DefaultOffset()};
    // xyz min/max for las header
    Vector3 max{};
    Vector3 min{};

    // # of points per return 0-14
    std::array<uint64_t, 15> points_by_return{};

    uint32_t point_offset{};
    uint32_t vlr_count{};

    uint64_t evlr_offset{};
    uint32_t evlr_count{};

  protected:
    std::string guid_{};
    std::string system_identifier_{};
    std::string generating_software_{};

    const uint8_t version_major{1};
    const uint8_t version_minor{4};

    const uint16_t header_size{375};
};

} // namespace las
} // namespace copc
#endif // COPCLIB_LAS_HEADER_H_
