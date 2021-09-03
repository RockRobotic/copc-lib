#ifndef COPCLIB_HIERARCHY_NODE_H_
#define COPCLIB_HIERARCHY_NODE_H_

#include <sstream>
#include <vector>

#include <copc-lib/hierarchy/entry.hpp>
#include <copc-lib/hierarchy/key.hpp>
#include <copc-lib/io/reader.hpp>
#include <copc-lib/las/point.hpp>

namespace copc::hierarchy
{

class Page;
class Node : public Entry
{
  public:
    Node() : Entry(){};
    // Node(int point_format_id);
    // Node(std::vector<copc::las::Point> points);
    // Node(int point_format_id, char *compressedPoints);
    Node(VoxelKey key, uint64_t offset, int32_t size, int32_t point_count, std::shared_ptr<io::Reader> reader)
        : Entry(key, offset, size, point_count), reader_(reader){};

    static void PackPoints(const std::vector<las::Point> &points, std::ostream &out_stream);

    // Reads the node's data into an uncompressed byte array
    std::vector<char> GetPointData();

    std::vector<las::Point> GetPoints();

  private:
    std::shared_ptr<io::Reader> reader_;
};

} // namespace copc::hierarchy

#endif // COPCLIB_HIERARCHY_NODE_H_
