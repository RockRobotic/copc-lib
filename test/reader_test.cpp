#include <catch2/catch.hpp>
#include <copc-lib/copc/file.hpp>
#include <copc-lib/io/reader.hpp>
#include <cstring>
#include <fstream>
#include <cfloat>

using namespace copc;
using namespace std;

TEST_CASE("Reader tests", "[Reader]")
{
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);

        Reader reader(in_stream);

        SECTION("GetCopc Test")
        {
            auto copc = reader.file->GetCopc();
            REQUIRE(copc.span == 0);
            REQUIRE(copc.root_hier_offset == 93169718);
            REQUIRE(copc.root_hier_size == 8896);
            REQUIRE(copc.laz_vlr_offset == 643);
            REQUIRE(copc.laz_vlr_size == 58);
            REQUIRE(copc.wkt_vlr_offset == 755);
            REQUIRE(copc.wkt_vlr_size == 993);
            REQUIRE(copc.eb_vlr_offset == 1802);
            REQUIRE(copc.eb_vlr_size == 384);
        }

        SECTION("GetHeader Test")
        {
            auto header = reader.file->GetLasHeader();
            REQUIRE(header.header_size == 375);
            REQUIRE(header.point_format_id == 3);
            REQUIRE(header.point_count == 10653336);
        }

        SECTION("WKT")
        {
            auto wkt = reader.file->GetWkt();
            REQUIRE(wkt.size() > 0);
            REQUIRE(wkt.rfind("COMPD_CS[", 0) == 0);
        }
    }
}

TEST_CASE("FindKey Check", "[Reader]")
{
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
        Reader reader(in_stream);

        auto key = VoxelKey(0, 0, 0, 0);
        auto hier_entry = reader.FindNode(key);

        REQUIRE(hier_entry->IsValid() == true);
        REQUIRE(hier_entry->key == key);
        REQUIRE(hier_entry->point_count == 61022);

        key = VoxelKey(5, 9, 7, 0);
        hier_entry = reader.FindNode(key);

        REQUIRE(hier_entry->IsValid() == true);
        REQUIRE(hier_entry->key == key);
        REQUIRE(hier_entry->point_count == 12019);
    }
}

TEST_CASE("GetPointData Test", "[Reader] ")
{
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
        Reader reader(in_stream);

        int record_length = reader.file->GetLasHeader().point_record_length;

        auto key = VoxelKey(0, 0, 0, 0);
        auto hier_entry = reader.FindNode(key);

        auto point_vec = reader.GetPointData(*hier_entry);
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

//TEST_CASE("GetPointDataCompressed Test", "[Hierarchy] ")
//{
//    GIVEN("A valid input stream")
//    {
//        fstream in_stream;
//        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
//        Reader reader(in_stream);
//
//        auto key = VoxelKey(0, 0, 0, 0);
//        auto hier_entry = reader.FindNode(key);
//
//        auto point_vec = hier_entry->GetPointDataCompressed();
//        REQUIRE(point_vec.size() > 0);
//    }
//}

TEST_CASE("GetPoints Test", "[Reader] ")
{
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
        Reader reader(in_stream);

        auto key = VoxelKey(0, 0, 0, 0);
        auto hier_entry = reader.FindNode(key);

        auto points = reader.GetPoints(*hier_entry);

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