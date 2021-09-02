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

    static void PackPoints(const std::vector<las::Point> &points, std::ostream &out_stream);
    static std::vector<char> PackPoints(const std::vector<las::Point> &points);
    static std::vector<las::Point> UnpackPoints(const std::vector<char> &in, int point_format_id,
                                                int point_record_length);
};

} // namespace copc::hierarchy

#endif // COPCLIB_HIERARCHY_NODE_H_