#include "copc-lib/io/laz_writer.hpp"

#include "copc-lib/laz/compressor.hpp"

namespace copc::laz
{

Writer::Writer(std::ostream &out_stream, const las::LazConfigWriter &laz_config_writer) : BaseWriter(out_stream)
{
    config_ = std::make_shared<copc::las::LazConfigWriter>(laz_config_writer);
}

int32_t Writer::WriteChunk(std::vector<char> in, int32_t point_count, bool compressed)
{
    auto startpos = out_stream_.tellp();
    if (startpos <= 0)
        throw std::runtime_error("Error while writing chunk!");

    if (compressed)
        out_stream_.write(in.data(), in.size());
    else
        point_count = laz::Compressor::CompressBytes(out_stream_, *(config_->LasHeader()), in);

    point_count_ += point_count;

    auto endpos = out_stream_.tellp();
    if (endpos <= 0)
        throw std::runtime_error("Error while writing chunk!");

    chunks_.push_back(lazperf::chunk{static_cast<uint64_t>(point_count), static_cast<uint64_t>(endpos)});

    auto size = endpos - startpos;
    if (size > (std::numeric_limits<int32_t>::max)())
        throw std::runtime_error("Chunk is too large!");
    if (point_count > (std::numeric_limits<int32_t>::max)())
        throw std::runtime_error("Chunk has too many points!");
    return point_count;
}

// Write a group of points as a chunk
void Writer::WritePoints(const las::Points &points)
{
    if (points.Size() == 0)
        throw std::runtime_error("Writer::AddNode: Cannot add empty las::Points.");
    if (points.PointFormatId() != config_->LasHeader()->PointFormatId() ||
        points.PointRecordLength() != config_->LasHeader()->PointRecordLength())
        throw std::runtime_error("Writer::AddNode: New points must be of same format and size.");

    std::vector<char> uncompressed_data = points.Pack();
    WriteChunk(uncompressed_data, 0, false);
}

FileWriter::FileWriter(const std::string &file_path, const las::LazConfigWriter &laz_config_writer)
    : Writer(f_stream_, laz_config_writer)
{
    f_stream_.open(file_path.c_str(), std::ios::out | std::ios::binary);
    if (!f_stream_.good())
        throw std::runtime_error("FileWriter: Error while opening file path.");
}

void FileWriter::Close()
{
    BaseWriter::Close();
    f_stream_.close();
}

} // namespace copc::laz
