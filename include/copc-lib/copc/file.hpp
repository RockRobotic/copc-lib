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
    CopcFile(las::LasHeader header, las::CopcVlr copc, las::WktVlr wkt, las::EbVlr eb)
        : LasFile(header, eb), copc(copc), wkt(wkt){};
    CopcFile(las::LasHeader header, int span, std::string wkt, las::EbVlr eb) : LasFile(header, eb)
    {
        this->wkt.wkt = wkt;
        this->copc.span = span;
    };

    // WKT string if defined, else empty
    std::string GetWkt() const { return wkt.wkt; }

    // CopcData
    las::CopcVlr GetCopc() const { return copc; }

  private:
    las::WktVlr wkt;
    las::CopcVlr copc;
};
} // namespace copc
#endif // COPCLIB_COPC_FILE_H_
