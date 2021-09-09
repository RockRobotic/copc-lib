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
    Points(const int8_t &point_format_id, const uint16_t &num_extra_bytes);
    Points(const int8_t &point_format_id, const uint16_t &num_extra_bytes, const uint32_t &number_of_points);
    Points(const int8_t &point_format_id, const uint16_t &num_extra_bytes, const std::vector<Point> &points);

    std::vector<Point> Get() { return points_; }

    int8_t PointFormatID() const { return point_format_id_; }
    uint32_t PointRecordLength() const { return point_record_length_; }
    size_t Size() const { return points_.size(); }

    // Add a Point to points_
    void AddPoint(const Point &point);
    // Add points from other Points to points_
    void AddPoints(Points points);
    // Converts all Point objects to a different format
    void ToPointFormat(const int8_t &point_format_id);

    // Provides helper functions for handling lists of Point objects
    std::vector<char> Pack();
    void Pack(std::ostream &out_stream);
    static Points Unpack(const std::vector<char> &point_data, int8_t point_format_id,
                                                int point_record_length);

  private:
    std::vector<Point> points_;
    int8_t point_format_id_;
    uint32_t point_record_length_;
};
} // namespace copc::las
#endif // COPCLIB_LAS_POINTS_H_
