#ifndef COPCLIB_IO_WRITER_H_
#define COPCLIB_IO_WRITER_H_

#include <array>
#include <ostream>
#include <stdexcept>
#include <string>

#include <copc-lib/copc/file.hpp>
#include <copc-lib/io/base_io.hpp>
#include <copc-lib/io/internal/writer_internal.hpp>
#include <copc-lib/las/header.hpp>
#include <copc-lib/las/points.hpp>
#include <copc-lib/utils.hpp>

namespace copc
{

// Provides the public interface for writing COPC files
class Writer : public BaseIO
{
  public:
    // Header config for creating a new file
    struct LasConfig
    {
        // Not sure we should allow default constructor
        // LasConfig()= default;
        LasConfig(const int8_t &point_format_id, const Vector3 &scale = {0.01, 0.01, 0.01},
                  const Vector3 &offset = {0, 0, 0})
            : point_format_id(point_format_id), scale(scale), offset(offset){};
        // Allow for "copying" a lasheader from one file to another
        LasConfig(const las::LasHeader &config, const las::EbVlr &extra_bytes_);

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

        uint16_t creation_day{};
        uint16_t creation_year{};

        // default to 0
        int8_t point_format_id{};

        las::EbVlr extra_bytes;

        // xyz scale/offset
        Vector3 scale{0.01, 0.01, 0.01};
        Vector3 offset{0, 0, 0};
        // xyz min/max for las header
        Vector3 max{0, 0, 0};
        Vector3 min{0, 0, 0};

        // # of points per return 0-14
        std::array<uint64_t, 15> points_by_return_14{};

      private:
        std::string guid_{};
        std::string system_identifier_{};
        std::string generating_software_{};
    };

    Writer(std::ostream &out_stream, LasConfig const &config, int span = 0, const std::string &wkt = "")
    {
        InitWriter(out_stream, config, span, wkt);
    }

    Page GetRootPage() { return *this->hierarchy_->seen_pages_[VoxelKey::BaseKey()]; }

    // Writes the file out
    virtual void Close() { writer_->Close(); }

    // Adds a node to a given page
    Node AddNode(Page &page, const VoxelKey &key, las::Points &points); // TODO[Leo]: Add this to tests
    Node AddNodeCompressed(Page &page, const VoxelKey &key, std::vector<char> const &compressed,
                           uint64_t point_count); // TODO[Leo]: Add this to tests
    Node AddNode(Page &page, const VoxelKey &key, std::vector<char> const &uncompressed);

    // Adds a subpage to a given page
    Page AddSubPage(Page &page, VoxelKey key);

    virtual ~Writer() { writer_->Close(); }

  protected:
    Writer() = default;

    std::unique_ptr<Internal::WriterInternal> writer_;

    Node DoAddNode(Page &page, VoxelKey key, std::vector<char> in, uint64_t point_count, bool compressed);
    std::vector<Entry> ReadPage(std::shared_ptr<Internal::PageInternal> page) override
    {
        throw std::runtime_error("No pages should be unloaded!");
    };

    // Constructor helper function, initializes the file and hierarchy
    void InitWriter(std::ostream &out_stream, LasConfig const &config, const int &span, const std::string &wkt);
    // Converts the lasconfig object into an actual LasHeader
    static las::LasHeader HeaderFromConfig(LasConfig const &config);
    // Gets the sum of the byte size the extra bytes will take up, for calculating point_record_len
    static int NumBytesFromExtraBytes(const std::vector<las::EbVlr::ebfield> &items);
};

class FileWriter : public Writer
{
  public:
    FileWriter(const std::string &file_path, LasConfig const &config, const int &span = 0, const std::string &wkt = "")
    {
        f_stream_.open(file_path.c_str(), std::ios::out | std::ios::binary);
        InitWriter(f_stream_, config, span, wkt);
    }

    void Close() override
    {
        writer_->Close();
        f_stream_.close();
    }

    ~FileWriter() override { f_stream_.close(); }

  private:
    std::fstream f_stream_;
};

} // namespace copc
#endif // COPCLIB_IO_WRITER_H_
