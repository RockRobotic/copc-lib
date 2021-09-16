#ifndef COPCLIB_LAS_HEADER_H_
#define COPCLIB_LAS_HEADER_H_

#include <array>

#include <lazperf/header.hpp>
#include <lazperf/readers.hpp>

namespace copc::las
{
using VlrHeader = lazperf::vlr_header;
using LasHeader = lazperf::header14;

// class OurLasHeader(){
// public:
//    std::array<char,4> magic{ 'L', 'A', 'S', 'F' };
//    uint16_t file_source_id {};
//    uint16_t global_encoding {};
//    std::array<char,16> guid {};
//
//    struct {
//        uint8_t major {1};
//        uint8_t minor {4};
//    } version;
//
//    struct {
//        uint16_t day {};
//        uint16_t year {};
//    } creation;
//
//    uint16_t header_size {};
//    uint32_t point_offset {};
//    uint32_t vlr_count {};
//
//    uint8_t point_format_id {};
//    uint16_t point_record_length {};
//
//    uint32_t point_count {};
//    uint32_t points_by_return[5] {};
//
//    vector3 scale;
//    vector3 offset;
//    double maxx { std::numeric_limits<double>::lowest() };
//    double minx { (std::numeric_limits<double>::max)() };
//    double maxy { std::numeric_limits<double>::lowest() };
//    double miny { (std::numeric_limits<double>::max)() };
//    double maxz { std::numeric_limits<double>::lowest() };
//    double minz { (std::numeric_limits<double>::max)() };
//
//    size_t sizeFromVersion() const;
//    int ebCount() const;
//    int pointFormat() const;
//    bool compressed() const;
//    static int minorVersion(std::istream& in);
//
//    static header14 create(std::istream& in);
//    void read(std::istream& in);
//    void write(std::ostream& out) const;
//    static const size_t Size;
//
//    uint64_t evlr_offset {0};
//    uint32_t evlr_count {0};
//    uint64_t point_count_14 {0};
//    uint64_t points_by_return_14[15] {};
// private:
//    std::string system_identifier{};
//    std::string generating_software{};
//}
} // namespace copc::las
#endif // COPCLIB_LAS_HEADER_H_
