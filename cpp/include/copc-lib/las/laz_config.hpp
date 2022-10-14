#ifndef COPCLIB_LAZ_CONFIG_H_
#define COPCLIB_LAZ_CONFIG_H_

#include <memory>
#include <string>
#include <utility>

#include "copc-lib/las/header.hpp"
#include "copc-lib/las/utils.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc
{
class CopcConfig;

namespace las
{
class LazConfigWriter;
class LazConfig
{
  public:
    LazConfig() = default;
    LazConfig(const las::LasHeader &header, std::string wkt, const las::EbVlr &extra_bytes_vlr) : wkt_(std::move(wkt))
    {
        header_ = std::make_shared<las::LasHeader>(header);
        eb_vlr_ = std::make_shared<las::EbVlr>(extra_bytes_vlr);
    }

    virtual las::LasHeader LasHeader() const { return *header_; }

    std::string Wkt() const { return wkt_; }

    las::EbVlr ExtraBytesVlr() const { return *eb_vlr_; }

    // Copy constructor from CopcConfig
    LazConfig(const CopcConfig &copc_config);

  protected:
    // Copy constructor from LazConfigWriter
    LazConfig(const LazConfigWriter &laz_config_writer);

    LazConfig(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset, std::string wkt,
              const las::EbVlr &extra_bytes_vlr, bool copc_flag);
    std::shared_ptr<las::LasHeader> header_;
    std::string wkt_;
    std::shared_ptr<las::EbVlr> eb_vlr_;
};

class LazConfigWriter : public LazConfig
{
  public:
    LazConfigWriter(const int8_t &point_format_id, const Vector3 &scale = Vector3::DefaultScale(),
                    const Vector3 &offset = Vector3::DefaultOffset(), const std::string &wkt = "",
                    const las::EbVlr &extra_bytes_vlr = las::EbVlr(0));

    LazConfigWriter(const LazConfig &laz_config)
        : LazConfig(las::LasHeader(laz_config.LasHeader(), false), laz_config.Wkt(), laz_config.ExtraBytesVlr())
    {
    }

    std::shared_ptr<las::LasHeader> LasHeader() { return header_; }
    las::LasHeader LasHeader() const override { return *header_; }
};

} // namespace las
} // namespace copc
#endif // COPCLIB_LAZ_CONFIG_H_
