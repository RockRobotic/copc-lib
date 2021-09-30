#ifndef COPCLIB_IO_WRITER_H_
#define COPCLIB_IO_WRITER_H_

#include <array>
#include <ostream>
#include <stdexcept>
#include <string>

#include <copc-lib/copc/extents.hpp>
#include <copc-lib/copc/file.hpp>
#include <copc-lib/geometry/box.hpp>
#include <copc-lib/io/base_io.hpp>
#include <copc-lib/io/internal/writer_internal.hpp>
#include <copc-lib/las/header.hpp>
#include <copc-lib/las/points.hpp>
#include <copc-lib/las/utils.hpp>

namespace copc
{

// Provides the public interface for writing COPC files
class Writer : public BaseIO
{
  public:
    // Header config for creating a new file
    struct LasHeaderConfig : public las::LasHeaderBase
    {
        LasHeaderConfig(const int8_t &point_format_id, const Vector3 &scale = Vector3::DefaultScale(),
                        const Vector3 &offset = Vector3::DefaultOffset())
            : LasHeaderBase(point_format_id, scale, offset)
        {
            if (point_format_id < 6 || point_format_id > 8)
                throw std::runtime_error("LasConfig: Supported point formats are 6 to 8.");
        };
        // Allow for "copying" a lasheader from one file to another
        LasHeaderConfig(const las::LasHeader &config, const las::EbVlr &extra_bytes);

        std::string ToString() const
        {
            std::stringstream ss;
            ss << "LasConfig:" << std::endl;
            ss << "\tFile Source ID: " << file_source_id << std::endl;
            ss << "\tGlobal Encoding ID: " << global_encoding << std::endl;
            ss << "\tGUID: " << GUID() << std::endl;
            ss << "\tSystem Identifier: " << SystemIdentifier() << std::endl;
            ss << "\tGenerating Software: " << GeneratingSoftware() << std::endl;
            ss << "\tCreation (DD/YYYY): (" << creation_day << "/" << creation_year << ")" << std::endl;
            ss << "\tPoint Format ID: " << static_cast<int>(point_format_id) << std::endl;
            ss << "\tScale: " << scale.ToString() << std::endl;
            ss << "\tOffset: " << offset.ToString() << std::endl;
            ss << "\tMax: " << max.ToString() << std::endl;
            ss << "\tMin: " << min.ToString() << std::endl;
            ss << "\tPoints By Return:" << std::endl;
            for (int i = 0; i < points_by_return_14.size(); i++)
                ss << "\t\t [" << i << "]: " << points_by_return_14[i] << std::endl;
            ss << "\tNumber of Extra Bytes: " << NumExtraBytes() << std::endl;
            return ss.str();
        }

        uint16_t NumExtraBytes() const { return extra_bytes.items.size(); }

        las::EbVlr extra_bytes;
    };

    Writer(std::ostream &out_stream, LasHeaderConfig const &config, int span = 0, const std::string &wkt = "")
    {
        InitWriter(out_stream, config, span, wkt);
    }

    Page GetRootPage() { return *this->hierarchy_->seen_pages_[VoxelKey::BaseKey()]; }

    // Writes the file out
    virtual void Close() { writer_->Close(); }

    // Adds a node to a given page
    Node AddNode(Page &page, const VoxelKey &key, las::Points &points);
    Node AddNodeCompressed(Page &page, const VoxelKey &key, std::vector<char> const &compressed, uint64_t point_count);
    Node AddNode(Page &page, const VoxelKey &key, std::vector<char> const &uncompressed);

    // Adds a subpage to a given page
    Page AddSubPage(Page &page, VoxelKey key);

    // Update file internals
    void SetMin(const Vector3 &min) { this->file_->SetMin(min); }
    void SetMax(const Vector3 &max) { this->file_->SetMax(max); }
    void SetPointsByReturn(const std::array<uint64_t, 15> &points_by_return_14)
    {
        this->file_->SetPointsByReturn(points_by_return_14);
    }

    void SetCopcExtents(const CopcExtents &extents) { this->file_->SetCopcExtents(extents); }

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
    void InitWriter(std::ostream &out_stream, LasHeaderConfig const &config, const int &span, const std::string &wkt);
    // Converts the LasHeaderConfig object into an actual LasHeader
    static las::LasHeader HeaderFromConfig(LasHeaderConfig const &config);
    // Gets the sum of the byte size the extra bytes will take up, for calculating point_record_len
    static int NumBytesFromExtraBytes(const std::vector<las::EbVlr::ebfield> &items);
};

class FileWriter : public Writer
{
  public:
    FileWriter(const std::string &file_path, LasHeaderConfig const &config, const int &span = 0,
               const std::string &wkt = "")
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
