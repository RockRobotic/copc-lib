#include "copc-lib/io/laz_writer.hpp"

#include <memory>

namespace copc::laz
{

LazWriter::LazWriter(std::ostream &out_stream, const las::LazConfigWriter &laz_config_writer)
    : BaseWriter(out_stream,
                 std::static_pointer_cast<las::LazConfig>(std::make_shared<las::LazConfigWriter>(laz_config_writer)))
{
    // reserve enough space for the header & VLRs in the file
    std::fill_n(std::ostream_iterator<char>(out_stream_), FirstChunkOffset(), 0);
}

// Write a group of points as a chunk
void LazWriter::WritePoints(const las::Points &points)
{
    if (points.Size() == 0)
        return;
    if (points.PointFormatId() != config_->LasHeader().PointFormatId() ||
        points.PointRecordLength() != config_->LasHeader().PointRecordLength())
        throw std::runtime_error("LazWriter::WritePoints: New points must be of same format and size.");

    std::vector<char> uncompressed_data = points.Pack(config_->LasHeader());
    WriteChunk(uncompressed_data);
}

// Write a group of points as a chunk
void LazWriter::WritePointsCompressed(std::vector<char> const &compressed_data, int32_t point_count)
{
    if (point_count == 0)
        throw std::runtime_error("Point count must be >0!");

    WriteChunk(compressed_data, point_count, true);
}

} // namespace copc::laz
