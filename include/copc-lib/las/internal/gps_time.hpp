#ifndef COPCLIB_LAS_INTERNAL_GPS_TIME_H_
#define COPCLIB_LAS_INTERNAL_GPS_TIME_H_

#include <sstream>

#include "utils.hpp"

namespace copc::las::internal
{
class GpsTime
{
  public:
    GpsTime() : value_(0) {}
    GpsTime(const double &v) : value_(v) {}
    GpsTime(std::istream &in_stream) { Unpack(in_stream); }

    double Value() const { return value_; }
    void Value(const double &value) { value_ = value; }

    void Unpack(std::istream &in_stream) { value_ = internal::unpack<double>(in_stream); }
    void Pack(std::ostream &out_stream) const { internal::pack(value_, out_stream); }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "GPS Time: " << value_;
        return ss.str();
    }

  private:
    double value_;
};
} // namespace copc::las::internal

#endif // COPCLIB_LAS_INTERNAL_GPS_TIME_H_
