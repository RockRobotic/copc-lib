#ifndef COPCLIB_HIERARCHY_PAGE_H_
#define COPCLIB_HIERARCHY_PAGE_H_

#include <istream>
#include <memory>
#include <utility>

#include "copc-lib/hierarchy/entry.hpp"
#include "copc-lib/hierarchy/node.hpp"

namespace copc
{
class Page;
class Page : public Entry
{
  public:
    Page(Entry e) : Entry(std::move(e)){};
    Page(VoxelKey key, int64_t offset, int32_t byte_size) : Entry(key, offset, byte_size, -1){};

    // If a page is "loaded" it doesn't matter the offset/size
    // (since the writer will default the offset/size to -1)
    bool IsValid() const override { return (loaded || (offset >= 0 && byte_size >= 0)) && key.IsValid(); }
    bool IsPage() const override { return IsValid() && point_count == -1; }

    bool loaded = false;

    friend bool operator==(const Page &lhs, const Page &rhs);
};

} // namespace copc

#endif // COPCLIB_HIERARCHY_PAGE_H_
