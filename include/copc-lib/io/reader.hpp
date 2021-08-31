#ifndef COPCLIB_IO_READER_H_
#define COPCLIB_IO_READER_H_

#include <istream>

#include <copc-lib/copc/file.hpp>
#include <copc-lib/las/vlr.hpp>

namespace copc::io
{
class Reader
{
  public:
    Reader(std::istream &in_stream);

    std::shared_ptr<CopcFile> file;
    std::istream &in_stream;

    las::CopcVlr copc_data;

  private:
    std::unique_ptr<lazperf::reader::generic_file> reader_;
    std::map<uint64_t, las::VlrHeader> vlrs_; // maps from absolute offsets to VLR entries

    void ReadVlrs();

    las::CopcVlr GetCopcData();
    las::WktVlr GetWktData();
};
} // namespace copc::io
#endif // COPCLIB_IO_READER_H_