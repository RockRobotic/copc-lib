#include <cstring>
#include <fstream>
#include <sstream>

#include "catch2/catch.hpp"
#include <copc-lib/io/reader.hpp>

using namespace copc;
using namespace std;

TEST_CASE("Packing Test", "[Hierarchy] ")
{
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
        Reader reader(&in_stream);

        auto key = VoxelKey(0, 0, 0, 0);
        auto hier_entry = reader.FindNode(key);

        // Load las::Points
        auto points = reader.GetPoints(hier_entry);

        // Pack loaded points into stream
        ostringstream oss;
        Node::PackPoints(points, oss);

        // Write stream into vector<char> to compare with point_data
        vector<char> point_data_write;
        string s = oss.str();
        point_data_write.insert(point_data_write.end(), s.begin(), s.end());

        // Get point data
        auto point_data_read = reader.GetPointData(hier_entry);

        REQUIRE(point_data_read == point_data_write);
    }
}

//
// TEST_CASE("LoadChildren Checks", "[Hierarchy]")
//{
//    fstream in_stream;
//    in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
//    io::CopcReader reader(in_stream);
//
//    auto copc = reader.GetCopcHeader();
//
//    auto rootPage = reader.ReadPage(copc.root_hier_offset, copc.root_hier_size);
//
//    Hierarchy hier = Hierarchy(&reader);
//
//    auto hier_entry = hier.GetKey(VoxelKey(0, 0, 0, 0));
//    REQUIRE(hier.LoadChildren(hier_entry) == 4);
//    hier_entry = hier.GetKey(VoxelKey(1, 0, 0, 0));
//    REQUIRE(hier.LoadChildren(hier_entry) == 4);
//    hier_entry = hier.GetKey(VoxelKey(4, 11, 9, 0));
//    REQUIRE(hier.LoadChildren(hier_entry) == 0);
//    hier_entry = hier.GetKey(VoxelKey(-1, -1, -1, -1));
//    REQUIRE(hier.LoadChildren(hier_entry) == 0);
//}
