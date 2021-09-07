#include <copc-lib/hierarchy/node.hpp>

#include <iterator>
#include <sstream>
#include <string>

#include <copc-lib/hierarchy/page.hpp>
#include <copc-lib/las/point.hpp>
#include <copc-lib/laz/decompressor.hpp>

namespace copc
{

std::vector<char> Node::PackPoints(const std::vector<las::Point> &points)
{
    std::stringstream out;
    PackPoints(points, out);
    auto ostr = out.str();
    return std::vector<char>(ostr.begin(), ostr.end());
}

std::vector<las::Point> Node::UnpackPoints(const std::vector<char> &point_data, int point_format_id,
                                                 int point_record_length)
{
    if (point_data.size() % point_record_length != 0)
        throw std::runtime_error("Invalid input point array!");

    uint64_t point_count = point_data.size() / point_record_length;

    // Make a stream out of the vector of char
    auto ss = std::istringstream(std::string(point_data.begin(), point_data.end()));

    // Go through each Point to unpack the data from the stream
    std::vector<las::Point> points(point_count, point_format_id);

    // Unpack points
    for (auto &point : points)
        point.Unpack(ss, point_record_length);

    return points;
}

void Node::PackPoints(const std::vector<las::Point> &points, std::ostream &out_stream)
{
    for (const auto &point : points)
        point.Pack(out_stream);
}

// this isn't where this goes
bool operator==(const Page &lhs, const Page &rhs) { return lhs.IsEqual(rhs) && lhs.loaded == rhs.loaded; }

} // namespace copc
