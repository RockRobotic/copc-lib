#ifndef COPCLIB_IO_BASE_H_
#define COPCLIB_IO_BASE_H_

#include <copc-lib/copc/file.hpp>
#include <copc-lib/hierarchy/internal/hierarchy.hpp>
#include <copc-lib/hierarchy/node.hpp>
#include <copc-lib/hierarchy/page.hpp>
#include <copc-lib/las/vlr.hpp>

namespace copc
{
// Base class for all IO reader/writers
class BaseIO
{
  public:
    // Find a node object given a key
    Node FindNode(VoxelKey key);

    std::shared_ptr<CopcFile> file;

  protected:
    std::shared_ptr<Internal::Hierarchy> hierarchy;
    virtual std::vector<Entry> ReadPage(std::shared_ptr<Internal::PageInternal> page) = 0;
};
} // namespace copc
#endif // COPCLIB_IO_BASE_H_
