#ifndef COPCLIB_COPC_FILE_H_
#define COPCLIB_COPC_FILE_H_

#include <iostream>
#include <stdexcept>
#include <vector>

#include "copc-lib/copc/extents.hpp"
#include "copc-lib/copc/info.hpp"
#include "copc-lib/las/file.hpp"
#include "copc-lib/las/utils.hpp"

namespace copc
{
const int COPC_OFFSET = 429;

class CopcFile : public las::LasFile
{
  public:
    CopcFile(const las::LasHeader &header, const CopcInfo &copc_info, const CopcExtents &copc_extents,
             const std::string &wkt, const las::EbVlr &extra_bytes_vlr)
        : LasFile(header, extra_bytes_vlr), copc_info_(copc_info), copc_extents_(copc_extents), wkt_(wkt){};

    // WKT string if defined, else empty
    std::string GetWkt() const { return wkt_; }

    // CopcInfo
    CopcInfo GetCopcInfo() const { return copc_info_; }

    // CopcExtents
    CopcExtents GetCopcExtents() const { return copc_extents_; }

    void SetCopcInfo(const CopcInfo &info) { copc_info_ = info; }
    void SetCopcHier(uint64_t root_hier_offset, uint64_t root_hier_size)
    {
        copc_info_.root_hier_offset = root_hier_offset;
        copc_info_.root_hier_size = root_hier_size;
    }
    void SetCopcExtents(const CopcExtents &extents) { copc_extents_ = extents; }

  private:
    std::string wkt_;
    CopcInfo copc_info_;
    CopcExtents copc_extents_;
};
} // namespace copc
#endif // COPCLIB_COPC_FILE_H_
