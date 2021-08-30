#ifndef COPCLIB_COPC_FILE_H_
#define COPCLIB_COPC_FILE_H_

#include <istream>

#include <copc-lib/las/file.hpp>
#include <copc-lib/las/vlr.hpp>

namespace copc
{
const int COPC_OFFSET = 429;

class CopcFile : public las::LasFile
{
  public:
    // CopcFile(std::string wkt);
    CopcFile(std::istream &in_stream);

    // WKT string if defined, else empty
    std::string GetWkt() const { return wkt.wkt; }
    void SetWkt(std::string wkt);

    // CopcData
    las::CopcVlr GetCopcHeader() const { return copc; }

  private:
    las::WktVlr wkt;
    las::CopcVlr copc;

    las::CopcVlr GetCopcData();
    las::WktVlr GetWktData();
};
} // namespace copc
#endif // COPCLIB_COPC_FILE_H_