#include "copc-lib/io/laz_reader.hpp"

namespace copc::laz
{
std::vector<char> LazReader::GetPointData()
{
    auto las_header = las_config_.LasHeader();
    // Seek to the end of the chunk table offset/start of the points
    in_stream_->seekg(las_header.PointOffset() + sizeof(int64_t));

    std::vector<char> out;
    int point_size = copc::las::PointByteSize(las_header.PointFormatId(), las_header.EbByteSize());
    char buff[255];
    for (size_t i = 0; i < las_header.PointCount(); i++)
    {
        reader_->readPoint(buff);
        out.insert(out.end(), buff, buff + point_size);
    }

    return out;
}

las::Points LazReader::GetPoints()
{
    std::vector<char> point_data = GetPointData();

    if (point_data.empty())
        return las::Points(las_config_.LasHeader());

    return las::Points::Unpack(point_data, las_config_.LasHeader());
}
} // namespace copc::laz
