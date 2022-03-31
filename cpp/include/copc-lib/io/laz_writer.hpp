#ifndef COPCLIB_IO_LAZ_WRITER_H_
#define COPCLIB_IO_LAZ_WRITER_H_

#include <array>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>

#include "copc-lib/copc/config.hpp"
#include "copc-lib/io/copc_base_io.hpp"
#include "copc-lib/las/config.hpp"
#include "copc-lib/las/header.hpp"
#include "copc-lib/las/points.hpp"
#include "copc-lib/las/utils.hpp"
#include "copc-lib/las/vector3.hpp"

namespace copc::laz
{

class Writer
{

  public:
    void InitWriter(std::ostream &out_stream, const LasConfig &las_config, const std::optional<int8_t> &point_format_id,
                    const std::optional<Vector3> &scale, const std::optional<Vector3> &offset,
                    const std::optional<std::string> &wkt, const std::optional<las::EbVlr> &extra_bytes_vlr);

    void WritePoints(const copc::Points &points);
    void WritePoint(const copc::Point &point);

    void Close();
};

class FileWriter : Writer
{

  public:
    void InitWriter(const std::string &file_path, const LasConfig &las_config,
                    const std::optional<int8_t> &point_format_id, const std::optional<Vector3> &scale,
                    const std::optional<Vector3> &offset, const std::optional<std::string> &wkt,
                    const std::optional<las::EbVlr> &extra_bytes_vlr);

    void Close();
};

} // namespace copc::laz
#endif // COPCLIB_IO_LAZ_WRITER_H_
