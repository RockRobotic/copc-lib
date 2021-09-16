#ifndef COPCLIB_LAS_HEADER_H_
#define COPCLIB_LAS_HEADER_H_

#include <array>
#include <limits>
#include <string>

#include <lazperf/header.hpp>
#include <lazperf/readers.hpp>

namespace copc
{

struct vector3
{
    vector3() : x(0), y(0), z(0) {}
    vector3(const double &x, const double &y, const double &z) : x(x), y(y), z(z) {}
    vector3(const std::vector<double> &vec)
    {
        if (vec.size() != 3)
            throw std::runtime_error("Vector must be of size 3.");
        x = vec[0];
        y = vec[1];
        z = vec[2];
    }

    double x;
    double y;
    double z;

    vector3 &operator=(const lazperf::vector3 &a)
    {
        x = a.x;
        y = a.y;
        z = a.z;

        return *this; // Return a reference to myself.
    }

    bool operator==(vector3 a) const { return x == a.x && y == a.y && z == a.z; }
};

namespace las
{
using VlrHeader = lazperf::vlr_header;

class LasHeader
{
  public:
    LasHeader(){};
    LasHeader(const lazperf::header14 &header) { FromLazPerf(header); }
    uint16_t NumExtraBytes() const;

    void FromLazPerf(const lazperf::header14 &header);
    lazperf::header14 ToLazPerf() const;

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

    std::string magic{"LASF"};
    uint16_t file_source_id{};
    uint16_t global_encoding{};

    uint8_t version_major{1};
    uint8_t version_minor{4};

    uint16_t creation_day{};
    uint16_t creation_year{};

    uint16_t header_size{};
    uint32_t point_offset{};
    uint32_t vlr_count{};

    int8_t point_format_id{};
    uint16_t point_record_length{};

    uint32_t point_count{};
    std::array<uint32_t, 5> points_by_return{};

    vector3 scale;
    vector3 offset;
    // xyz min/max for las header
    vector3 max{std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(),
                std::numeric_limits<double>::lowest()};
    vector3 min{(std::numeric_limits<double>::max)(), (std::numeric_limits<double>::max)(),
                (std::numeric_limits<double>::max)()};

    uint64_t wave_offset{0};

    uint64_t evlr_offset{0};
    uint32_t evlr_count{0};
    uint64_t point_count_14{0};
    std::array<uint64_t, 15> points_by_return_14{};

    static const size_t size = 375; // Size of header for LAS 1.4
  private:
    std::string guid_{};
    std::string system_identifier_{};
    std::string generating_software_{};
};

} // namespace las
} // namespace copc
#endif // COPCLIB_LAS_HEADER_H_
