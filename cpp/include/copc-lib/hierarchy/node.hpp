#ifndef COPCLIB_HIERARCHY_NODE_H_
#define COPCLIB_HIERARCHY_NODE_H_

#include <sstream>
#include <vector>

#include "copc-lib/hierarchy/entry.hpp"
#include "copc-lib/las/point.hpp"

namespace copc
{

class Page;
class Node : public Entry
{
  public:
    Node(const Entry &e, const VoxelKey &page_key = VoxelKey::InvalidKey()) : Entry(e), page_key(page_key){};
    Node() : Entry(){};

    bool operator==(const Node &rhs) { return IsEqual(rhs); }
    VoxelKey page_key{};
};

} // namespace copc

#endif // COPCLIB_HIERARCHY_NODE_H_
