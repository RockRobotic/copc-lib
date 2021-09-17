#ifndef COPCLIB_LAS_POINTS_H_
#define COPCLIB_LAS_POINTS_H_

#include <algorithm>
#include <utility>
#include <vector>

#include <copc-lib/las/header.hpp>
#include <copc-lib/las/point.hpp>
#include <copc-lib/las/utils.hpp>

namespace copc::las
{
// The Points class provides a wrapper around a vector of copc::las::Point objects
// It is recommended to use this class over a vector, and to use the CreatePoint function
// rather than calling the point constructor directly.
class Points
{
  public:
    Points(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset,
           const uint16_t &num_extra_bytes = 0);
    Points(const LasHeader &header);
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
    void AddPoints(std::vector<las::Point> points);

    // Point functions
    las::Point CreatePoint() { return las::Point(point_format_id_, scale_, offset_, NumExtraBytes()); }
    void ToPointFormat(const int8_t &point_format_id);

    // Pack/unpack
    std::vector<char> Pack();
    void Pack(std::ostream &out_stream);
    static Points Unpack(const std::vector<char> &point_data, const int8_t &point_format_id,
                         const uint16_t &num_extra_bytes, const Vector3 &scale, const Vector3 &offset);
    static Points Unpack(const std::vector<char> &point_data, const LasHeader &header);

    std::string ToString() const;

    // Getters and setters
    std::vector<double> X() const
    {
        std::vector<double> out;
        out.resize(Size());
        std::transform(points_.begin(), points_.end(), out.begin(), [](Point p) { return p.X(); });
        return out;
    }
    void X(const std::vector<double> &in)
    {
        if (in.size() != Size())
            throw std::runtime_error("X setter array must be same size as Points array!");

        for (unsigned i = 0; i < points_.size(); ++i)
            points_[i].X(in[i]);
    }

    std::vector<double> Y() const
    {
        std::vector<double> out;
        out.resize(Size());
        std::transform(points_.begin(), points_.end(), out.begin(), [](const Point &p) { return p.Y(); });
        return out;
    }
    void Y(const std::vector<double> &in)
    {
        if (in.size() != Size())
            throw std::runtime_error("Y setter array must be same size as Points array!");

        for (unsigned i = 0; i < points_.size(); ++i)
            points_[i].Y(in[i]);
    }

    std::vector<double> Z() const
    {
        std::vector<double> out;
        out.resize(Size());
        std::transform(points_.begin(), points_.end(), out.begin(), [](const Point &p) { return p.Z(); });
        return out;
    }
    void Z(const std::vector<double> &in)
    {
        if (in.size() != Size())
            throw std::runtime_error("Z setter array must be same size as Points array!");

        for (unsigned i = 0; i < points_.size(); ++i)
            points_[i].Z(in[i]);
    }

  private:
    std::vector<Point> points_;
    int8_t point_format_id_;
    uint32_t point_record_length_;
    Vector3 scale_;
    Vector3 offset_;
};
} // namespace copc::las
#endif // COPCLIB_LAS_POINTS_H_
