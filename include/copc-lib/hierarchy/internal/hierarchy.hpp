#ifndef COPCLIB_HIERARCHY_HIERARCHY_H_
#define COPCLIB_HIERARCHY_HIERARCHY_H_

#include <unordered_map>

#include <copc-lib/hierarchy/internal/page.hpp>
#include <copc-lib/hierarchy/key.hpp> // include the key so that the hash function gets in namespace
#include <copc-lib/hierarchy/node.hpp>

namespace copc
{
class Hierarchy
{
  public:
    Hierarchy()
    {
        auto page = std::make_shared<PageInternal>(VoxelKey::BaseKey(), -1, -1);
        page->loaded = true;
        // add the root page to the pages list
        seen_pages_[VoxelKey::BaseKey()] = page;
    }
    Hierarchy(int64_t root_hier_offset, int32_t root_hier_size)
    {
        // add the root page to the pages list
        seen_pages_[VoxelKey::BaseKey()] =
            std::make_shared<PageInternal>(VoxelKey::BaseKey(), root_hier_offset, root_hier_size);
    };

    // Find the lowest depth page that has been seen within a hierarchy list
    std::shared_ptr<PageInternal> NearestLoadedPage(const std::vector<VoxelKey> &parents_list)
    {
        // Iterates through the list of key ancestors to find if any have been seen
        for (auto &nearest_seen_parent : parents_list)
        {
            if (seen_pages_.find(nearest_seen_parent) != seen_pages_.end())
            {
                return seen_pages_[nearest_seen_parent];
            }
        }
        return nullptr;
    }

    std::unordered_map<VoxelKey, std::shared_ptr<PageInternal>> seen_pages_;
    std::unordered_map<VoxelKey, std::shared_ptr<Node>> loaded_nodes_;
};

} // namespace copc

#endif // COPCLIB_HIERARCHY_HIERARCHY_H_