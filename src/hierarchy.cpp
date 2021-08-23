#include "hierarchy.hpp"

namespace copc
{

// First entry should be the root entry
Hierarchy::Hierarchy(io::CopcReader *reader)
{
    // Save reader
    reader_ = reader;
}

// Create hierarchy and load up to depth
Hierarchy::Hierarchy(io::CopcReader *reader, const int32_t &depth)
{
    // Save reader
    reader_ = reader;
    // Load all pages up to depth
    LoadPagesToDepth(GetRootPage(), depth);
}

Entry Hierarchy::GetRootPage()
{
    return Entry(VoxelKey(0, 0, 0, 0), reader_->GetCopcHeader().root_hier_offset,
                 reader_->GetCopcHeader().root_hier_size, -1);
}

Entry Hierarchy::GetKey(const VoxelKey &key)
{
    if (loadedEntries_.empty())
        LoadPage(GetRootPage());

    // Check if the entry has already been loaded
    if (loadedEntries_.find(key) != loadedEntries_.end())
    {
        // If entry is subpage then load it before returning
        if (loadedEntries_[key].IsSubpage())
            LoadPage(loadedEntries_[key]);

        return loadedEntries_[key];
    }

    // If key hasn't been loaded already
    // Iterate through parents of that key until finding one that is loaded
    VoxelKey parent_key = key;
    do
    {
        parent_key = GetParent(parent_key);

        // If we've reached the root key then the key doesn't exist
        if (!parent_key.IsValid())
            return {};
    } while (loadedEntries_.find(parent_key) == loadedEntries_.end());

    auto parent_entry = loadedEntries_[parent_key];
    if (!parent_entry.IsSubpage())
        return {};

    // Load the page associated to it
    LoadPage(parent_entry);

    return GetKey(key);
}

VoxelKey Hierarchy::GetParent(const VoxelKey &key)
{

    // If key is valid, return parent, if not, return invalid key
    if (key.IsValid())
        return {key.d - 1, key.x / 2, key.y / 2, key.z / 2};
    else
        return {-1, -1, -1, -1};
}

std::vector<VoxelKey> Hierarchy::GetParents(VoxelKey key, bool include_current)
{
    std::vector<VoxelKey> out;
    if (!key.IsValid())
        return out;

    if (include_current)
        out.push_back(key);

    auto parentKey = Hierarchy::GetParent(key);
    while (parentKey.IsValid())
    {
        out.push_back(parentKey);
        parentKey = Hierarchy::GetParent(parentKey);
    }

    return out;
}

void Hierarchy::LoadPage(const Entry &page_ref)
{
    if (!page_ref.IsSubpage())
        throw runtime_error("Can't load page " + page_ref.ToString() + " because it's not a subpage!");

    std::vector<Entry> page_entries = reader_->ReadPage(page_ref.offset, page_ref.byteSize);
    for (auto page_entry : page_entries)
    {
        loadedEntries_[page_entry.key] = page_entry;
    }
}

int8_t Hierarchy::LoadChildren(const Entry &entry)
{

    // Count of number of children found
    int8_t children_found = 0;
    // For all 8 possible directions
    for (uint64_t dir(0); dir < 8; ++dir)
    {

        // Compute the corresponding key
        VoxelKey child_key = entry.key.Bisect(dir);

        // Look for key in entry Page
        // If the child was found then load it
        auto hier_entry = GetKey(child_key);
        if (hier_entry.IsValid())
        {
            ++children_found;
        }
    }

    return children_found;
}

void Hierarchy::LoadPagesToDepth(const Entry &entry, const int32_t &depth)
{

    if (depth < 0)
        return;
    LoadPage(entry);

    for (auto const &[loaded_key, loaded_entry] : loadedEntries_)
    {
        if (loaded_entry.IsSubpage() && loaded_key.d <= depth)
        {
            LoadPagesToDepth(loaded_entry, depth);
        }
    }
}

} // namespace copc