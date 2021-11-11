#ifndef COPCLIB_IO_WRITER_INTERNAL_H_
#define COPCLIB_IO_WRITER_INTERNAL_H_

#include <ostream>

#include "copc-lib/copc/config.hpp"
#include "copc-lib/io/base_io.hpp"
#include "copc-lib/las/header.hpp"

namespace copc::Internal
{

class WriterInternal
{
  public:
    const uint32_t VARIABLE_CHUNK_SIZE = (std::numeric_limits<uint32_t>::max)();

    // 8 bytes for the chunk table offset
    uint64_t FirstChunkOffset() const { return OffsetToPointData() + sizeof(uint64_t); };

    WriterInternal(std::ostream &out_stream, std::shared_ptr<CopcConfigWriter> copc_config,
                   std::shared_ptr<Hierarchy> hierarchy);

    // Writes the header and COPC vlrs
    void Close();
    // Call close on destructor if needed
    ~WriterInternal() { Close(); }

    bool IsOpen() const { return open_; }

    // Writes a chunk to the laz file
    Entry WriteNode(std::vector<char> in, int32_t point_count, bool compressed);

  private:
    bool open_{};

    std::ostream &out_stream_;
    std::shared_ptr<CopcConfigWriter> copc_config_;
    std::shared_ptr<Hierarchy> hierarchy_;

    std::vector<lazperf::chunk> chunks_;
    uint64_t point_count_{};
    uint64_t evlr_offset_{};
    uint32_t evlr_count_{};

    size_t OffsetToPointData() const;

    void WriteHeader();
    void WriteChunkTable();
    void WritePage(const std::shared_ptr<PageInternal> &page);

    void ComputePageHierarchy();

    // Iterates through a given page in a postorder traversal and writes the pages
    void WritePageTree(const std::shared_ptr<PageInternal> &current);
};
} // namespace copc::Internal
#endif // COPCLIB_IO_WRITER_INTERNAL_H_
