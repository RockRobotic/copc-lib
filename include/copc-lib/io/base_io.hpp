#ifndef COPCLIB_IO_BASE_H_
#define COPCLIB_IO_BASE_H_

#include <copc-lib/copc/file.hpp>
#include <copc-lib/hierarchy/internal/hierarchy.hpp>
#include <copc-lib/hierarchy/node.hpp>
#include <copc-lib/hierarchy/page.hpp>
#include <copc-lib/las/vlr.hpp>

namespace copc
{
class BaseIO
{
  public:
    // Find a node object given a key
    Node FindNode(VoxelKey key);

    std::unique_ptr<CopcFile> file;

  protected:
    std::unique_ptr<Hierarchy> hierarchy;
    virtual std::vector<Entry> ReadPage(std::shared_ptr<Page> page){};
};
} // namespace copc
#endif // COPCLIB_IO_BASE_H_