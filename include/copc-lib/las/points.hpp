#ifndef COPCLIB_LAS_POINTS_H_
#define COPCLIB_LAS_POINTS_H_

#include <utility>
#include <vector>

#include <copc-lib/las/point.hpp>
#include <copc-lib/las/utils.hpp>

namespace copc::las
{
// The Points class provides a wrapper around a vector of copc::las::Point objects
// It is recommended to use this class over a vector, and to use the NewPoint function
// rather than calling the point constructor directly.
class Points
{
  public:
    Points(const int8_t &point_format_id, const uint16_t &num_extra_bytes = 0);
    // Will create Points object given a points vector
    Points(const std::vector<Point> &points);

    // Getters
    int8_t PointFormatID() const { return point_format_id_; }
    uint32_t PointRecordLength() const { return point_record_length_; }
    uint32_t NumExtraBytes() const { return ComputeNumExtraBytes(point_format_id_, point_record_length_); }

    // Vector functions
    std::vector<Point> Get() { return points_; }
    Point Get(const size_t &idx) { return points_[idx]; }
    size_t Size() const { return points_.size(); }
    void Reserve(const size_t &num) { points_.reserve(num); }

    // Add points functions
    void AddPoint(const Point &point);
    void AddPoints(Points points);
    // TODO[Leo]: Add this to tests
    void AddPoints(std::vector<las::Point> points);

    // Point functions
    las::Point NewPoint() { return las::Point(point_format_id_, NumExtraBytes()); }
    void ToPointFormat(const int8_t &point_format_id);

    // Pack/unpack
    std::vector<char> Pack();
    void Pack(std::ostream &out_stream);
    static Points Unpack(const std::vector<char> &point_data, int8_t point_format_id, int point_record_length);

    std::string ToString() const;

  private:
    std::vector<Point> points_;
    int8_t point_format_id_;
    uint32_t point_record_length_;
};
} // namespace copc::las
#endif // COPCLIB_LAS_POINTS_H_
