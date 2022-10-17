#ifndef COPCLIB_IO_BASE_READER_H_
#define COPCLIB_IO_BASE_READER_H_

#include "copc-lib/las/vlr.hpp"

#include <istream>
#include <limits>
#include <map>
#include <string>

#include "copc-lib/copc/copc_config.hpp"
#include "copc-lib/hierarchy/key.hpp"
#include "copc-lib/las/points.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc
{
class BaseReader
{
  public:
    BaseReader(std::istream *in_stream) : in_stream_(in_stream) { InitReader(); }

  protected:
    BaseReader() = default;

    las::LazConfig las_config_;
    std::map<uint64_t, las::VlrHeader> vlrs_; // maps from absolute offsets to VLR entries

    std::istream *in_stream_;

    std::unique_ptr<lazperf::reader::generic_file> reader_;

    // Constructor helper function, initializes the file and hierarchy
    void InitReader();
    // Reads file VLRs and EVLRs into vlrs_
    std::map<uint64_t, las::VlrHeader> ReadVlrHeaders(); // TODO: Allow user to create/reader arbitrary VLRs
    // Fetchs the map key for a query vlr user and record IDs
    static uint64_t FetchVlr(const std::map<uint64_t, las::VlrHeader> &vlrs, const std::string &user_id,
                             uint16_t record_id);

    // Finds and loads the WKT vlr
    las::WktVlr ReadWktVlr(std::map<uint64_t, las::VlrHeader> &vlrs);
    // Finds and loads EB vlr
    las::EbVlr ReadExtraBytesVlr(std::map<uint64_t, las::VlrHeader> &vlrs);
};
} // namespace copc
#endif // COPCLIB_IO_BASE_READER_H_
