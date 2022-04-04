#ifndef COPCLIB_IO_LAZ_BASE_WRITER_H_
#define COPCLIB_IO_LAZ_BASE_WRITER_H_

#include <array>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>

#include "copc-lib/copc/copc_config.hpp"
#include "copc-lib/geometry/vector3.hpp"
#include "copc-lib/las/header.hpp"
#include "copc-lib/las/laz_config.hpp"
#include "copc-lib/las/points.hpp"
#include "copc-lib/las/utils.hpp"

namespace copc::laz
{

class BaseWriter
{

  protected:
    BaseWriter(std::ostream &out_stream, const las::LazConfigWriter &laz_config_writer)
        : out_stream_(out_stream), config_(std::make_shared<copc::las::LazConfigWriter>(laz_config_writer))
    {
        // TODO: InitWriter
        // InitWriter(out_stream, las_config_writer);
    }

    // Base constructor used in WriterInternal Class
    BaseWriter(std::ostream &out_stream) : out_stream_(out_stream), config_(nullptr)
    {
        // TODO: InitWriter
        // InitWriter(out_stream, las_config_writer);
    }

    bool open_{};
    std::ostream &out_stream_;
    std::vector<lazperf::chunk> chunks_;
    uint64_t point_count_{};
    uint64_t evlr_offset_{};
    uint32_t evlr_count_{};

    virtual size_t OffsetToPointData() const;
    virtual void WriteHeader();
    virtual void WriteChunkTable();
    virtual void Close();

    std::shared_ptr<las::LazConfigWriter> config_;
};
} // namespace copc::laz

#endif // COPCLIB_IO_LAZ_BASE_WRITER_H_
