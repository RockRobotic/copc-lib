#ifndef COPCLIB_LAS_INTERNAL_RGB_H_
#define COPCLIB_LAS_INTERNAL_RGB_H_

#include "utils.hpp"

namespace copc::las::internal
{
struct Rgb
{
  public:
    Rgb() : r_(0), g_(0), b_(0) {}
    Rgb(const uint16_t &r, const uint16_t &g, const uint16_t &b) : r_(r), g_(g), b_(b) {}
    Rgb(std::istream &in_stream) { Unpack(in_stream); }

    void R(const uint16_t &r) { r_ = r; }
    uint16_t R() const { return r_; }
    void G(const uint16_t &g) { g_ = g; }
    uint16_t G() const { return g_; }
    void B(const uint16_t &b) { b_ = b; }
    uint16_t B() const { return b_; }

    void SetRgb(const uint16_t &r, const uint16_t &g, const uint16_t &b)
    {
        r_ = r;
        g_ = g;
        b_ = b;
    }

    void Unpack(std::istream &in_stream)
    {
        r_ = internal::unpack<uint16_t>(in_stream);
        g_ = internal::unpack<uint16_t>(in_stream);
        b_ = internal::unpack<uint16_t>(in_stream);
    }

    void Pack(std::ostream &out_stream) const
    {
        internal::pack(r_, out_stream);
        internal::pack(g_, out_stream);
        internal::pack(b_, out_stream);
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "RGB: [" << r_ << "," << g_ << "," << b_ << "]";
        return ss.str();
    }

  private:
    uint16_t r_;
    uint16_t g_;
    uint16_t b_;
};
} // namespace copc::las::internal

#endif // COPCLIB_LAS_INTERNAL_RGB_H_
