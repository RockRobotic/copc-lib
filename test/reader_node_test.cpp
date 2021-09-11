#include <catch2/catch.hpp>
#include <cfloat>
#include <copc-lib/io/reader.hpp>
#include <cstring>
#include <fstream>

using namespace copc;
using namespace std;

TEST_CASE("GetPointData Test", "[Reader] ")
{
    GIVEN("A valid input stream")
    {
        FileReader reader("test/data/autzen-classified.copc.laz");

        int record_length = reader.GetLasHeader().point_record_length;

        {
            auto key = VoxelKey(0, 0, 0, 0);
            auto hier_entry = reader.FindNode(key);

            auto point_vec = reader.GetPointData(hier_entry);
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

        {
            auto key = VoxelKey(1, 0, 0, 0);
            auto hier_entry = reader.FindNode(key);

            auto point_vec = reader.GetPointData(hier_entry);
            REQUIRE(!point_vec.empty());
            auto point_buff = point_vec.data();
        }
    }
}

TEST_CASE("GetPointDataCompressed Test", "[Reader] ")
{
    GIVEN("A valid input stream")
    {
        FileReader reader("test/data/autzen-classified.copc.laz");

        auto key = VoxelKey(0, 0, 0, 0);
        auto hier_entry = reader.FindNode(key);

        auto point_vec = reader.GetPointDataCompressed(hier_entry);
        REQUIRE(point_vec.size() > 0);
    }
}

TEST_CASE("GetPoints Test", "[Reader] ")
{
    GIVEN("A valid input stream")
    {
        FileReader reader("test/data/autzen-classified.copc.laz");

        auto key = VoxelKey(0, 0, 0, 0);
        auto hier_entry = reader.FindNode(key);

        auto points = reader.GetPoints(hier_entry).Get();

        REQUIRE(points.size() == hier_entry.point_count);

        // Getters
        REQUIRE(points[0].X() == -144147);
        REQUIRE(points[0].Y() == -13541);
        REQUIRE(points[0].Z() == -227681);
        REQUIRE(points[0].Intensity() == 11);
        REQUIRE(points[0].NumberOfReturns() == 3);
        REQUIRE(points[0].ReturnNumber() == 1);
        REQUIRE(points[0].ScanDirectionFlag() == true);
        REQUIRE(points[0].EdgeOfFlightLineFlag() == false);
        REQUIRE(points[0].Classification() == 5);
        REQUIRE(points[0].ScanAngleRank() == 5);
        REQUIRE(points[0].ScanAngle() == 5.0);
        REQUIRE(points[0].UserData() == 124);
        REQUIRE(points[0].PointSourceID() == 7330);
        REQUIRE(points[0].GPSTime() > 247570);
        REQUIRE(points[0].GPSTime() < 247570.5);
        REQUIRE(points[0].Red() == 46);
        REQUIRE(points[0].Green() == 60);
        REQUIRE(points[0].Blue() == 92);
        REQUIRE(points[0].PointFormatID() == 3);
        REQUIRE(points[0].PointRecordLength() == 36);
        REQUIRE(points[0].ExtraBytes().size() == 2);
        REQUIRE_THROWS(points[0].NIR());
        REQUIRE_THROWS(points[0].ExtendedFlagsBitFields());
        REQUIRE_THROWS(points[0].ExtendedReturnsBitFields());
        REQUIRE_THROWS(points[0].ScannerChannel());
        REQUIRE_THROWS(points[0].ExtendedScanAngle());

        // Setters
        points[0].X(INT32_MAX);
        points[0].Y(INT32_MAX);
        points[0].Z(INT32_MAX);
        points[0].Intensity(UINT16_MAX);
        points[0].NumberOfReturns(7);
        points[0].ReturnNumber(7);
        points[0].ScanDirectionFlag(false);
        points[0].EdgeOfFlightLineFlag(true);
        points[0].Classification(31);
        points[0].ScanAngleRank(90);
        points[0].UserData(UINT8_MAX);
        points[0].PointSourceID(UINT8_MAX);
        points[0].GPSTime(DBL_MAX);
        points[0].Red(UINT16_MAX);
        points[0].Green(UINT16_MAX);
        points[0].Blue(UINT16_MAX);
        REQUIRE_THROWS(points[0].NIR(UINT16_MAX));
        REQUIRE_THROWS(points[0].ExtendedFlagsBitFields(UINT8_MAX));
        REQUIRE_THROWS(points[0].ExtendedReturnsBitFields(UINT8_MAX));
        REQUIRE_THROWS(points[0].ScannerChannel(3));
        REQUIRE_THROWS(points[0].ScanAngle(INT16_MAX));

        REQUIRE(points[0].X() == INT32_MAX);
        REQUIRE(points[0].Y() == INT32_MAX);
        REQUIRE(points[0].Z() == INT32_MAX);
        REQUIRE(points[0].Intensity() == UINT16_MAX);
        REQUIRE(points[0].NumberOfReturns() == 7);
        REQUIRE(points[0].ReturnNumber() == 7);
        REQUIRE(points[0].ScanDirectionFlag() == false);
        REQUIRE(points[0].EdgeOfFlightLineFlag() == true);
        REQUIRE(points[0].Classification() == 31);
        REQUIRE(points[0].ScanAngleRank() == 90);
        REQUIRE(points[0].UserData() == UINT8_MAX);
        REQUIRE(points[0].PointSourceID() == UINT8_MAX);
        REQUIRE(points[0].GPSTime() == DBL_MAX);
        REQUIRE(points[0].Red() == UINT16_MAX);
        REQUIRE(points[0].Green() == UINT16_MAX);
        REQUIRE(points[0].Blue() == UINT16_MAX);
    }
}
