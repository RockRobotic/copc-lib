#ifndef COPCLIB_HIERARCHY_PAGE_INTERNAL_H_
#define COPCLIB_HIERARCHY_PAGE_INTERNAL_H_

#include <copc-lib/hierarchy/page.hpp>

namespace copc::Internal
{
class PageInternal;
// This internal class allows for tracking of a page's subpages and nodes without exposing them to the user
class PageInternal : public Page
{
  public:
    PageInternal(Entry e) : Page(e){};
    PageInternal(VoxelKey key, int64_t offset, int32_t byte_size) : Page(key, offset, byte_size){};
    PageInternal(VoxelKey key) : Page(key, -1, -1){};

    std::vector<std::shared_ptr<PageInternal>> sub_pages;
    std::vector<std::shared_ptr<Node>> nodes;
};

} // namespace copc::Internal

#endif // COPCLIB_HIERARCHY_PAGE_INTERNAL_H_
