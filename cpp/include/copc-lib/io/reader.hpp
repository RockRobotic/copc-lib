#ifndef COPCLIB_IO_READER_H_
#define COPCLIB_IO_READER_H_

#include <istream>
#include <string>

#include "copc-lib/copc/file.hpp"
#include "copc-lib/hierarchy/node.hpp"
#include "copc-lib/hierarchy/page.hpp"
#include "copc-lib/io/base_io.hpp"
#include "copc-lib/las/points.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc
{
namespace Internal
{
class PageInternal;
} // namespace Internal

class Reader : public BaseIO
{
  public:
    Reader(std::istream *in_stream) : in_stream_(in_stream) { InitReader(); }

    // Reads the node's data into an uncompressed byte array
    // Node needs to be valid for this function, it will error
    std::vector<char> GetPointData(Node const &node);
    // VoxelKey can be invalid, function will return empty arr
    std::vector<char> GetPointData(VoxelKey const &key);
    // Reads the node's data into Point objects
    las::Points GetPoints(Node const &node);
    las::Points GetPoints(VoxelKey const &key);
    // Reads node data without decompressing
    std::vector<char> GetPointDataCompressed(Node const &node);
    std::vector<char> GetPointDataCompressed(VoxelKey const &key);

    // Return all children of a page with a given key
    // (or the node itself, if it exists, if there isn't a page with that key)
    std::vector<Node> GetAllChildren(const VoxelKey &key);
    // Helper function to get all nodes from the root
    std::vector<Node> GetAllChildren() { return GetAllChildren(VoxelKey::BaseKey()); }

  protected:
    Reader() = default;

    std::istream *in_stream_;

    std::unique_ptr<lazperf::reader::generic_file> reader_;

    // Constructor helper function, initializes the file and hierarchy
    void InitReader();
    // Reads file VLRs into vlrs_
    std::map<uint64_t, las::VlrHeader> ReadVlrs();
    // Finds and loads the COPC vlr
    las::CopcVlr ReadCopcData();
    // Finds and loads the WKT vlr
    las::WktVlr ReadWktData(const las::CopcVlr &copc_data);
    // finds and loads EB vlr
    las::EbVlr ReadExtraByteVlr(std::map<uint64_t, las::VlrHeader> &vlrs);

    std::vector<Entry> ReadPage(std::shared_ptr<Internal::PageInternal> page) override;
};

class FileReader : public Reader
{
  public:
    FileReader(const std::string &file_path) : is_open_(true)
    {
        auto f_stream = new std::fstream;
        f_stream->open(file_path.c_str(), std::ios::in | std::ios::binary);
        in_stream_ = f_stream;

        InitReader();
    }

    void Close()
    {
        if (is_open_)
        {
            dynamic_cast<std::fstream *>(in_stream_)->close();
            delete in_stream_;
            is_open_ = false;
        }
    }

    ~FileReader() { Close(); }

  private:
    bool is_open_;
};

} // namespace copc
#endif // COPCLIB_IO_READER_H_
