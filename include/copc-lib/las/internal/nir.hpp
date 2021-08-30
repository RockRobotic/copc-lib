#ifndef COPCLIB_LAS_INTERNAL_NIR_H_
#define COPCLIB_LAS_INTERNAL_NIR_H_

#include "utils.hpp"

namespace copc::las::internal
{
class Nir
{
  public:
    Nir() : value_(0) {}

    Nir(const uint16_t &v) : value_(v) {}

    Nir(std::istream &in_stream) { Unpack(in_stream); }

    uint16_t Value() const { return value_; }
    void Value(const uint16_t &value) { value_ = value; }

    void Pack(std::ostream &out_stream) { internal::pack(value_, out_stream); }

    void Unpack(std::istream &in_stream) { value_ = internal::unpack<uint16_t>(in_stream); }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "NIR: " << value_;
        return ss.str();
    }

  private:
    uint16_t value_;
};
} // namespace copc::las::internal

#endif // COPCLIB_LAS_INTERNAL_NIR_H_
