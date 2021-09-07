#ifndef COPCLIB_LAS_POINTS_H_
#define COPCLIB_LAS_POINTS_H_

#include <copc-lib/las/point.hpp>
#include <vector>

namespace copc::las
{
class Points
{
  public:
    Points(){};
    Points(const uint32_t &number_of_points, const int8_t &point_format_id){

    };

    void ToPointFormat(const int8_t &point_format_id)
    {
        if (point_format_id < 0 || point_format_id > 10)
            throw std::runtime_error("Point format must be 0-10");
        for (auto point : points_)
            point.ToPointFormat(point_format_id_)
    }

  private:
    std::vector<Points> points_;
    int8_t point_format_id_;
};
} // namespace copc::las
#endif // COPCLIB_LAS_POINTS_H_
