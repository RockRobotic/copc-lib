#include "copc-lib/io/laz_base_writer.hpp"

#include <lazperf/filestream.hpp>
#include <lazperf/vlr.hpp>

#include "copc-lib/laz/compressor.hpp"

namespace copc::laz
{

size_t BaseWriter::OffsetToPointData() const
{
    // LAS Extra Byte VLR
    size_t las_eb_vlr_size = config_->ExtraBytesVlr().size();
    if (las_eb_vlr_size > 0)
        las_eb_vlr_size += lazperf::vlr_header::Size;

    // LAZ VLR
    size_t laz_vlr_size =
        lazperf::laz_vlr(config_->LasHeader().PointFormatId(), config_->LasHeader().EbByteSize(), VARIABLE_CHUNK_SIZE)
            .size();
    laz_vlr_size += lazperf::vlr_header::Size;

    return las::LasHeader::HEADER_SIZE_BYTES + las_eb_vlr_size + laz_vlr_size;
}

void BaseWriter::WriteLasHeader(bool extended_stats_flag)
{
    // Write LAS header
    auto las_header = config_->LasHeader().ToLazPerf(OffsetToPointData(), point_count_, evlr_offset_, evlr_count_,
                                                     config_->LasHeader().EbByteSize(), extended_stats_flag);
    out_stream_.seekp(0);
    las_header.write(out_stream_);
}

void BaseWriter::WriteLazAndEbVlrs()
{

    // Write optional LAS Extra Byte VLR
    if (config_->LasHeader().EbByteSize() > 0)
    {
        auto ebVlr = this->config_->ExtraBytesVlr();
        ebVlr.header().write(out_stream_);
        ebVlr.write(out_stream_);
    }

    // Write the LAZ VLR
    lazperf::laz_vlr lazVlr(config_->LasHeader().PointFormatId(), config_->LasHeader().EbByteSize(),
                            VARIABLE_CHUNK_SIZE);
    lazVlr.header().write(out_stream_);
    lazVlr.write(out_stream_);
}

// Writes the LAS header and VLRs
void BaseWriter::WriteHeader()
{
    WriteLasHeader(false);

    WriteLazAndEbVlrs();

    // Make sure that we haven't gone over allocated size
    if (out_stream_.tellp() > OffsetToPointData())
        throw std::runtime_error("BaseWriter::WriteHeader: LasHeader + VLRs are bigger than offset to point data.");
}

void BaseWriter::WriteChunkTable()
{
    // move to the end of the file to start emitting our compressed table
    out_stream_.seekp(0, std::ios::end);

    // take note of where we're writing the chunk table, we need this later
    auto chunk_table_offset = static_cast<uint64_t>(out_stream_.tellp());

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
    out_stream_.seekp(static_cast<int64_t>(OffsetToPointData()));
    out_stream_.write(reinterpret_cast<char *>(&chunk_table_offset), sizeof(uint64_t));
}

void BaseWriter::WriteWKT()
{
    // If WKT is provided, write it as EVLR
    if (!config_->Wkt().empty())
    {
        evlr_count_++;
        lazperf::wkt_vlr wkt_vlr(config_->Wkt());
        wkt_vlr.eheader().write(out_stream_);
        wkt_vlr.write(out_stream_);
    }
}

int32_t BaseWriter::WriteChunk(const std::vector<char> &in, int32_t point_count, bool compressed, uint64_t *offset,
                               int32_t *byte_size)
{
    uint64_t startpos = out_stream_.tellp();
    if (startpos <= 0)
        throw std::runtime_error("BaseWriter::WriteChunk: Error while writing chunk!");
    if (offset != nullptr)
        *offset = static_cast<uint64_t>(startpos);

    if (compressed)
        out_stream_.write(in.data(), in.size());
    else
        point_count = laz::Compressor::CompressBytes(out_stream_, config_->LasHeader(), in);

    point_count_ += point_count;

    uint64_t endpos = out_stream_.tellp();
    if (endpos <= 0)
        throw std::runtime_error("BaseWriter::WriteChunk: Error while writing chunk!");

    chunks_.push_back(lazperf::chunk{static_cast<uint64_t>(point_count), endpos});

    auto size = endpos - startpos;
    if (size > (std::numeric_limits<int32_t>::max)())
        throw std::runtime_error("BaseWriter::WriteChunk: Chunk is too large!");
    if (byte_size != nullptr)
        *byte_size = static_cast<int32_t>(size);
    if (point_count > (std::numeric_limits<int32_t>::max)())
        throw std::runtime_error("BaseWriter::WriteChunk: Chunk has too many points!");
    return point_count;
}

void BaseWriter::Close()
{
    if (!open_)
        return;

    WriteChunkTable();

    // Set hierarchy evlr
    out_stream_.seekp(0, std::ios::end);
    evlr_offset_ = out_stream_.tellp();

    WriteWKT();

    WriteHeader();

    open_ = false;
}

BaseFileWriter::BaseFileWriter(const std::string &file_path)
{
    file_path_ = file_path;
    f_stream_.open(file_path.c_str(), std::ios::out | std::ios::binary);
    if (!f_stream_.good())
        throw std::runtime_error("FileWriterBase: Error while opening file path.");
}

void BaseFileWriter::Close()
{
    if (f_stream_.is_open())
        f_stream_.close();
}
} // namespace copc::laz
