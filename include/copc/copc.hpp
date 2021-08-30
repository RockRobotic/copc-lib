#ifndef COPCLIB_COPC_COPC_H_
#define COPCLIB_COPC_COPC_H_

#include <istream>
#include <ostream>

#include "copc/file.hpp"
#include "hierarchy/hierarchy.hpp"
#include "io/reader.hpp"

namespace copc
{

class Copc
{
  public:
    Copc(std::istream &in_stream);
    Copc(const std::string &in_file);

    // Copc (CopcFile &in_file, int span);
    // bool Write(std::ostream *out);

    std::unique_ptr<hierarchy::Hierarchy> hierarchy;
    std::shared_ptr<CopcFile> file;

  private:
    std::shared_ptr<io::Reader> reader;
    std::fstream in_stream;
};
} // namespace copc

#endif // COPCLIB_COPC_COPC_H_