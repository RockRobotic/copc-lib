#ifndef COPCLIB_IO_WRITER_INTERNAL_H_
#define COPCLIB_IO_WRITER_INTERNAL_H_

#include <ostream>

#include <copc-lib/copc/file.hpp>
#include <copc-lib/io/base_io.hpp>
#include <copc-lib/las/header.hpp>

namespace copc::Internal
{

class WriterInternal
{
  public:
    const uint32_t VARIABLE_CHUNK_SIZE = (std::numeric_limits<uint32_t>::max)();
    // header + COPC vlr + LAZ vlr (max 4 items)
    const int OFFSET_TO_POINT_DATA = 375 + (54 + 160) + (54 + (34 + 4 * 6));
    // 8 bytes for the chunk table offset
    const uint64_t FIRST_CHUNK_OFFSET = OFFSET_TO_POINT_DATA + sizeof(uint64_t);

    WriterInternal(std::ostream &out_stream, std::shared_ptr<CopcFile> file, std::shared_ptr<Hierarchy> hierarchy);

    // Writes the header and COPC vlrs
    void Close();

    // Writes a chunk to the laz file
    Entry WriteNode(std::vector<char> in, uint64_t point_count, bool compressed);

  private:
    las::CopcVlr copc_data;
    std::ostream &out_stream;
    std::shared_ptr<CopcFile> file;
    std::shared_ptr<Hierarchy> hierarchy;

    std::vector<lazperf::chunk> chunks_;
    uint64_t point_count_14_ = 0;

    void WriteHeader(las::LasHeader &head14);
    void WriteChunkTable();
    void WriteWkt(las::LasHeader &head14);
    void WritePage(std::shared_ptr<PageInternal> page);

    // Iterates through a given page in a postorder traversal and writes the pages
    void WritePageTree(std::shared_ptr<PageInternal> current);
};
} // namespace copc::Internal
#endif // COPCLIB_IO_WRITER_INTERNAL_H_