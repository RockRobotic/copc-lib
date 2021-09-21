#include <cstring>
#include <fstream>
#include <limits>

#include <catch2/catch.hpp>
#include <copc-lib/io/reader.hpp>

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
        REQUIRE(points[0].UnscaledX() == -144147);
        REQUIRE(points[0].UnscaledY() == -13541);
        REQUIRE(points[0].UnscaledZ() == -227681);
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
        points[0].UnscaledX(std::numeric_limits<int>::max());
        points[0].UnscaledY(std::numeric_limits<int>::max());
        points[0].UnscaledZ(std::numeric_limits<int>::max());
        points[0].Intensity(std::numeric_limits<unsigned short>::max());
        points[0].NumberOfReturns(7);
        points[0].ReturnNumber(7);
        points[0].ScanDirectionFlag(false);
        points[0].EdgeOfFlightLineFlag(true);
        points[0].Classification(31);
        points[0].ScanAngleRank(90);
        points[0].UserData(std::numeric_limits<char>::max());
        points[0].PointSourceID(std::numeric_limits<char>::max());
        points[0].GPSTime(std::numeric_limits<double>::max());
        points[0].Red(std::numeric_limits<unsigned short>::max());
        points[0].Green(std::numeric_limits<unsigned short>::max());
        points[0].Blue(std::numeric_limits<unsigned short>::max());
        REQUIRE_THROWS(points[0].NIR(std::numeric_limits<unsigned short>::max()));
        REQUIRE_THROWS(points[0].ExtendedFlagsBitFields(std::numeric_limits<char>::max()));
        REQUIRE_THROWS(points[0].ExtendedReturnsBitFields(std::numeric_limits<char>::max()));
        REQUIRE_THROWS(points[0].ScannerChannel(3));
        REQUIRE_THROWS(points[0].ScanAngle(std::numeric_limits<short>::max()));

        REQUIRE(points[0].UnscaledX() == std::numeric_limits<int>::max());
        REQUIRE(points[0].UnscaledY() == std::numeric_limits<int>::max());
        REQUIRE(points[0].UnscaledZ() == std::numeric_limits<int>::max());
        REQUIRE(points[0].Intensity() == std::numeric_limits<unsigned short>::max());
        REQUIRE(points[0].NumberOfReturns() == 7);
        REQUIRE(points[0].ReturnNumber() == 7);
        REQUIRE(points[0].ScanDirectionFlag() == false);
        REQUIRE(points[0].EdgeOfFlightLineFlag() == true);
        REQUIRE(points[0].Classification() == 31);
        REQUIRE(points[0].ScanAngleRank() == 90);
        REQUIRE(points[0].UserData() == std::numeric_limits<char>::max());
        REQUIRE(points[0].PointSourceID() == std::numeric_limits<char>::max());
        REQUIRE(points[0].GPSTime() == std::numeric_limits<double>::max());
        REQUIRE(points[0].Red() == std::numeric_limits<unsigned short>::max());
        REQUIRE(points[0].Green() == std::numeric_limits<unsigned short>::max());
        REQUIRE(points[0].Blue() == std::numeric_limits<unsigned short>::max());
    }
}
