#ifndef COPCLIB_IO_LAZ_BASE_WRITER_H_
#define COPCLIB_IO_LAZ_BASE_WRITER_H_

#include <array>
#include <iterator>
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
#include "copc-lib/las/vlr.hpp"

namespace copc::laz
{

class BaseWriter
{
  public:
    BaseWriter(std::ostream &out_stream, std::shared_ptr<las::LazConfigWriter> laz_config_writer)
        : out_stream_(out_stream), config_(laz_config_writer)
    {
        // reserve enough space for the header & VLRs in the file
        std::fill_n(std::ostream_iterator<char>(out_stream_), FirstChunkOffset(), 0);
        open_ = true;
        std::cout << "BaseWriter::BaseWriter" << std::endl;
    }

    virtual void Close();
    // Call close on destructor if needed
    ~BaseWriter() { Close(); }

  protected:
    const uint32_t VARIABLE_CHUNK_SIZE = (std::numeric_limits<uint32_t>::max)();

    // 8 bytes for the chunk table offset
    uint64_t FirstChunkOffset() const { return OffsetToPointData() + sizeof(uint64_t); };

    // Base constructor used in WriterInternal Class
    BaseWriter(std::ostream &out_stream) : out_stream_(out_stream)
    {
        std::cout << "BaseWriter::BaseWriter (BASE)" << std::endl;
    }

    bool open_{};
    std::ostream &out_stream_;
    std::vector<lazperf::chunk> chunks_;
    uint64_t point_count_{};
    uint64_t evlr_offset_{};
    uint32_t evlr_count_{};

    virtual size_t OffsetToPointData() const;
    virtual void WriteHeader();
    void WriteChunkTable();

    std::shared_ptr<las::LazConfigWriter> config_;
};
} // namespace copc::laz

#endif // COPCLIB_IO_LAZ_BASE_WRITER_H_
