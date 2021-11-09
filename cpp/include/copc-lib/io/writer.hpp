#ifndef COPCLIB_IO_WRITER_H_
#define COPCLIB_IO_WRITER_H_

#include <array>
#include <ostream>
#include <stdexcept>
#include <string>

#include "copc-lib/copc/config.hpp"
#include "copc-lib/geometry/box.hpp"
#include "copc-lib/io/base_io.hpp"
#include "copc-lib/las/header.hpp"
#include "copc-lib/las/points.hpp"
#include "copc-lib/las/utils.hpp"

namespace copc
{
namespace Internal
{
class WriterInternal;
}

// Provides the public interface for writing COPC files
class Writer : public BaseIO
{
  public:
    Writer(std::ostream &out_stream, CopcConfigWriter const &copc_file_writer)
    {
        InitWriter(out_stream, copc_file_writer);
    }
    Page GetPage(const VoxelKey &key);
    Page GetRootPage();

    // Writes the file out
    virtual void Close();

    // Adds a node to a given page
    Node AddNode(Page &page, const VoxelKey &key, las::Points &points);
    Node AddNodeCompressed(Page &page, const VoxelKey &key, std::vector<char> const &compressed, uint64_t point_count);
    Node AddNode(Page &page, const VoxelKey &key, std::vector<char> const &uncompressed);

    // Adds a subpage to a given page
    Page AddSubPage(Page &page, VoxelKey key);

    std::shared_ptr<CopcConfigWriter> CopcConfig() { return config_; }

    virtual ~Writer();

  protected:
    Writer() = default;

    std::shared_ptr<CopcConfigWriter> config_;

    std::shared_ptr<Internal::WriterInternal> writer_;

    Node DoAddNode(Page &page, VoxelKey key, std::vector<char> in, uint64_t point_count, bool compressed);
    std::vector<Entry> ReadPage(std::shared_ptr<Internal::PageInternal> page) override
    {
        throw std::runtime_error("No pages should be unloaded!");
    };

    // Constructor helper function, initializes the file and hierarchy
    void InitWriter(std::ostream &out_stream, const CopcConfigWriter &copc_file_writer);
    // Gets the sum of the byte size the extra bytes will take up, for calculating point_record_len
    static int NumBytesFromExtraBytes(const std::vector<las::EbVlr::ebfield> &items);
};

class FileWriter : public Writer
{
  public:
    FileWriter(const std::string &file_path, const CopcConfigWriter &copc_file_writer)
    {

        f_stream_.open(file_path.c_str(), std::ios::out | std::ios::binary);
        if (!f_stream_.good())
            throw std::runtime_error("FileWriter: Error while opening file path.");
        InitWriter(f_stream_, copc_file_writer);
    }

    void Close() override;

    ~FileWriter() override { f_stream_.close(); }

  private:
    std::fstream f_stream_;
};

} // namespace copc
#endif // COPCLIB_IO_WRITER_H_
