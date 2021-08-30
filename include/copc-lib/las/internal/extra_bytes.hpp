#ifndef COPCLIB_LAS_INTERNAL_EXTRA_BYTE_H_
#define COPCLIB_LAS_INTERNAL_EXTRA_BYTE_H_

#include "utils.hpp"

namespace copc::las::internal
{
class ExtraByte
{
  public:
    ExtraByte() : value_(0) {}

    ExtraByte(const uint8_t &v) : value_(v) {}

    ExtraByte(std::istream &in_stream) { Unpack(in_stream); }

    uint16_t Value() const { return value_; }
    void Value(const uint8_t &value) { value_ = value; }

    void Pack(std::ostream &out_stream) { internal::pack(value_, out_stream); }

    void Unpack(std::istream &in_stream) { value_ = internal::unpack<uint8_t>(in_stream); }

  private:
    uint8_t value_;
};
} // namespace copc::las::internal

#endif // COPCLIB_LAS_INTERNAL_EXTRA_BYTE_H_
