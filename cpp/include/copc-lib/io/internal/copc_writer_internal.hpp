#ifndef COPCLIB_IO_COPC_WRITER_INTERNAL_H_
#define COPCLIB_IO_COPC_WRITER_INTERNAL_H_

#include <ostream>

#include "copc-lib/copc/copc_config.hpp"
#include "copc-lib/io/copc_base_io.hpp"
#include "copc-lib/io/laz_base_writer.hpp"
#include "copc-lib/las/header.hpp"

namespace copc::Internal
{

class WriterInternal : laz::BaseWriter
{
  public:
    WriterInternal(std::ostream &out_stream, const std::shared_ptr<CopcConfigWriter> &copc_config,
                   std::shared_ptr<Hierarchy> hierarchy);

    // Writes the header and COPC vlrs
    void Close() override;
    // Call close on destructor if needed
    ~WriterInternal() { Close(); }

    // Writes a chunk to the laz file
    Entry WriteNode(const std::vector<char> &in, int32_t point_count, bool compressed);

  private:
    std::shared_ptr<Hierarchy> hierarchy_;

    std::shared_ptr<CopcConfigWriter> GetConfig() const
    {
        return std::dynamic_pointer_cast<CopcConfigWriter>(config_);
    };

    size_t OffsetToPointData() const override;
    void WriteHeader() override;

    void WritePage(const std::shared_ptr<PageInternal> &page);

    void ComputePageHierarchy();

    // Iterates through a given page in a postorder traversal and writes the pages
    void WritePageTree(const std::shared_ptr<PageInternal> &current);
};
} // namespace copc::Internal
#endif // COPCLIB_IO_COPC_WRITER_INTERNAL_H_
