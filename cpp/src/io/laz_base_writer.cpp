#include "copc-lib/io/laz_base_writer.hpp"

#include <lazperf/filestream.hpp>
#include <lazperf/vlr.hpp>

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

// Writes the LAS header and VLRs
void BaseWriter::WriteHeader()
{
    // Write LAS header
    auto las_header_vlr = config_->LasHeader()->ToLazPerf(OffsetToPointData(), point_count_, evlr_offset_, evlr_count_,
                                                          config_->LasHeader()->EbByteSize(), false);
    out_stream_.seekp(0);
    las_header_vlr.write(out_stream_);

    // Write the LAZ VLR
    lazperf::laz_vlr lazVlr(config_->LasHeader()->PointFormatId(), config_->LasHeader()->EbByteSize(),
                            VARIABLE_CHUNK_SIZE);
    lazVlr.header().write(out_stream_);
    lazVlr.write(out_stream_);

    // Write optional Extra Byte VLR
    if (config_->LasHeader()->EbByteSize() > 0)
    {
        auto ebVlr = this->config_->ExtraBytesVlr();
        ebVlr.header().write(out_stream_);
        ebVlr.write(out_stream_);
    }

    // Make sure that we haven't gone over allocated size
    if (static_cast<int64_t>(out_stream_.tellp()) > OffsetToPointData())
        throw std::runtime_error("BaseWriter::WriteHeader: LasHeader + VLRs are bigger than offset to point data.");
}

void BaseWriter::WriteChunkTable()
{
    // move to the end of the file to start emitting our compressed table
    out_stream_.seekp(0, std::ios::end);

    // take note of where we're writing the chunk table, we need this later
    auto chunk_table_offset = static_cast<int64_t>(out_stream_.tellp());

    // Fixup the chunk table to be relative offsets rather than absolute ones.
    uint64_t prevOffset = FirstChunkOffset();
    for (auto &c : chunks_)
    {
        uint64_t relOffset = c.offset - prevOffset;
        prevOffset = c.offset;
        c.offset = relOffset;
    }

    // write out the chunk table header (version and total chunks)
    uint32_t version = 0;
    out_stream_.write((const char *)&version, sizeof(uint32_t));
    if (chunks_.size() > (std::numeric_limits<uint32_t>::max)())
        throw std::runtime_error("You've got way too many chunks!");
    auto numChunks = static_cast<uint32_t>(chunks_.size());
    out_stream_.write((const char *)&numChunks, sizeof(uint32_t));

    // Write the chunk table
    lazperf::OutFileStream w(out_stream_);

    compress_chunk_table(w.cb(), chunks_, true);
    // go back to where we're supposed to write chunk table offset
    out_stream_.seekp(static_cast<long>(OffsetToPointData()));
    out_stream_.write(reinterpret_cast<char *>(&chunk_table_offset), sizeof(chunk_table_offset));
}

void BaseWriter::Close()
{
    if (!open_)
        return;

    WriteChunkTable();

    // Set hierarchy evlr
    out_stream_.seekp(0, std::ios::end);
    evlr_offset_ = static_cast<int64_t>(out_stream_.tellp());

    // If WKT is provided, write it as EVLR
    if (!config_->Wkt().empty())
    {
        evlr_count_++;
        lazperf::wkt_vlr wkt_vlr(config_->Wkt());
        wkt_vlr.eheader().write(out_stream_);
        wkt_vlr.write(out_stream_);
    }

    WriteHeader();

    open_ = false;
}

} // namespace copc::laz
