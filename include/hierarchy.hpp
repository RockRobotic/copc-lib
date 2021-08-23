
#ifndef COPCLIB_COPC_HIERARCHY_H_
#define COPCLIB_COPC_HIERARCHY_H_

#include "reader.hpp"
#include "structs.hpp"
#include <optional>
#include <unordered_map>
#include <vector>

namespace copc
{

class Hierarchy
{
  public:
    // First entry_ should be the root entry
    explicit Hierarchy(io::CopcReader *reader);

    // Create hierarchy and load up to depth
    Hierarchy(io::CopcReader *reader, const int32_t &depth);

    // Finds the key in the copc hierarchy and returns its entry and page reference
    Entry GetKey(const VoxelKey &key);

    // Get parent of any key based on d-x-y-z combination
    static VoxelKey GetParent(const VoxelKey &key);
    static std::vector<VoxelKey> GetParents(VoxelKey key, bool include_current);

    // Attempts to load all 8 children of an entry and return the number of children found
    int8_t LoadChildren(const Entry &entry);

    // Load page corresponding to entry
    void LoadPage(const Entry &page_ref);

    // Load all nodes in hierarchy down to specified depth
    void LoadPagesToDepth(const Entry &entry, const int32_t &depth);

    // Load all nodes in hierarchy up to specified pointcount
    // TODO[LEO]: Implement this
    void LoadToPointCount(const int32_t &point_count);

    // Returns the Entry corresponding to root Page
    Entry GetRootPage();

  private:
    // Loaded entries accessible by VoxelKey
    std::unordered_map<VoxelKey, Entry> loadedEntries_;
    // Pointer to the reader to read COPC pages
    io::CopcReader *reader_;
};

} // namespace copc
#endif // COPCLIB_COPC_HIERARCHY_H_