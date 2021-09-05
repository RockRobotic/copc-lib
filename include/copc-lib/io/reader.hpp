#ifndef COPCLIB_IO_READER_H_
#define COPCLIB_IO_READER_H_

#include <istream>

#include <copc-lib/copc/file.hpp>
#include <copc-lib/hierarchy/internal/hierarchy.hpp>
#include <copc-lib/hierarchy/node.hpp>
#include <copc-lib/hierarchy/page.hpp>
#include <copc-lib/io/base_io.hpp>
#include <copc-lib/las/vlr.hpp>

namespace copc
{
class Reader : public BaseIO
{
  public:
    Reader(std::istream &in_stream);

    // Reads the node's data into an uncompressed byte array
    std::vector<char> GetPointData(Node const &node);
    // Reads the node's data into Point objects
    std::vector<las::Point> GetPoints(Node const &node);
    // Reads node data without decompressing
    std::vector<char> GetPointDataCompressed(Node const &node);

  private:
    std::istream &in_stream;

    std::unique_ptr<lazperf::reader::generic_file> reader_;

    // Constructor helper function, initializes the file and hierarchy
    void InitFile();
    // Reads file VLRs into vlrs_
    std::map<uint64_t, las::VlrHeader> ReadVlrs();
    // Finds and loads the COPC vlr
    las::CopcVlr ReadCopcData();
    // Finds and loads the WKT vlr
    las::WktVlr ReadWktData(las::CopcVlr copc_data);
    // finds and loads EB vlr
    las::EbVlr ReadExtraByteVlr(std::map<uint64_t, las::VlrHeader> &vlrs);

    std::vector<Entry> ReadPage(std::shared_ptr<Internal::PageInternal> page) override;
};
} // namespace copc
#endif // COPCLIB_IO_READER_H_
