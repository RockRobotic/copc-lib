#ifndef COPCLIB_LAS_FILE_H_
#define COPCLIB_LAS_FILE_H_

#include <iostream>
#include <map>

#include <copc-lib/las/vlr.hpp>
#include <copc-lib/las/header.hpp>

#include <lazperf/readers.hpp>

namespace copc::las
{
class LasFile
{
  public:
    LasFile(std::istream *in_stream);
    LasFile(LasHeader header);

    std::vector<Vlr> vlrs;
    LasHeader GetLasHeader() const { return header_; }

  protected:
    std::unique_ptr<lazperf::reader::generic_file> reader_;
    std::istream *in_stream_;
    std::map<uint64_t, VlrHeader> vlrs_; // maps from absolute offsets to VLR entries
    LasHeader header_;

    void ReadVlrs();
};
} // namespace copc::las
#endif // COPCLIB_LAS_FILE_H_