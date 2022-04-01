#ifndef COPCLIB_LAZ_CONFIG_H_
#define COPCLIB_LAZ_CONFIG_H_

#include <memory>
#include <string>

#include "copc-lib/las/header.hpp"
#include "copc-lib/las/utils.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc::las
{
const int LAZ_OFFSET = 375;

class LazConfig
{
  public:
    LazConfig() = default;
    LazConfig(const las::LasHeader &header, const std::string &wkt, const las::EbVlr &extra_bytes_vlr)
        : header_(std::make_shared<las::LasHeader>(header)), wkt_(wkt),
          eb_vlr_(std::make_shared<las::EbVlr>(extra_bytes_vlr))
    {
    }

    virtual las::LasHeader LasHeader() const { return *header_; }

    std::string Wkt() const { return wkt_; }

    las::EbVlr ExtraBytesVlr() const { return *eb_vlr_; }

  protected:
    LazConfig(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset, const std::string &wkt,
              const las::EbVlr &extra_bytes_vlr);
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

    std::shared_ptr<las::LasHeader> LasHeader() { return header_; }
    las::LasHeader LasHeader() const override { return *header_; }
};

} // namespace copc::las
#endif // COPCLIB_LAZ_CONFIG_H_
