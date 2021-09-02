#include <copc-lib/hierarchy/node.hpp>

#include <string>

#include <copc-lib/hierarchy/page.hpp>
#include <copc-lib/las/point.hpp>
#include <copc-lib/laz/decompressor.hpp>

namespace copc::hierarchy
{

std::vector<las::Point> Node::GetPoints()
{
    // Get data from node chunk in laz file
    reader_->in_stream.seekg(offset_);
    std::vector<char> point_data = laz::Decompressor::DecompressBytes(reader_, point_count);

    // Make a stream out of the vector of char
    auto ss = std::istringstream(std::string(point_data.begin(), point_data.end()));

    // Go through each Point to unpack the data from the stream
    std::vector<las::Point> points(point_count, reader_->file->GetLasHeader().point_format_id);

    // Unpack points
    for (auto &point : points)
        point.Unpack(ss, reader_->file->GetLasHeader().point_record_length);

    return points;
}

void Node::PackPoints(const std::vector<las::Point> &points, std::ostream &out_stream)
{
    for (const auto &point : points)
        point.Pack(out_stream);
}

std::vector<char> Node::GetPointData()
{
    if (this == nullptr || !this->IsValid())
        return {};

    reader_->in_stream.seekg(offset_);
    return laz::Decompressor::DecompressBytes(reader_, point_count);
}

} // namespace copc::hierarchy
