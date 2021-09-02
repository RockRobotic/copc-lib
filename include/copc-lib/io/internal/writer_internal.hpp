#ifndef COPCLIB_IO_WRITER_INTERNAL_H_
#define COPCLIB_IO_WRITER_INTERNAL_H_

#include <ostream>

#include <copc-lib/copc/file.hpp>
#include <copc-lib/io/base_io.hpp>
#include <copc-lib/las/header.hpp>

namespace copc
{
const uint32_t VARIABLE_CHUNK_SIZE = (std::numeric_limits<uint32_t>::max)();

class WriterInternal
{
  public:
    WriterInternal(std::ostream &out_stream, std::shared_ptr<CopcFile> file, std::shared_ptr<Hierarchy> hierarchy);

    void Close();

    Entry WriteNode(std::vector<char> in, uint64_t point_count, bool compressed);

  private:
    las::CopcVlr copc_data;
    std::ostream &out_stream;
    std::shared_ptr<CopcFile> file;
    std::shared_ptr<Hierarchy> hierarchy;

    // header + COPC vlr + LAZ vlr (max 4 items)
    const int OFFSET_TO_POINT_DATA = 375 + (54 + 160) + (54 + (34 + 4 * 6));
    const uint64_t FIRST_CHUNK_OFFSET = OFFSET_TO_POINT_DATA + sizeof(uint64_t);

    std::vector<lazperf::chunk> chunks_;
    uint64_t point_count_14_ = 0;

    void WriteHeader(las::LasHeader &head14);
    void WriteChunkTable();
    void WriteWkt(las::LasHeader &head14);
    void WriteChunk(std::vector<char> &compressed);
    void WritePage(std::shared_ptr<PageInternal> page);
    bool InsertPage(std::shared_ptr<PageInternal> page);

    void WritePageTree(std::shared_ptr<PageInternal> current);
};
} // namespace copc
#endif // COPCLIB_IO_WRITER_INTERNAL_H_