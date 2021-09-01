#include <copc-lib/copc/file.hpp>

namespace copc
{

CopcFile::CopcFile(las::LasHeader header, int span, std::string wkt) : LasFile(header)
{
    las::CopcVlr copc_header;
    copc_header.span = span;
    this->copc = copc_header;

    this->wkt = wkt;
}

} // namespace copc