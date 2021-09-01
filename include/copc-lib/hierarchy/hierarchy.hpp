#ifndef COPCLIB_HIERARCHY_HIERARCHY_H_
#define COPCLIB_HIERARCHY_HIERARCHY_H_

#include <unordered_map>

#include <copc-lib/hierarchy/key.hpp> // include the key so that the hash function gets in namespace
#include <copc-lib/hierarchy/node.hpp>
#include <copc-lib/hierarchy/page.hpp>
#include <copc-lib/io/reader.hpp>
#include <copc-lib/io/writer.hpp>

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
        auto copcData = reader->copc_data;
        int size = copcData.root_hier_size;
        // add the root page to the pages list
        seen_pages_[VoxelKey(0, 0, 0, 0)] =
            std::make_shared<Page>(VoxelKey(0, 0, 0, 0), copcData.root_hier_offset, copcData.root_hier_size, reader);
    };

    Hierarchy(std::shared_ptr<io::Writer> writer) : writer_(writer){};

    // Find a node object given a key
    std::shared_ptr<Node> FindNode(VoxelKey key);
    // Find the lowest depth page that has been seen within a hierarchy list
    std::shared_ptr<Page> NearestLoadedPage(const std::vector<VoxelKey> &parents_list);

    std::shared_ptr<Page> CreateAndInsertPage(VoxelKey key);
    // std::vector<Page> GetPages();

    // static Hierarchy Unpack(char *in);
    // void Pack(char *out);

  private:
    // make this private for now
    bool InsertPage(std::shared_ptr<Page> page);

    std::unordered_map<VoxelKey, std::shared_ptr<Page>> seen_pages_;
    std::unordered_map<VoxelKey, std::shared_ptr<Node>> loaded_nodes_;

    std::shared_ptr<io::Reader> reader_;
    std::shared_ptr<io::Writer> writer_;
};

} // namespace copc::hierarchy

#endif // COPCLIB_HIERARCHY_HIERARCHY_H_