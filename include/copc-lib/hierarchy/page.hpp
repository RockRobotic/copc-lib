#ifndef COPCLIB_HIERARCHY_PAGE_H_
#define COPCLIB_HIERARCHY_PAGE_H_

#include <istream>
#include <memory>

#include <copc-lib/hierarchy/entry.hpp>
#include <copc-lib/hierarchy/node.hpp>

namespace copc
{
const int ENTRY_SIZE = 32;

class Page;
class Page : public Entry
{
  public:
    Page(Entry e) : Entry(e){};
    Page(VoxelKey key, int64_t offset, int32_t size) : Entry(key, offset, size, -1){};

    bool loaded = false;
};

} // namespace copc::hierarchy

#endif // COPCLIB_HIERARCHY_PAGE_H_