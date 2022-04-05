#ifndef COPCLIB_IO_LAZ_WRITER_H_
#define COPCLIB_IO_LAZ_WRITER_H_

#include <array>
#include <memory>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>

#include "copc-lib/copc/copc_config.hpp"
#include "copc-lib/geometry/vector3.hpp"
#include "copc-lib/io/laz_base_writer.hpp"
#include "copc-lib/las/header.hpp"
#include "copc-lib/las/laz_config.hpp"
#include "copc-lib/las/points.hpp"
#include "copc-lib/las/utils.hpp"

namespace copc::laz
{

class Writer : public BaseWriter
{

  public:
    Writer(std::ostream &out_stream, const las::LazConfigWriter &las_config);

    int32_t WriteChunk(std::vector<char> in, int32_t point_count, bool compressed);

    // Write a group of points as a chunk
    void WritePoints(const las::Points &points);

    std::shared_ptr<las::LazConfigWriter> LazConfig() { return config_; }
};

class FileWriter : public Writer
{

  public:
    FileWriter(const std::string &file_path, const las::LazConfigWriter &laz_config_writer);

    void Close() override;

  private:
    std::fstream f_stream_;
};

} // namespace copc::laz
#endif // COPCLIB_IO_LAZ_WRITER_H_
