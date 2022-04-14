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

class LazWriter : public BaseWriter
{

  public:
    LazWriter(std::ostream &out_stream, const las::LazConfigWriter &las_config_writer);

    // Write a group of points as a chunk
    void WritePoints(const las::Points &points);

    std::shared_ptr<las::LazConfigWriter> LazConfig()
    {
        return std::dynamic_pointer_cast<las::LazConfigWriter>(config_);
    }
    uint64_t PointCount() { return point_count_; }
    uint64_t ChunkCount() { return chunks_.size(); }
};

class LazFileWriter : public BaseFileWriter
{

  public:
    LazFileWriter(const std::string &file_path, const las::LazConfigWriter &laz_config_writer);
    void Close() override;
    ~LazFileWriter() { Close(); };

    // Write a group of points as a chunk
    void WritePoints(const las::Points &points) { writer_->WritePoints(points); }
    uint64_t PointCount() { return writer_->PointCount(); }
    uint64_t ChunkCount() { return writer_->ChunkCount(); }
    std::shared_ptr<las::LazConfigWriter> LazConfig() { return writer_->LazConfig(); }

  private:
    std::shared_ptr<LazWriter> writer_;
};

} // namespace copc::laz
#endif // COPCLIB_IO_LAZ_WRITER_H_
