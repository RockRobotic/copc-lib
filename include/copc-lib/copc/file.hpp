#ifndef COPCLIB_COPC_FILE_H_
#define COPCLIB_COPC_FILE_H_

#include <iostream>
#include <stdexcept>

#include <copc-lib/las/file.hpp>
#include <copc-lib/las/utils.hpp>
#include <copc-lib/las/vlr.hpp>

namespace copc
{
const int COPC_OFFSET = 429;

class CopcFile : public las::LasFile
{
  public:
    CopcFile(const las::LasHeader &header, const las::CopcInfoVlr &copc_info, const las::CopcExtentsVlr &copc_extents,
             const las::WktVlr &wkt, const las::EbVlr &eb)
        : LasFile(header, eb), copc_info_(copc_info), copc_extents_(copc_extents), wkt_(wkt){};
    CopcFile(const las::LasHeader &header, int span, const std::string &wkt, const las::EbVlr &eb) : LasFile(header, eb)
    {
        this->wkt_.wkt = wkt;
        this->copc_info_.span = span;
    };

    // WKT string if defined, else empty
    std::string GetWkt() const { return wkt_.wkt; }

    // CopcData
    las::CopcInfoVlr GetCopcInfoVlr() const { return copc_info_; }

    // CopcInfo
    las::CopcExtentsVlr GetCopcExtentsVlr() const { return copc_extents_; }

    void SetExtents(const std::vector<las::CopcExtent> &extents)
    {
        // Check that the size of extents matches the point format id and number of EBs
        if (extents.size() != (las::PointBaseNumberDimensions(header_.point_format_id) + eb_.items.size()))
            throw std::runtime_error("Wrong number of extents.");
        copc_extents_.extents = extents;
    }

  private:
    las::WktVlr wkt_;
    las::CopcInfoVlr copc_info_;
    las::CopcExtentsVlr copc_extents_;
};
} // namespace copc
#endif // COPCLIB_COPC_FILE_H_
