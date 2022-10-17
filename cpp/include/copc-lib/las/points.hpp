#ifndef COPCLIB_LAS_POINTS_H_
#define COPCLIB_LAS_POINTS_H_

#include <algorithm>
#include <utility>
#include <vector>

#include "copc-lib/hierarchy/key.hpp"
#include "copc-lib/las/header.hpp"
#include "copc-lib/las/point.hpp"
#include "copc-lib/las/utils.hpp"

namespace copc::las
{
// The Points class provides a wrapper around a vector of copc::las::Point objects
// It is recommended to use this class over a vector, and to use the CreatePoint function
// rather than calling the point constructor directly.
class Points
{
  public:
    Points(const int8_t &point_format_id, const uint16_t &eb_byte_size = 0);
    Points(const LasHeader &header);
    // Will create Points object given a points vector
    Points(const std::vector<std::shared_ptr<Point>> &points);

    // Getters
    int8_t PointFormatId() const { return point_format_id_; }
    uint32_t PointRecordLength() const { return point_record_length_; }
    uint32_t EbByteSize() const { return copc::las::EbByteSize(point_format_id_, point_record_length_); }

    // Vector functions
    std::vector<std::shared_ptr<Point>> Get() { return points_; }
    size_t Size() const { return points_.size(); }
    void Reserve(const size_t &num) { points_.reserve(num); }

    std::shared_ptr<Point> Get(const size_t &idx) { return points_[idx]; }
    std::shared_ptr<Point> &operator[](size_t i) { return points_[i]; }
    const std::shared_ptr<Point> &operator[](size_t i) const { return points_[i]; }

    std::vector<std::shared_ptr<Point>>::const_iterator begin() const { return points_.begin(); }
    std::vector<std::shared_ptr<Point>>::const_iterator end() const { return points_.end(); }

    // Add points functions
    void AddPoint(const std::shared_ptr<Point> &point);
    void AddPoints(Points points);
    void AddPoints(std::vector<std::shared_ptr<Point>> points);

    // Point functions
    std::shared_ptr<Point> CreatePoint() { return std::make_shared<Point>(point_format_id_, EbByteSize()); }
    void ToPointFormat(const int8_t &point_format_id);

    // Pack/unpack
    std::vector<char> Pack(const LasHeader &header) const;
    std::vector<char> Pack(const Vector3 &scale, const Vector3 &offset) const;
    void Pack(std::ostream &out_stream, const Vector3 &scale, const Vector3 &offset) const;
    void Pack(std::ostream &out_stream, const LasHeader &header) const;
    static Points Unpack(const std::vector<char> &point_data, const int8_t &point_format_id,
                         const uint16_t &eb_byte_size, const Vector3 &scale, const Vector3 &offset);
    static Points Unpack(const std::vector<char> &point_data, const LasHeader &header);

    std::string ToString() const;
    friend std::ostream &operator<<(std::ostream &os, Points const &value)
    {
        os << value.ToString();
        return os;
    }

    // Getters and setters
    std::vector<double> X() const
    {
        std::vector<double> out;
        out.resize(Size());
        std::transform(points_.begin(), points_.end(), out.begin(),
                       [](const std::shared_ptr<Point> &p) { return p->X(); });
        return out;
    }
    void X(const std::vector<double> &in)
    {
        if (in.size() != Size())
            throw std::runtime_error("X setter array must be same size as Points array!");

        for (unsigned i = 0; i < points_.size(); ++i)
            points_[i]->X(in[i]);
    }

    std::vector<double> Y() const
    {
        std::vector<double> out;
        out.resize(Size());
        std::transform(points_.begin(), points_.end(), out.begin(),
                       [](const std::shared_ptr<Point> &p) { return p->Y(); });
        return out;
    }
    void Y(const std::vector<double> &in)
    {
        if (in.size() != Size())
            throw std::runtime_error("Y setter array must be same size as Points array!");

        for (unsigned i = 0; i < points_.size(); ++i)
            points_[i]->Y(in[i]);
    }

    std::vector<double> Z() const
    {
        std::vector<double> out;
        out.resize(Size());
        std::transform(points_.begin(), points_.end(), out.begin(),
                       [](const std::shared_ptr<Point> &p) { return p->Z(); });
        return out;
    }
    void Z(const std::vector<double> &in)
    {
        if (in.size() != Size())
            throw std::runtime_error("Z setter array must be same size as Points array!");

        for (unsigned i = 0; i < points_.size(); ++i)
            points_[i]->Z(in[i]);
    }

    std::vector<uint8_t> Classification() const
    {
        std::vector<uint8_t> out;
        out.resize(Size());
        std::transform(points_.begin(), points_.end(), out.begin(),
                       [](const std::shared_ptr<Point> &p) { return p->Classification(); });
        return out;
    }
    void Classification(const std::vector<uint8_t> &in)
    {
        if (in.size() != Size())
            throw std::runtime_error("Classification setter array must be same size as Points array!");

        for (unsigned i = 0; i < points_.size(); ++i)
            points_[i]->Classification(in[i]);
    }

    std::vector<uint8_t> PointSourceId() const
    {
        std::vector<uint8_t> out;
        out.resize(Size());
        std::transform(points_.begin(), points_.end(), out.begin(),
                       [](const std::shared_ptr<Point> &p) { return p->PointSourceId(); });
        return out;
    }
    void PointSourceId(const std::vector<uint8_t> &in)
    {
        if (in.size() != Size())
            throw std::runtime_error("PointSourceId setter array must be same size as Points array!");

        for (unsigned i = 0; i < points_.size(); ++i)
            points_[i]->PointSourceId(in[i]);
    }

    std::vector<uint16_t> Red() const
    {
        std::vector<uint16_t> out;
        out.resize(Size());
        std::transform(points_.begin(), points_.end(), out.begin(),
                       [](const std::shared_ptr<Point> &p) { return p->Red(); });
        return out;
    }
    void Red(const std::vector<uint16_t> &in)
    {
        if (in.size() != Size())
            throw std::runtime_error("Red setter array must be same size as Points array!");

        for (unsigned i = 0; i < points_.size(); ++i)
            points_[i]->Red(in[i]);
    }

    std::vector<uint16_t> Green() const
    {
        std::vector<uint16_t> out;
        out.resize(Size());
        std::transform(points_.begin(), points_.end(), out.begin(),
                       [](const std::shared_ptr<Point> &p) { return p->Green(); });
        return out;
    }
    void Green(const std::vector<uint16_t> &in)
    {
        if (in.size() != Size())
            throw std::runtime_error("Green setter array must be same size as Points array!");

        for (unsigned i = 0; i < points_.size(); ++i)
            points_[i]->Green(in[i]);
    }

    std::vector<uint16_t> Blue() const
    {
        std::vector<uint16_t> out;
        out.resize(Size());
        std::transform(points_.begin(), points_.end(), out.begin(),
                       [](const std::shared_ptr<Point> &p) { return p->Blue(); });
        return out;
    }
    void Blue(const std::vector<uint16_t> &in)
    {
        if (in.size() != Size())
            throw std::runtime_error("Blue setter array must be same size as Points array!");

        for (unsigned i = 0; i < points_.size(); ++i)
            points_[i]->Blue(in[i]);
    }

    // Function that return true only if all points are within the box
    bool Within(const Box &box) const
    {
        for (const auto &point : points_)
        {
            if (!point->Within(box))
                return false;
        }
        return true;
    }

    // Return sub-set of points that fall within the box
    std::vector<std::shared_ptr<Point>> GetWithin(const Box &box)
    {

        std::vector<std::shared_ptr<Point>> points;

        for (const auto &point : points_)
        {
            if (point->Within(box))
                points.push_back(point);
        }
        return points;
    }

  private:
    std::vector<std::shared_ptr<Point>> points_;
    int8_t point_format_id_;
    uint32_t point_record_length_;
};
} // namespace copc::las
#endif // COPCLIB_LAS_POINTS_H_
