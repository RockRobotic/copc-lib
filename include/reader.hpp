#ifndef COPCLIB_IO_READER_H_
#define COPCLIB_IO_READER_H_

#include <fstream>
#include <map>
#include <string>

#include "structs.hpp"
#include <lazperf/filestream.hpp>
#include <lazperf/readers.hpp>

using namespace lazperf;
using namespace std;

namespace copc
{

namespace io
{

class CopcReader
{
  public:
    CopcReader(istream &in_stream);

    LasHeader GetHeader() { return reader_->header(); }
    uint64_t GetPointCount() { return reader_->pointCount(); }
    CopcData GetCopcHeader() { return copc_data_; }
    std::string GetWkt()
    {
        if (!wkt_loaded_)
            ReadWkt();
        return wkt_data_;
    }

    std::vector<Entry> ReadPage(const int64_t &offset, const uint64_t &pageSize);
    // Given an entry, decompress all the points in entry to out buf
    std::vector<char> GetPoints(Entry entry);

  private:
    std::unique_ptr<lazperf::reader::generic_file> reader_;
    istream &stream_;
    std::map<uint64_t, VlrHeader> vlrs_; // maps from absolute offsets to VLR entries

    CopcData copc_data_;
    std::string wkt_data_;
    bool wkt_loaded_ = false;

    Entry ReadEntry();
    void ReadVlrs();
    void ReadCopcVlr();
    void ReadWkt();
};

} // namespace io

} // namespace copc
#endif // COPCLIB_IO_READER_H_