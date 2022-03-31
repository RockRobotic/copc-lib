#ifndef COPCLIB_LAZ_CONFIG_H_
#define COPCLIB_LAZ_CONFIG_H_

#include <memory>
#include <string>

#include "copc-lib/copc/extents.hpp"
#include "copc-lib/copc/info.hpp"
#include "copc-lib/las/header.hpp"
#include "copc-lib/las/utils.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc::laz
{

class LasConfig
{
  public:
    LasConfig(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset, const std::string &wkt,
              const las::EbVlr &extra_bytes_vlr);

  protected:
    std::shared_ptr<las::LasHeader> header_;
    std::string wkt_;
    std::shared_ptr<las::EbVlr> eb_vlr_;
};

} // namespace copc::laz
#endif // COPCLIB_LAZ_CONFIG_H_
