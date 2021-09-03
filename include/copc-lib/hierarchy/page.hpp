#ifndef COPCLIB_HIERARCHY_PAGE_H_
#define COPCLIB_HIERARCHY_PAGE_H_

#include <istream>
#include <memory>

#include <copc-lib/hierarchy/entry.hpp>
#include <copc-lib/hierarchy/node.hpp>
#include <copc-lib/io/reader.hpp>

namespace copc::hierarchy
{
const int ENTRY_SIZE = 32;

class Hierarchy;
class Page : public Entry
{
  public:
    Page() : Entry(){};
    // Page(std::vector<Node> nodes);
    Page(VoxelKey key, int64_t offset, int32_t size, std::shared_ptr<io::Reader> reader)
        : Entry(key, offset, size, -1), reader_(reader)
    {
        // All entries are 32 bytes, so our page size must be a multiple of 32
        if (size % ENTRY_SIZE != 0)
            throw std::runtime_error("invalid page size!");
    };

    bool InsertNode(Node node);

    void Load();
    void Pack(char *out);

    std::unordered_map<VoxelKey, std::shared_ptr<Page>> sub_pages;
    std::unordered_map<VoxelKey, std::shared_ptr<Node>> nodes;

    bool loaded = false;

  private:
    void LoadChildren();

    std::unordered_map<VoxelKey, Node> children_;
    std::shared_ptr<io::Reader> reader_;
};

} // namespace copc::hierarchy

#endif // COPCLIB_HIERARCHY_PAGE_H_
