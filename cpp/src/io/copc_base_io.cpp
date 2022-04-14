#include "copc-lib/io/copc_base_io.hpp"
#include "copc-lib/hierarchy/internal/hierarchy.hpp"
#include "copc-lib/hierarchy/internal/page.hpp"

namespace copc
{

// Find a node object given a key
Node BaseIO::FindNode(VoxelKey key)
{
    // Check if the entry has already been loaded
    if (hierarchy_->loaded_nodes_.find(key) != hierarchy_->loaded_nodes_.end())
    {
        return *hierarchy_->loaded_nodes_[key];
    }

    // Get a list of the key's hierarchial parents, so we can see if any of them are loaded
    auto parents_list = key.GetParents(true);

    // Find if of the key's ancestors have been seen
    std::shared_ptr<Internal::PageInternal> nearest_page = hierarchy_->NearestLoadedPage(parents_list);
    // If none of the key's ancestors exist, then this key doesn't exist in the hierarchy
    // Or, if the nearest ancestor has already been loaded, that means the key isn't a node within that page.
    if (nearest_page == nullptr || nearest_page->loaded)
        return {};

    // Load the page and add the subpages and page nodes
    ReadAndParsePage(nearest_page);

    // Try to find the key again
    return FindNode(key);
}

void BaseIO::LoadPageHierarchy(const std::shared_ptr<Internal::PageInternal> &page, std::vector<Node> &loaded_nodes)
{
    if (!page->IsValid())
        return;

    if (!page->loaded)
        ReadAndParsePage(page);

    for (const auto &sub_page : page->sub_pages)
    {
        LoadPageHierarchy(sub_page, loaded_nodes);
    }
    for (const auto &node : page->nodes)
    {
        loaded_nodes.push_back(*node.second);
    }
}

void BaseIO::ReadAndParsePage(const std::shared_ptr<Internal::PageInternal> &page)
{
    auto children = ReadPage(page);
    for (const Entry &e : children)
    {
        if (e.IsPage())
        {
            auto subpage = std::make_shared<Internal::PageInternal>(e);
            hierarchy_->seen_pages_[e.key] = subpage;
            page->sub_pages.insert(subpage);
        }
        else
        {
            auto node = std::make_shared<Node>(e, page->key);
            hierarchy_->loaded_nodes_[e.key] = node;
            page->nodes[node->key] = node;
        }
    }
}
} // namespace copc
