#ifndef COPCLIB_LAS_HEADER_H_
#define COPCLIB_LAS_HEADER_H_

#include <array>
#include <limits>
#include <sstream>
#include <string>

#include <lazperf/header.hpp>
#include <lazperf/readers.hpp>

namespace copc
{
const double DEFAULT_SCALE = 0.01;

struct Vector3
{
    Vector3() : x(0), y(0), z(0) {}
    Vector3(const double &x, const double &y, const double &z) : x(x), y(y), z(z) {}
    static Vector3 DefaultScale() { return Vector3(DEFAULT_SCALE, DEFAULT_SCALE, DEFAULT_SCALE); }
    static Vector3 DefaultOffset() { return Vector3{}; }

    Vector3(const std::vector<double> &vec)
    {
        if (vec.size() != 3)
            throw std::runtime_error("Vector must be of size 3.");
        x = vec[0];
        y = vec[1];
        z = vec[2];
    }

    double x{};
    double y{};
    double z{};

    Vector3 &operator=(const lazperf::vector3 &a)
    {
        x = a.x;
        y = a.y;
        z = a.z;

        return *this; // Return a reference to myself.
    }

    std::string ToString()
    {
        std::stringstream ss;
        ss << "Vector3: x=" << x << ", y=" << y << ", z=" << z;
        return ss.str();
    }

    bool operator==(Vector3 a) const { return x == a.x && y == a.y && z == a.z; }
};

namespace las
{
using VlrHeader = lazperf::vlr_header;

class LasHeader
{
  public:
    LasHeader(){};
    uint16_t NumExtraBytes() const;

    static LasHeader FromLazPerf(const lazperf::header14 &header);
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

    Vector3 scale{Vector3::DefaultScale()};
    Vector3 offset{Vector3::DefaultOffset()};
    // xyz min/max for las header
    Vector3 max{};
    Vector3 min{};

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
