#ifndef COPCLIB_COPC_CONFIG_H_
#define COPCLIB_COPC_CONFIG_H_

#include <memory>
#include <string>

#include "copc-lib/copc/extents.hpp"
#include "copc-lib/copc/info.hpp"
#include "copc-lib/las/header.hpp"
#include "copc-lib/las/utils.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc
{
const int COPC_OFFSET = 429;

class CopcConfig
{
  public:
    CopcConfig() = default;
    CopcConfig(const las::LasHeader &header, const CopcInfo &copc_info, const CopcExtents &copc_extents,
               const std::string &wkt, const las::EbVlr &extra_bytes_vlr)
        : header_(std::make_shared<las::LasHeader>(header)), copc_info_(std::make_shared<copc::CopcInfo>(copc_info)),
          copc_extents_(std::make_shared<copc::CopcExtents>(copc_extents)), wkt_(wkt),
          eb_vlr_(std::make_shared<las::EbVlr>(extra_bytes_vlr)){};

    virtual las::LasHeader LasHeader() const { return *header_; }

    virtual copc::CopcInfo CopcInfo() const { return *copc_info_; }

    virtual copc::CopcExtents CopcExtents() const { return *copc_extents_; }

    std::string Wkt() const { return wkt_; }

    las::EbVlr ExtraBytesVlr() const { return *eb_vlr_; }

  protected:
    CopcConfig(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset, const std::string &wkt,
               const las::EbVlr &extra_bytes_vlr, bool has_extended_stats);

    std::shared_ptr<las::LasHeader> header_;
    std::shared_ptr<copc::CopcInfo> copc_info_;
    std::shared_ptr<copc::CopcExtents> copc_extents_;
    std::string wkt_;
    std::shared_ptr<las::EbVlr> eb_vlr_;
};

class CopcConfigWriter : public CopcConfig
{
  public:
    CopcConfigWriter(const int8_t &point_format_id, const Vector3 &scale = Vector3::DefaultScale(),
                     const Vector3 &offset = Vector3::DefaultOffset(), const std::string &wkt = "",
                     const las::EbVlr &extra_bytes_vlr = las::EbVlr(0), bool has_extended_stats = false);

    // Copy constructor
    CopcConfigWriter(const CopcConfigWriter &copc_config)
        : CopcConfig(copc_config.LasHeader(), copc_config.CopcInfo(), copc_config.CopcExtents(), copc_config.Wkt(),
                     copc_config.ExtraBytesVlr())
    {
    }

    // Allow copy from CopcFile
    CopcConfigWriter(const CopcConfig &copc_config)
        : CopcConfig(copc_config.LasHeader(), copc_config.CopcInfo(), copc_config.CopcExtents(), copc_config.Wkt(),
                     copc_config.ExtraBytesVlr())
    {
    }

    std::shared_ptr<las::LasHeader> LasHeader() { return header_; }
    las::LasHeader LasHeader() const override { return *header_; }

    std::shared_ptr<copc::CopcInfo> CopcInfo() { return copc_info_; }
    copc::CopcInfo CopcInfo() const override { return *copc_info_; }

    std::shared_ptr<copc::CopcExtents> CopcExtents() { return copc_extents_; }
    copc::CopcExtents CopcExtents() const override { return *copc_extents_; }
};

} // namespace copc
#endif // COPCLIB_COPC_CONFIG_H_
