#include <cfloat>
#include <cstring>
#include <sstream>

#include "catch2/catch.hpp"
#include <copc-lib/hierarchy/hierarchy.hpp>
#include <copc-lib/io/reader.hpp>

using namespace copc::hierarchy;
using namespace std;

TEST_CASE("Page & Node Checks", "[Hierarchy]")
{
    GIVEN("A valid input stream")
    {
        int64_t root_hier_offset = 93169718;
        int32_t root_hier_size = 8896;

        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
        auto reader = std::make_shared<copc::io::Reader>(in_stream);

        Page page(VoxelKey(0, 0, 0, 0), root_hier_offset, root_hier_size, reader);

        SECTION("Page Loading")
        {
            REQUIRE(page.loaded == false);
            REQUIRE(page.nodes.empty());
            REQUIRE(page.sub_pages.empty());

            page.Load();

            REQUIRE(page.loaded == true);
            REQUIRE(page.nodes.size() == 278);
            REQUIRE(page.sub_pages.empty());

            auto node = page.nodes[VoxelKey(0, 0, 0, 0)];
            REQUIRE(node->point_count == 61022);

            node = page.nodes[VoxelKey(5, 9, 7, 0)];
            REQUIRE(node->point_count == 12019);
        }
    }
}

TEST_CASE("FindKey Check", "[Hierarchy]")
{
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
        auto reader = std::make_shared<copc::io::Reader>(in_stream);

        Hierarchy h(reader);

        auto key = VoxelKey(0, 0, 0, 0);
        auto hier_entry = h.FindNode(key);

        REQUIRE(hier_entry->IsValid() == true);
        REQUIRE(hier_entry->key == key);
        REQUIRE(hier_entry->point_count == 61022);

        key = VoxelKey(5, 9, 7, 0);
        hier_entry = h.FindNode(key);

        REQUIRE(hier_entry->IsValid() == true);
        REQUIRE(hier_entry->key == key);
        REQUIRE(hier_entry->point_count == 12019);
    }
}

TEST_CASE("GetPointData Test", "[Hierarchy] ")
{
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
        auto reader = std::make_shared<copc::io::Reader>(in_stream);

        int record_length = reader->file->GetLasHeader().point_record_length;

        Hierarchy h(reader);

        auto key = VoxelKey(0, 0, 0, 0);
        auto hier_entry = h.FindNode(key);

        auto point_vec = hier_entry->GetPointData();
        REQUIRE(!point_vec.empty());
        auto point_buff = point_vec.data();

        int x;
        std::memcpy(&x, point_buff, sizeof(x));
        REQUIRE(x == -144147);
        std::memcpy(&x, point_buff + record_length, sizeof(x));
        REQUIRE(x == -172520);
        std::memcpy(&x, point_buff + record_length * 2, sizeof(x));
        REQUIRE(x == -121603);
        std::memcpy(&x, point_buff + record_length * 3, sizeof(x));
        REQUIRE(x == -49828);
        std::memcpy(&x, point_buff + record_length * 4, sizeof(x));
        REQUIRE(x == -138082);
    }
}

TEST_CASE("GetPointDataCompressed Test", "[Hierarchy] ")
{
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
        auto reader = std::make_shared<copc::io::Reader>(in_stream);

        Hierarchy h(reader);

        auto key = VoxelKey(0, 0, 0, 0);
        auto hier_entry = h.FindNode(key);

        auto point_vec = hier_entry->GetPointDataCompressed();
        REQUIRE(point_vec.size() > 0);
    }
}

TEST_CASE("GetPoints Test", "[Hierarchy] ")
{
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
        auto reader = std::make_shared<copc::io::Reader>(in_stream);

        Hierarchy h(reader);

        auto key = VoxelKey(0, 0, 0, 0);
        auto hier_entry = h.FindNode(key);

        auto points = hier_entry->GetPoints();

        REQUIRE(points.size() == hier_entry->point_count);

        // Getters
        REQUIRE(points[0].X() == -144147);
        REQUIRE(points[0].Y() == -13541);
        REQUIRE(points[0].Z() == -227681);
        REQUIRE(points[0].Intensity() == 11);
        REQUIRE(points[0].NumberOfReturns() == 3);
        REQUIRE(points[0].ReturnNumber() == 1);
        REQUIRE(points[0].ScanDirFlag() == true);
        REQUIRE(points[0].EdgeOfFlightLineFlag() == false);
        REQUIRE(points[0].Classification() == 5);
        REQUIRE(points[0].ScanAngleRank() == 5);
        REQUIRE(points[0].UserData() == 124);
        REQUIRE(points[0].PointSourceId() == 7330);
        REQUIRE(points[0].GetGpsTimeVal() > 247570);
        REQUIRE(points[0].GetGpsTimeVal() < 247570.5);
        REQUIRE(points[0].R() == 46);
        REQUIRE(points[0].G() == 60);
        REQUIRE(points[0].B() == 92);
        REQUIRE_THROWS(points[0].GetNirVal());
        REQUIRE_THROWS(points[0].FlagsBitFields());
        REQUIRE_THROWS(points[0].ReturnsBitFields());
        REQUIRE_THROWS(points[0].ScannerChannel());
        REQUIRE_THROWS(points[0].ScanAngle());

        // Setters
        points[0].X(INT32_MAX);
        points[0].Y(INT32_MAX);
        points[0].Z(INT32_MAX);
        points[0].Intensity(UINT16_MAX);
        points[0].NumberOfReturns(7);
        points[0].ReturnNumber(7);
        points[0].ScanDirFlag(false);
        points[0].EdgeOfFlightLineFlag(true);
        points[0].Classification(31);
        points[0].ScanAngleRank(90);
        points[0].UserData(UINT8_MAX);
        points[0].PointSourceId(UINT8_MAX);
        points[0].SetGpsTimeVal(DBL_MAX);
        points[0].R(UINT16_MAX);
        points[0].G(UINT16_MAX);
        points[0].B(UINT16_MAX);
        REQUIRE_THROWS(points[0].SetNirVal(UINT16_MAX));
        REQUIRE_THROWS(points[0].FlagsBitFields(UINT8_MAX));
        REQUIRE_THROWS(points[0].ReturnsBitFields(UINT8_MAX));
        REQUIRE_THROWS(points[0].ScannerChannel(3));
        REQUIRE_THROWS(points[0].ScanAngle(INT16_MAX));

        REQUIRE(points[0].X() == INT32_MAX);
        REQUIRE(points[0].Y() == INT32_MAX);
        REQUIRE(points[0].Z() == INT32_MAX);
        REQUIRE(points[0].Intensity() == UINT16_MAX);
        REQUIRE(points[0].NumberOfReturns() == 7);
        REQUIRE(points[0].ReturnNumber() == 7);
        REQUIRE(points[0].ScanDirFlag() == false);
        REQUIRE(points[0].EdgeOfFlightLineFlag() == true);
        REQUIRE(points[0].Classification() == 31);
        REQUIRE(points[0].ScanAngleRank() == 90);
        REQUIRE(points[0].UserData() == UINT8_MAX);
        REQUIRE(points[0].PointSourceId() == UINT8_MAX);
        REQUIRE(points[0].GetGpsTimeVal() == DBL_MAX);
        REQUIRE(points[0].R() == UINT16_MAX);
        REQUIRE(points[0].G() == UINT16_MAX);
        REQUIRE(points[0].B() == UINT16_MAX);
    }
}

TEST_CASE("Packing Test", "[Hierarchy] ")
{
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
        auto reader = std::make_shared<copc::io::Reader>(in_stream);

        Hierarchy h(reader);

        auto key = VoxelKey(0, 0, 0, 0);
        auto hier_entry = h.FindNode(key);

        // Load las::Points
        auto points = hier_entry->GetPoints();

        // Pack loaded points into stream
        ostringstream oss;
        hier_entry->PackPoints(points, oss);

        // Write stream into vector<char> to compare with point_data
        vector<char> point_data_write;
        string s = oss.str();
        point_data_write.insert(point_data_write.end(), s.begin(), s.end());

        // Get point data
        auto point_data_read = hier_entry->GetPointData();

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
//
// TEST_CASE("GetKey Validity Checks", "[Hierarchy]")
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
//    REQUIRE(hier_entry.IsValid() == true);
//    hier_entry = hier.GetKey(VoxelKey(4, 11, 9, 0));
//    REQUIRE(hier_entry.IsValid() == true);
//
//    hier_entry = hier.GetKey(VoxelKey(20, 11, 9, 0));
//    REQUIRE(hier_entry.IsValid() == false);
//    hier_entry = hier.GetKey(VoxelKey(-1, 0, 0, 0));
//    REQUIRE(hier_entry.IsValid() == false);
//}