#ifndef COPCLIB_COPC_FILE_H_
#define COPCLIB_COPC_FILE_H_

#include <iostream>

#include <copc-lib/las/file.hpp>
#include <copc-lib/las/vlr.hpp>

namespace copc
{
const int COPC_OFFSET = 429;

class CopcFile : public las::LasFile
{
  public:
    CopcFile(las::LasHeader header, int span = 0, std::string wkt = "");

    // WKT string if defined, else empty
    std::string GetWkt() const { return wkt.wkt; }

    // CopcData
    int64_t GetSpan() const { return copc.span; }

  private:
    las::WktVlr wkt;
    las::CopcVlr copc;
};
} // namespace copc
#endif // COPCLIB_COPC_FILE_H_