#ifndef COPCLIB_IO_COPC_WRITER_H_
#define COPCLIB_IO_COPC_WRITER_H_

#include <array>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>

#include "copc-lib/copc/copc_config.hpp"
#include "copc-lib/geometry/box.hpp"
#include "copc-lib/io/copc_base_io.hpp"
#include "copc-lib/io/laz_base_writer.hpp"
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
    Node AddNode(const VoxelKey &key, const las::Points &points, const VoxelKey &page_key = VoxelKey::RootKey());
    Node AddNodeCompressed(const VoxelKey &key, std::vector<char> const &compressed_data, int32_t point_count,
                           const VoxelKey &page_key = VoxelKey::RootKey());
    Node AddNode(const VoxelKey &key, std::vector<char> const &uncompressed_data,
                 const VoxelKey &page_key = VoxelKey::RootKey());

    void ChangeNodePage(const VoxelKey &node_key, const VoxelKey &new_page_key);

    std::shared_ptr<CopcConfigWriter> CopcConfig() { return config_; }

    ~Writer() { Close(); }

  protected:
    Writer() = default;

    std::shared_ptr<CopcConfigWriter> config_;

    std::shared_ptr<Internal::WriterInternal> writer_;

    bool PageExists(const VoxelKey &key);

    Node DoAddNode(const VoxelKey &key, const std::vector<char> &in, int32_t point_count, bool compressed_data,
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
};

class FileWriter : public Writer, laz::BaseFileWriter
{
  public:
    FileWriter(const std::string &file_path, const CopcConfigWriter &copc_config_writer,
               const std::optional<int8_t> &point_format_id = {}, const std::optional<Vector3> &scale = {},
               const std::optional<Vector3> &offset = {}, const std::optional<std::string> &wkt = {},
               const std::optional<las::EbVlr> &extra_bytes_vlr = {},
               const std::optional<bool> &has_extended_stats = {})
        : BaseFileWriter(file_path)
    {
        InitWriter(f_stream_, copc_config_writer, point_format_id, scale, offset, wkt, extra_bytes_vlr,
                   has_extended_stats);
    }

    void Close() override;
    ~FileWriter() { Close(); }

    std::string FilePath() { return file_path_; }
};

} // namespace copc
#endif // COPCLIB_IO_COPC_WRITER_H_
