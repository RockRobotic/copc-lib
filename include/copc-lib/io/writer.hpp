#ifndef COPCLIB_IO_WRITER_H_
#define COPCLIB_IO_WRITER_H_

#include <array>
#include <ostream>
#include <stdexcept>

#include <copc-lib/copc/file.hpp>
#include <copc-lib/io/base_io.hpp>
#include <copc-lib/io/internal/writer_internal.hpp>
#include <copc-lib/las/header.hpp>
#include <copc-lib/las/points.hpp>

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
};

// Provides the public interface for writing COPC files
class Writer : public BaseIO
{
  public:
    // Header config for creating a new file
    struct LasConfig
    {
        // Not sure we should allow default constructor
        // LasConfig()= default;
        LasConfig(const int8_t &point_format_id, const vector3 &scale = {0.01, 0.01, 0.01},
                  const vector3 &offset = {0, 0, 0})
            : point_format_id(point_format_id), scale(scale), offset(offset){};
        // Allow for "copying" a lasheader from one file to another
        LasConfig(const las::LasHeader &config, const las::EbVlr &extra_bytes_);

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

        // default to 0
        int8_t point_format_id{};

        las::EbVlr extra_bytes;

        // xyz scale/offset
        vector3 scale{0.01, 0.01, 0.01};
        vector3 offset{0, 0, 0};
        // xyz min/max for las header
        vector3 max{0, 0, 0};
        vector3 min{0, 0, 0};

        // # of points per return 0-14
        uint64_t points_by_return_14[15]{};
        //    private:
        //        // Getter/Setters for python bindings
        //        void CreationDay(const uint16_t& day){
        //            creation.day = day;
        //        }
        //        uint16_t CreationDay() const{
        //            return creation.day;
        //        }
        //        void CreationYear(const uint16_t& year){
        //            creation.year = year;
        //        }
        //        uint16_t CreationYear() const{
        //            return creation.year;
        //        }
        //        void Scale(const std::vector<double> &scale){
        //            this->scale.x = scale[0];
        //            this->scale.y = scale[1];
        //            this->scale.z = scale[2];
        //        }
        //        std::vector<double> Scale() const{
        //            return std::vector<double>(){scale.x,scale.y,scale.z};
        //        }
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
