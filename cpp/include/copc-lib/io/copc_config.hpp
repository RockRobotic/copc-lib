#ifndef COPCLIB_IO_COPC_CONFIG_H_
#define COPCLIB_IO_COPC_CONFIG_H_

#include "copc-lib/copc/extents.hpp"
#include "copc-lib/copc/info.hpp"
#include "copc-lib/geometry/vector3.hpp"
#include "copc-lib/las/header.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc
{

// COPC Config to create new files
struct CopcConfig
{
    CopcConfig(const int8_t &point_format_id, const Vector3 &scale = Vector3::DefaultScale(),
               const Vector3 &offset = Vector3::DefaultOffset(), const las::EbVlr &extra_bytes_vlr = {0});
    // Allow for "copying" a lasheader from one file to another
    CopcConfig(const las::LasHeader &config, const CopcInfo &copc_info, const CopcExtents &copc_extents,
               const std::string &wkt, const las::EbVlr &extra_bytes);

    std::string ToString() const;

    uint16_t NumExtraBytes() const { return extra_bytes_vlr_.items.size(); }

    int8_t GetPointFormatID() const { return point_format_id_; }
    CopcExtents GetCopcExtents() const { return copc_extents_; }
    las::EbVlr GetExtraBytesVlr() const { return extra_bytes_vlr_; }

    CopcInfo copc_info;
    std::string wkt;
    las::LasHeaderBase las_header_base;

  private:
    int8_t point_format_id_;
    CopcExtents copc_extents_;
    las::EbVlr extra_bytes_vlr_;
};

} // namespace copc
#endif // COPCLIB_IO_COPC_CONFIG_H_
