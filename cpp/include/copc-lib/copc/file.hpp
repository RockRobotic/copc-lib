#ifndef COPCLIB_COPC_FILE_H_
#define COPCLIB_COPC_FILE_H_

#include <iostream>
#include <stdexcept>
#include <vector>

#include "copc-lib/copc/extents.hpp"
#include "copc-lib/las/file.hpp"
#include "copc-lib/las/utils.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc
{
const int COPC_OFFSET = 429;

class CopcFile : public las::LasFile
{
  public:
    CopcFile(const las::LasHeader &header, const las::CopcInfoVlr &copc_info, const CopcExtents &copc_extents,
             const las::WktVlr &wkt, const las::EbVlr &eb)
        : LasFile(header, eb), copc_info_(copc_info), copc_extents_(copc_extents), wkt_(wkt){};
    CopcFile(const las::LasHeader &header, double spacing, const std::string &wkt, const las::EbVlr &eb)
        : LasFile(header, eb), copc_extents_(header.point_format_id, eb.items.size())
    {
        this->wkt_.wkt = wkt;
        this->copc_info_.spacing = spacing;
    };

    // WKT string if defined, else empty
    std::string GetWkt() const { return wkt_.wkt; }

    // CopcInfo
    las::CopcInfoVlr GetCopcInfoVlr() const { return copc_info_; }

    // CopcExtents
    CopcExtents GetCopcExtents() const { return copc_extents_; }

    void SetCopcExtents(const CopcExtents &extents) { copc_extents_ = extents; }

  private:
    las::WktVlr wkt_;
    las::CopcInfoVlr copc_info_;
    CopcExtents copc_extents_;
};
} // namespace copc
#endif // COPCLIB_COPC_FILE_H_
