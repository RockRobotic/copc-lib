#ifndef COPCLIB_IO_WRITER_H_
#define COPCLIB_IO_WRITER_H_

#include <ostream>

#include <copc-lib/copc/file.hpp>
#include <copc-lib/las/header.hpp>

namespace copc::io
{
const uint32_t VARIABLE_CHUNK_SIZE = (std::numeric_limits<uint32_t>::max)();

struct vector3
{
    vector3() : x(0), y(0), z(0) {}

    vector3(double x, double y, double z) : x(x), y(y), z(z) {}

    double x;
    double y;
    double z;
};

class Writer
{
  public:
    struct LasConfig;
    Writer(std::ostream &out_stream, LasConfig config, int span = 0, std::string wkt = "");

    void Close();

    std::shared_ptr<CopcFile> file;
    std::ostream &out_stream;

    struct LasConfig
    {
        uint16_t file_source_id{};
        uint16_t global_encoding{};
        char guid[16]{};

        char system_identifier[32]{};
        char generating_software[32]{};

        struct
        {
            uint16_t day{};
            uint16_t year{};
        } creation;

        uint8_t point_format_id{};

        vector3 scale{0.01, 0.01, 0.01};
        vector3 offset{0, 0, 0};
    };

  private:
    // header + COPC vlr + LAZ vlr (max 4 items)
    const int OFFSET_TO_POINT_DATA = 375 + (54 + 160) + (54 + (34 + 4 * 6));
    const uint64_t FIRST_CHUNK_OFFSET = OFFSET_TO_POINT_DATA + sizeof(uint64_t);

    std::vector<lazperf::chunk> chunks_;

    void WriteHeader();
    void WriteChunkTable();

    las::LasHeader HeaderFromConfig(LasConfig config);
};
} // namespace copc::io
#endif // COPCLIB_IO_WRITER_H_