#include "io/reader.hpp"
#include "copc/copc.hpp"

namespace copc::io
{
Reader::Reader(std::istream &in_stream) : in_stream(in_stream) { this->file = std::make_shared<CopcFile>(in_stream); }
} // namespace copc::io