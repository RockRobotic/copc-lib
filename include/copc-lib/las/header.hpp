#ifndef COPCLIB_LAS_HEADER_H_
#define COPCLIB_LAS_HEADER_H_

#include <array>
#include <cmath>
#include <limits>
#include <sstream>
#include <string>

#include <copc-lib/geometry/vector3.hpp>
#include <lazperf/header.hpp>
#include <lazperf/readers.hpp>

namespace copc
{
class Box;
namespace las
{
using VlrHeader = lazperf::vlr_header;
// Base class for LasHeaderConfig and LasHeader
class LasHeaderBase
{
  public:
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

    virtual std::string ToString() const = 0;

    // TODO[Leo]: Check with Chris if we can remove some things here
    uint16_t file_source_id{};
    uint16_t global_encoding{};

    uint16_t creation_day{};
    uint16_t creation_year{};

    // default to 0
    int8_t point_format_id{};

    // xyz scale/offset
    Vector3 scale{Vector3::DefaultScale()};
    Vector3 offset{Vector3::DefaultOffset()};
    // xyz min/max for las header
    Vector3 max{};
    Vector3 min{};

    // # of points per return 0-14
    std::array<uint64_t, 15> points_by_return_14{};

  protected:
    LasHeaderBase() = default;
    LasHeaderBase(const int8_t &point_format_id, const Vector3 &scale = Vector3::DefaultScale(),
                  const Vector3 &offset = Vector3::DefaultOffset())
        : point_format_id(point_format_id), scale(scale), offset(offset){};
    std::string guid_{};
    std::string system_identifier_{};
    std::string generating_software_{};
};

// Class used to convert to and from lazperf
class LasHeader : public LasHeaderBase
{
  public:
    LasHeader(){};
    uint16_t NumExtraBytes() const;

    static LasHeader FromLazPerf(const lazperf::header14 &header);
    lazperf::header14 ToLazPerf() const;

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "LasHeader:" << std::endl;
        ss << "\tFile Source ID: " << file_source_id << std::endl;
        ss << "\tGlobal Encoding ID: " << global_encoding << std::endl;
        ss << "\tGUID: " << GUID() << std::endl;
        ss << "\tVersion: " << static_cast<int>(version_major) << "." << static_cast<int>(version_minor) << std::endl;
        ss << "\tSystem Identifier: " << SystemIdentifier() << std::endl;
        ss << "\tGenerating Software: " << GeneratingSoftware() << std::endl;
        ss << "\tCreation (DD/YYYY): (" << creation_day << "/" << creation_year << ")" << std::endl;
        ss << "\tHeader Size: " << header_size << std::endl;
        ss << "\tPoint Offset: " << point_offset << std::endl;
        ss << "\tVLR Count: " << vlr_count << std::endl;
        ss << "\tPoint Format ID: " << static_cast<int>(point_format_id) << std::endl;
        ss << "\tPoint Record Length: " << point_record_length << std::endl;
        ss << "\tPoint Count: " << point_count << std::endl;
        ss << "\tScale: " << scale.ToString() << std::endl;
        ss << "\tOffset: " << offset.ToString() << std::endl;
        ss << "\tMax: " << max.ToString() << std::endl;
        ss << "\tMin: " << min.ToString() << std::endl;
        ss << "\tWave Offset: " << wave_offset << std::endl;
        ss << "\tEVLR Offset: " << evlr_offset << std::endl;
        ss << "\tEVLR count: " << evlr_count << std::endl;
        ss << "\tPoints By Return:" << std::endl;
        for (int i = 0; i < points_by_return_14.size(); i++)
            ss << "\t\t[" << i << "]: " << points_by_return_14[i] << std::endl;
        return ss.str();
    }

    uint8_t version_major{1};
    uint8_t version_minor{4};

    uint16_t header_size{};
    uint32_t point_offset{};
    uint32_t vlr_count{};

    uint16_t point_record_length{};

    uint32_t point_count{};
    std::array<uint32_t, 5> points_by_return{};

    uint64_t wave_offset{0};

    uint64_t evlr_offset{0};
    uint32_t evlr_count{0};
    uint64_t point_count_14{0};

    static const size_t size = 375; // Size of header for LAS 1.4
};

} // namespace las
} // namespace copc
#endif // COPCLIB_LAS_HEADER_H_
