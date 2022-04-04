#include "copc-lib/io/laz_base_writer.hpp"

namespace copc::laz
{

size_t BaseWriter::OffsetToPointData() const
{
    size_t base_offset(las::LAZ_HEADER_SIZE + (54 + (34 + 4 * 6))); // header + LAZ vlr (max 4 items)

    size_t eb_offset = config_->ExtraBytesVlr().size();
    if (eb_offset > 0)
        eb_offset += lazperf::vlr_header::Size;

    return base_offset + eb_offset;
}

} // namespace copc::laz
