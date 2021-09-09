#include <copc-lib/las/points.hpp>
#include <iterator>
#include <sstream>
#include <string>

namespace copc::las
{

Points::Points(const int8_t &point_format_id, const uint16_t &num_extra_bytes) : point_format_id_(point_format_id)
{
    if (point_format_id < 0 || point_format_id > 10)
        throw std::runtime_error("Point format must be 0-10.");

    point_record_length_ = Point::ComputePointBytes(point_format_id, num_extra_bytes);
};

Points::Points(const std::vector<Point> &points)
{
    if (points.empty())
        throw std::runtime_error("Can't add empty vector of points to Points!");

    point_record_length_ = points[0].PointRecordLength();
    point_format_id_ = points[0].PointFormatID();

    AddPoints(points);
}

void Points::ToPointFormat(const int8_t &point_format_id)
{
    if (point_format_id < 0 || point_format_id > 10)
        throw std::runtime_error("Point format must be 0-10.");
    for (auto &point : points_)
        point.ToPointFormat(point_format_id);
    point_format_id_ = point_format_id;
}

void Points::AddPoint(const Point &point)
{
    if (point.PointFormatID() == point_format_id_ && point.PointRecordLength() == point_record_length_)
        points_.push_back(point);
    else
        throw std::runtime_error("New point must be of same format and size.");
}

void Points::AddPoints(Points points)
{
    if (points.PointFormatID() != point_format_id_ || points.PointRecordLength() != point_record_length_)
        throw std::runtime_error("New points must be of same format and size.");

    auto point_vec = points.Get();
    points_.insert(points_.end(), point_vec.begin(), point_vec.end());
}

void Points::AddPoints(std::vector<las::Point> points)
{
    for (const auto &point : points)
    {
        if (point.PointFormatID() != point_format_id_ || point.PointRecordLength() != point_record_length_)
            throw std::runtime_error("New points must be of same format and size.");
    }

    points_.insert(points_.end(), points.begin(), points.end());
}

Points Points::Unpack(const std::vector<char> &point_data, int8_t point_format_id, int point_record_length)
{
    if (point_data.size() % point_record_length != 0)
        throw std::runtime_error("Invalid input point array!");

    auto num_extra_bytes = las::Point::ComputeNumExtraBytes(point_format_id, point_record_length);
    uint64_t point_count = point_data.size() / point_record_length;

    // Make a stream out of the vector of char
    auto ss = std::istringstream(std::string(point_data.begin(), point_data.end()));

    // Go through each Point to unpack the data from the stream
    Points points(point_format_id, num_extra_bytes);
    points.Reserve(point_count);

    // Unpack points
    for (int i = 0; i < point_count; i++)
    {
        points.AddPoint(las::Point::Unpack(ss, point_format_id, num_extra_bytes));
    }

    return points;
}

void Points::Pack(std::ostream &out_stream)
{
    for (const auto &point : points_)
        point.Pack(out_stream);
}

std::vector<char> Points::Pack()
{
    std::stringstream out;
    Pack(out);
    auto ostr = out.str();
    return std::vector<char>(ostr.begin(), ostr.end());
}

} // namespace copc::las