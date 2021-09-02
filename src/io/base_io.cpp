#include <copc-lib/io/base_io.hpp>

namespace copc
{

// Find a node object given a key
Node BaseIO::FindNode(VoxelKey key)
{
    // Check if the entry has already been loaded
    if (hierarchy->loaded_nodes_.find(key) != hierarchy->loaded_nodes_.end())
    {
        return *hierarchy->loaded_nodes_[key];
    }

    // Get a list of the key's hierarchial parents, so we can see if any of them are loaded
    auto parents_list = key.GetParents(true);

    // Find if of the key's ancestors have been seen
    std::shared_ptr<Page> nearest_page = hierarchy->NearestLoadedPage(parents_list);
    // If none of the key's ancestors exist, then this key doesn't exist in the hierarchy
    // Or, if the nearest ancestor has already been loaded, that means the key isn't a node within that page.
    if (nearest_page == nullptr || nearest_page->loaded)
        return {};

    // Load the page and add the subpages and page nodes
    auto children = ReadPage(nearest_page);
    for (Entry e : children)
    {
        if (e.IsPage())
            hierarchy->seen_pages_[e.key] = std::make_shared<Page>(e);
        else
            hierarchy->loaded_nodes_[e.key] = std::make_shared<Node>(e);
    }

    // Try to find the key again
    return FindNode(key);
}
} // namespace copc