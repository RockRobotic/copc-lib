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

    // Provides helper functions for handling lists of Point objects
    static void PackPoints(const std::vector<las::Point> &points, std::ostream &out_stream);
    static std::vector<char> PackPoints(const std::vector<las::Point> &points);
    static std::vector<las::Point> UnpackPoints(const std::vector<char> &point_data, int8_t point_format_id,
                                                int point_record_length);

    bool operator==(const Node &rhs) { return IsEqual(rhs); }
};

} // namespace copc

#endif // COPCLIB_HIERARCHY_NODE_H_
