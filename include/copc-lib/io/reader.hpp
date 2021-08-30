#ifndef COPCLIB_IO_READER_H_
#define COPCLIB_IO_READER_H_

#include <istream>

#include <copc-lib/copc/file.hpp>

namespace copc::io
{
class Reader
{
  public:
    Reader(std::istream &in_stream);

    std::shared_ptr<CopcFile> file;
    std::istream &in_stream;
};
} // namespace copc::io
#endif // COPCLIB_IO_READER_H_