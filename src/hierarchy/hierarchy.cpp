#include "hierarchy/hierarchy.hpp"

namespace copc::hierarchy
{
std::shared_ptr<Node> Hierarchy::FindNode(VoxelKey key)
{
    // Check if the entry has already been loaded
    if (loaded_nodes_.find(key) != loaded_nodes_.end())
    {
        return loaded_nodes_[key];
    }

    // Get a list of the key's hierarchial parents, so we can see if any of them are loaded
    auto parents_list = key.GetParents(true);

    // Find if of the key's ancestors have been seen
    std::shared_ptr<Page> nearest_page = NearestLoadedPage(parents_list);
    // If none of the key's ancestors exist, then this key doesn't exist in the hierarchy
    // Or, if the nearest ancestor has already been loaded, that means the key isn't a node within that page.
    if (nearest_page == nullptr || nearest_page->loaded)
        return nullptr;

    // Load the page and add the subpages and page nodes
    nearest_page->Load();
    for (auto &[key, node] : nearest_page->nodes)
    {
        loaded_nodes_[key] = node;
    }
    for (auto &[key, page] : nearest_page->sub_pages)
    {
        seen_pages_[key] = page;
    }

    // Try to find the key again
    return FindNode(key);
}

std::shared_ptr<Page> Hierarchy::NearestLoadedPage(const std::vector<VoxelKey> &parents_list)
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
} // namespace copc::hierarchy
