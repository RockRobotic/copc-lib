#include "copc-lib/io/laz_writer.hpp"

#include <memory>

#include "copc-lib/laz/compressor.hpp"

namespace copc::laz
{

LazWriter::LazWriter(std::ostream &out_stream, const las::LazConfigWriter &laz_config_writer)
    : BaseWriter(out_stream,
                 std::static_pointer_cast<las::LazConfig>(std::make_shared<las::LazConfigWriter>(laz_config_writer)))
{
    std::cout << out_stream_.tellp() << std::endl;
    // reserve enough space for the header & VLRs in the file
    std::fill_n(std::ostream_iterator<char>(out_stream_), FirstChunkOffset(), 0);
    std::cout << out_stream_.tellp() << std::endl;
    open_ = true;
}

int32_t LazWriter::WriteChunk(std::vector<char> in, int32_t point_count, bool compressed)
{
    uint64_t startpos = out_stream_.tellp();
    if (startpos <= 0)
        throw std::runtime_error("LazWriter::WriteChunk: Error while writing chunk!");

    if (compressed)
        out_stream_.write(in.data(), in.size());
    else
        point_count = laz::Compressor::CompressBytes(out_stream_, config_->LasHeader(), in);

    point_count_ += point_count;

    uint64_t endpos = out_stream_.tellp();
    if (endpos <= 0)
        throw std::runtime_error("LazWriter::WriteChunk: sError while writing chunk!");

    chunks_.push_back(lazperf::chunk{static_cast<uint64_t>(point_count), endpos});

    auto size = endpos - startpos;
    if (size > (std::numeric_limits<int32_t>::max)())
        throw std::runtime_error("LazWriter::WriteChunk: Chunk is too large!");
    if (point_count > (std::numeric_limits<int32_t>::max)())
        throw std::runtime_error("LazWriter::WriteChunk: Chunk has too many points!");
    return point_count;
}

// Write a group of points as a chunk
void LazWriter::WritePoints(const las::Points &points)
{
    if (points.Size() == 0)
        return;
    if (points.PointFormatId() != config_->LasHeader().PointFormatId() ||
        points.PointRecordLength() != config_->LasHeader().PointRecordLength())
        throw std::runtime_error("LazWriter::WritePoints: New points must be of same format and size.");

    std::vector<char> uncompressed_data = points.Pack();
    WriteChunk(uncompressed_data, 0, false);
}

LazFileWriter::LazFileWriter(const std::string &file_path, const las::LazConfigWriter &laz_config_writer)
    : BaseFileWriter(file_path)
{
    writer_ = std::make_shared<LazWriter>(f_stream_, laz_config_writer);
}

void LazFileWriter::Close()
{
    if (writer_ != nullptr)
        writer_->Close();
    BaseFileWriter::Close();
}
} // namespace copc::laz
