#ifndef COPCLIB_IO_COPC_BASE_IO_H_
#define COPCLIB_IO_COPC_BASE_IO_H_

#include "copc-lib/copc/copc_config.hpp"
#include "copc-lib/copc/extents.hpp"
#include "copc-lib/copc/info.hpp"
#include "copc-lib/hierarchy/node.hpp"
#include "copc-lib/hierarchy/page.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc
{
namespace Internal
{
class Hierarchy;
class PageInternal;
} // namespace Internal

struct CopcConfig;

// Base class for all IO reader/writers
class BaseIO
{
  public:
    // Find a node object given a key
    Node FindNode(VoxelKey key);

  protected:
    std::shared_ptr<Internal::Hierarchy> hierarchy_;
    virtual std::vector<Entry> ReadPage(std::shared_ptr<Internal::PageInternal> page) = 0;
    void ReadAndParsePage(const std::shared_ptr<Internal::PageInternal> &page);
    // Recursively reads all subpages and nodes given a root and returns all the nodes that were loaded
    void LoadPageHierarchy(const std::shared_ptr<Internal::PageInternal> &page, std::vector<Node> &loaded_nodes);
};

} // namespace copc
#endif // COPCLIB_IO_COPC_BASE_IO_H_
