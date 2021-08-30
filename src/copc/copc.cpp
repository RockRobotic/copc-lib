#include "copc/copc.hpp"
#include <fstream>

namespace copc
{

Copc::Copc(std::istream &in_stream)
{
    this->reader = std::make_unique<io::Reader>(in_stream);
    this->file = this->reader->file;
    this->hierarchy = std::make_unique<hierarchy::Hierarchy>(this->reader);
}

Copc::Copc(const std::string &in_file)
{
    in_stream.open(in_file, std::ios::in | std::ios::binary);

    if (!in_stream.good())
        throw std::runtime_error("Error opening input file!");

    this->reader = std::make_unique<io::Reader>(in_stream);
    this->file = this->reader->file;
    this->hierarchy = std::make_unique<hierarchy::Hierarchy>(this->reader);
}

} // namespace copc