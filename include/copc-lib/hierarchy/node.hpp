#ifndef COPCLIB_HIERARCHY_NODE_H_
#define COPCLIB_HIERARCHY_NODE_H_

#include <sstream>
#include <vector>

#include <copc-lib/hierarchy/entry.hpp>
#include <copc-lib/las/point.hpp>

namespace copc
{

class Page;
class Node : public Entry
{
  public:
    Node(Entry e) : Entry(e){};
    Node() : Entry(){};

    bool operator==(const Node &rhs) { return IsEqual(rhs); }
};

} // namespace copc

#endif // COPCLIB_HIERARCHY_NODE_H_
