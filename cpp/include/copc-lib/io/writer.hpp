#ifndef COPCLIB_IO_WRITER_H_
#define COPCLIB_IO_WRITER_H_

#include <array>
#include <optional>
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
    Writer(std::ostream &out_stream, const CopcConfigWriter &copc_config_writer,
           const std::optional<int8_t> &point_format_id = {}, const std::optional<Vector3> &scale = {},
           const std::optional<Vector3> &offset = {}, const std::optional<std::string> &wkt = {},
           const std::optional<las::EbVlr> &extra_bytes_vlr = {}, const std::optional<bool> &has_extended_stats = {})
    {
        InitWriter(out_stream, copc_config_writer, point_format_id, scale, offset, wkt, extra_bytes_vlr,
                   has_extended_stats);
    }

    // Writes the file out
    virtual void Close();

    // Adds a node to a given page
    Node AddNode(const VoxelKey &key, las::Points &points, const VoxelKey &page_key = VoxelKey::RootKey());
    Node AddNodeCompressed(const VoxelKey &key, std::vector<char> const &compressed_data, uint64_t point_count,
                           const VoxelKey &page_key = VoxelKey::RootKey());
    Node AddNode(const VoxelKey &key, std::vector<char> const &uncompressed_data,
                 const VoxelKey &page_key = VoxelKey::RootKey());

    void ChangeNodePage(const VoxelKey &node_key, const VoxelKey &new_page_key);

    std::shared_ptr<CopcConfigWriter> CopcConfig() { return config_; }

    virtual ~Writer();

  protected:
    Writer() = default;

    std::shared_ptr<CopcConfigWriter> config_;

    std::shared_ptr<Internal::WriterInternal> writer_;

    bool PageExists(const VoxelKey &key);

    Node DoAddNode(const VoxelKey &key, std::vector<char> in, uint64_t point_count, bool compressed_data,
                   const VoxelKey &page_key);
    std::vector<Entry> ReadPage(std::shared_ptr<Internal::PageInternal> page) override
    {
        throw std::runtime_error("No pages should be unloaded!");
    };

    // Constructor helper function, initializes the file and hierarchy
    void InitWriter(std::ostream &out_stream, const CopcConfigWriter &copc_file_writer,
                    const std::optional<int8_t> &point_format_id, const std::optional<Vector3> &scale,
                    const std::optional<Vector3> &offset, const std::optional<std::string> &wkt,
                    const std::optional<las::EbVlr> &extra_bytes_vlr, const std::optional<bool> &has_extended_stats);
    // Gets the sum of the byte size the extra bytes will take up, for calculating point_record_len
    static int NumBytesFromExtraBytes(const std::vector<las::EbVlr::ebfield> &items);
};

class FileWriter : public Writer
{
  public:
    FileWriter(const std::string &file_path, const CopcConfigWriter &copc_file_writer,
               const std::optional<int8_t> &point_format_id = {}, const std::optional<Vector3> &scale = {},
               const std::optional<Vector3> &offset = {}, const std::optional<std::string> &wkt = {},
               const std::optional<las::EbVlr> &extra_bytes_vlr = {},
               const std::optional<bool> &has_extended_stats = {})
    {

        f_stream_.open(file_path.c_str(), std::ios::out | std::ios::binary);
        if (!f_stream_.good())
            throw std::runtime_error("FileWriter: Error while opening file path.");
        InitWriter(f_stream_, copc_file_writer, point_format_id, scale, offset, wkt, extra_bytes_vlr,
                   has_extended_stats);
    }

    void Close() override;

    ~FileWriter() override { f_stream_.close(); }

  private:
    std::fstream f_stream_;
};

} // namespace copc
#endif // COPCLIB_IO_WRITER_H_
