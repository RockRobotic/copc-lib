#ifndef COPCLIB_IO_WRITER_H_
#define COPCLIB_IO_WRITER_H_

#include <ostream>

#include <copc-lib/copc/file.hpp>
#include <copc-lib/io/base_io.hpp>
#include <copc-lib/io/internal/writer_internal.hpp>
#include <copc-lib/las/header.hpp>

namespace copc
{

struct vector3
{
    vector3() : x(0), y(0), z(0) {}
    vector3(double x, double y, double z) : x(x), y(y), z(z) {}

    double x;
    double y;
    double z;
};

// Provides the public interface for writing COPC files
class Writer : public BaseIO
{
  public:
    // Header config for creating a new file
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
        vector3 max{0, 0, 0};
        vector3 min{0, 0, 0};
    };

    Writer(std::ostream &out_stream, LasConfig const &config, int span = 0, std::string wkt = "");

    Page GetRootPage() { return *hierarchy->seen_pages_[VoxelKey::BaseKey()]; }

    // Writes the file out
    void Close();

    // Adds a node to a given page
    Node AddNode(Page &page, VoxelKey key, std::vector<las::Point> const &points);
    Node AddNodeCompressed(Page &page, VoxelKey key, std::vector<char> const &compressed, uint64_t point_count);
    Node AddNode(Page &page, VoxelKey key, std::vector<char> const &uncompressed);

    // Adds a subpage to a given page
    Page AddSubPage(Page &page, VoxelKey key);

  private:
    std::unique_ptr<Internal::WriterInternal> writer_;

    Node DoAddNode(Page &page, VoxelKey key, std::vector<char> in, uint64_t point_count, bool compressed);
    std::vector<Entry> ReadPage(std::shared_ptr<Internal::PageInternal> page) override
    {
        throw std::runtime_error("No pages should be unloaded!");
    };

    // Converts the lasconfig object into an actual LasHeader
    las::LasHeader HeaderFromConfig(LasConfig const &config);
};
} // namespace copc
#endif // COPCLIB_IO_WRITER_H_
