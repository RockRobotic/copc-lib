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

class Point;
class LasHeader
{
  public:
    static const uint16_t HEADER_SIZE_BYTES = 375;

    LasHeader() = default;
    LasHeader(const LasHeader &las_header) = default; // default copy constructor
    uint16_t EbByteSize() const;
    LasHeader(int8_t point_format_id, uint16_t point_record_length, const Vector3 &scale, const Vector3 &offset,
              bool copc_flag)
        : point_format_id_(point_format_id), point_record_length_{point_record_length}, scale_(scale), offset_(offset),
          copc_flag_(copc_flag){};

    // Constructor for python pickling
    // TODO: Add a CMAKE flag to only compile python-specific code when python is compiled
    LasHeader(int8_t point_format_id, uint16_t point_record_length, uint32_t point_offset, uint64_t point_count,
              uint32_t vlr_count, const Vector3 &scale, const Vector3 &offset, uint64_t evlr_offset,
              uint32_t evlr_count, bool copc_flag)
        : point_format_id_(point_format_id), point_record_length_(point_record_length), point_offset_(point_offset),
          point_count_(point_count), vlr_count_(vlr_count), scale_(scale), offset_(offset), evlr_offset_(evlr_offset),
          evlr_count_(evlr_count), copc_flag_(copc_flag){};

    // Copy constructor with modification of protected attributes
    LasHeader(const LasHeader &header, int8_t point_format_id, uint16_t point_record_length, const Vector3 &scale,
              const Vector3 &offset);

    // Copy constructor with modification of copc flag
    LasHeader(const LasHeader &header, bool is_copc);

    static LasHeader FromLazPerf(const lazperf::header14 &header);
    lazperf::header14 ToLazPerf(uint32_t point_offset, uint64_t point_count, uint64_t evlr_offset, uint32_t evlr_count,
                                bool eb_flag, bool extended_stats_flag) const;

    std::string ToString() const;
    friend std::ostream &operator<<(std::ostream &os, LasHeader const &value)
    {
        os << value.ToString();
        return os;
    }

    // Getters/Setters for protected attributes

    int8_t PointFormatId() const { return point_format_id_; }
    uint16_t PointRecordLength() const { return point_record_length_; }
    Vector3 Scale() const { return scale_; }
    Vector3 Offset() const { return offset_; }
    uint64_t PointCount() const { return point_count_; }
    uint32_t PointOffset() const { return point_offset_; }
    uint32_t VlrCount() const { return vlr_count_; }
    uint32_t EvlrCount() const { return evlr_count_; }
    uint64_t EvlrOffset() const { return evlr_offset_; }
    bool IsCopc() const { return copc_flag_; }

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

    void CheckAndUpdateBounds(const Point &point);

    void SetGpsTimeBit();

    // Apply Las scale factors to Vector3 or double
    Vector3 ApplyScale(const Vector3 &unscaled_value) const { return unscaled_value * scale_ + offset_; }
    Vector3 ApplyInverseScale(const Vector3 &scaled_value) const { return scaled_value / scale_ - offset_; }
    double ApplyScaleX(double unscaled_value) const { return unscaled_value * scale_.x + offset_.x; }
    double ApplyScaleY(double unscaled_value) const { return unscaled_value * scale_.y + offset_.y; }
    double ApplyScaleZ(double unscaled_value) const { return unscaled_value * scale_.z + offset_.z; }
    int32_t RemoveScaleX(double scaled_value) const
    {
        return static_cast<int32_t>(std::round((scaled_value - offset_.x) / scale_.x));
    }
    int32_t RemoveScaleY(double scaled_value) const
    {
        return static_cast<int32_t>(std::round((scaled_value - offset_.y) / scale_.y));
    }
    int32_t RemoveScaleZ(double scaled_value) const
    {
        return static_cast<int32_t>(std::round((scaled_value - offset_.z) / scale_.z));
    }

    uint16_t file_source_id{};
    uint16_t global_encoding{};

    uint16_t creation_day{};
    uint16_t creation_year{};

    // xyz min/max for las header
    Vector3 max{};
    Vector3 min{};

    // # of points per return 0-14
    std::array<uint64_t, 15> points_by_return{};

  protected:
    // xyz scale/offset
    Vector3 scale_{Vector3::DefaultScale()};
    Vector3 offset_{Vector3::DefaultOffset()};

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

    static const uint8_t version_major_{1};
    static const uint8_t version_minor_{4};

    bool copc_flag_{true}; // flag to set true if the header is used for a COPC file
};

} // namespace las
} // namespace copc
#endif // COPCLIB_LAS_HEADER_H_
