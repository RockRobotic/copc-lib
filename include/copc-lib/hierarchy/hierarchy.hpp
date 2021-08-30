#ifndef COPCLIB_HIERARCHY_HIERARCHY_H_
#define COPCLIB_HIERARCHY_HIERARCHY_H_

#include <unordered_map>

#include <copc-lib/hierarchy/key.hpp> // include the key so that the hash function gets in namespace
#include <copc-lib/hierarchy/node.hpp>
#include <copc-lib/hierarchy/page.hpp>
#include <copc-lib/io/reader.hpp>

namespace copc::hierarchy
{

class Copc;
class Hierarchy
{
  public:
    // Hierarchy();
    // Hierarchy(std::vector<Page> pages);

    Hierarchy(std::shared_ptr<io::Reader> reader) : reader_(reader)
    {
        auto copcData = reader->file->GetCopcHeader();
        // add the root page to the pages list
        seen_pages_[VoxelKey(0, 0, 0, 0)] =
            std::make_shared<Page>(VoxelKey(0, 0, 0, 0), copcData.root_hier_offset, copcData.root_hier_size, reader);
    };

    // Find a node object given a key
    std::shared_ptr<Node> FindNode(VoxelKey key);
    // Find the lowest depth page that has been seen within a hierarchy list
    std::shared_ptr<Page> NearestLoadedPage(const std::vector<VoxelKey> &parents_list);

    // bool InsertPage(Page page);
    // std::vector<Page> GetPages();

    // static Hierarchy Unpack(char *in);
    // void Pack(char *out);

  private:
    std::unordered_map<VoxelKey, std::shared_ptr<Page>> seen_pages_;
    std::unordered_map<VoxelKey, std::shared_ptr<Node>> loaded_nodes_;

    std::shared_ptr<io::Reader> reader_;
};

} // namespace copc::hierarchy

#endif // COPCLIB_HIERARCHY_HIERARCHY_H_