#ifndef COPCLIB_LAS_POINTS_H_
#define COPCLIB_LAS_POINTS_H_

#include <copc-lib/las/point.hpp>
#include <utility>
#include <vector>

namespace copc::las
{
class Points
{
  public:
    Points(const int8_t &point_format_id, const uint16_t &num_extra_bytes)
        : point_format_id_(point_format_id),
          point_record_length_(Point::BaseByteSize(point_format_id) + num_extra_bytes),
          points_(0, Point(point_format_id, num_extra_bytes))
    {
        if (point_format_id < 0 || point_format_id > 10)
            throw std::runtime_error("Point format must be 0-10.");
    };
    Points(const int8_t &point_format_id, const uint16_t &num_extra_bytes, const uint32_t &number_of_points)
        : point_format_id_(point_format_id),
          point_record_length_(Point::BaseByteSize(point_format_id) + num_extra_bytes),
          points_(number_of_points, Point(point_format_id, num_extra_bytes))
    {
        if (point_format_id < 0 || point_format_id > 10)
            throw std::runtime_error("Point format must be 0-10.");
    };

    Points(const int8_t &point_format_id, const uint16_t &num_extra_bytes, const std::vector<Point> &points)
        : point_format_id_(point_format_id),
          point_record_length_(Point::BaseByteSize(point_format_id) + num_extra_bytes)
    {
        if (point_format_id < 0 || point_format_id > 10)
            throw std::runtime_error("Point format must be 0-10.");

        points_.reserve(points.size());
        for (const auto &point : points)
        {
            if (point.PointFormatID() != point_format_id_ || point.PointRecordLength() != point_record_length_)
                throw std::runtime_error("All points formats must be of the requested point format.");
            else
                points_.push_back(point);
        }
    };

    void ToPointFormat(const int8_t &point_format_id)
    {
        if (point_format_id < 0 || point_format_id > 10)
            throw std::runtime_error("Point format must be 0-10.");
        for (auto &point : points_)
            point.ToPointFormat(point_format_id);
        point_format_id_ = point_format_id;
    }

    std::vector<Point> GetPoints() { return points_; }

    int8_t PointFormatID() const { return point_format_id_; }
    uint32_t PointRecordLength() const { return point_record_length_; }

    // Add a Point to points_
    void AddPoint(const Point &point)
    {
        if (point.PointFormatID() == point_format_id_ && point.PointRecordLength() == point_record_length_)
            points_.push_back(point);
        else
        {
            throw std::runtime_error("New point must be of same format and size.");
        }
    }

    // Add points from other Points to points_
    void AddPoints(Points points)
    {
        if (points.PointFormatID() == point_format_id_ && points.PointRecordLength() == point_record_length_)
            for (const auto &point : points.GetPoints())
                points_.push_back(point);
        else
        {
            throw std::runtime_error("New points must be of same format and size.");
        }
    }

  private:
    std::vector<Point> points_;
    int8_t point_format_id_;
    uint32_t point_record_length_;
};
} // namespace copc::las
#endif // COPCLIB_LAS_POINTS_H_
