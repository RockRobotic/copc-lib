#ifndef COPCLIB_LAS_FILE_H_
#define COPCLIB_LAS_FILE_H_

#include <istream>
#include <map>

#include <copc-lib/las/vlr.hpp>

#include <lazperf/readers.hpp>

namespace copc::las
{
using VlrHeader = lazperf::vlr_header;
// todo: create our own lasheader class so we can switch between header12/14
using LasHeader = lazperf::header12;

class LasFile
{
  public:
    LasFile(std::istream &in_stream);

    std::vector<Vlr> vlrs;
    LasHeader GetLasHeader() { return header_; }

  protected:
    std::unique_ptr<lazperf::reader::generic_file> reader_;
    std::istream &in_stream_;
    std::map<uint64_t, VlrHeader> vlrs_; // maps from absolute offsets to VLR entries
    LasHeader header_;

    void ReadVlrs();
};
} // namespace copc::las
#endif // COPCLIB_LAS_FILE_H_
