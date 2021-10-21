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
        FileReader reader("autzen-classified.copc.laz");

        int record_length = reader.CopcConfig().LasHeader().PointRecordLength();

        {
            auto key = VoxelKey(0, 0, 0, 0);
            auto hier_entry = reader.FindNode(key);

            auto point_vec = reader.GetPointData(hier_entry);
            REQUIRE(!point_vec.empty());
            auto point_buff = point_vec.data();

            int x;
            std::memcpy(&x, point_buff, sizeof(x));
            REQUIRE(x == -113822);
            std::memcpy(&x, point_buff + record_length, sizeof(x));
            REQUIRE(x == -215872);
            std::memcpy(&x, point_buff + record_length * 2, sizeof(x));
            REQUIRE(x == -94105);
            std::memcpy(&x, point_buff + record_length * 3, sizeof(x));
            REQUIRE(x == -141210);
            std::memcpy(&x, point_buff + record_length * 4, sizeof(x));
            REQUIRE(x == -156093);
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
        FileReader reader("autzen-classified.copc.laz");

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
        FileReader reader("autzen-classified.copc.laz");

        auto key = VoxelKey(0, 0, 0, 0);
        auto hier_entry = reader.FindNode(key);

        auto points = reader.GetPoints(hier_entry).Get();

        REQUIRE(points.size() == hier_entry.point_count);

        // Getters
        REQUIRE(points[0]->UnscaledX() == -113822);
        REQUIRE(points[0]->UnscaledY() == -118589);
        REQUIRE(points[0]->UnscaledZ() == -221965);
        REQUIRE(points[0]->Intensity() == 11);
        REQUIRE(points[0]->NumberOfReturns() == 3);
        REQUIRE(points[0]->ReturnNumber() == 1);
        REQUIRE(points[0]->ScanDirectionFlag() == true);
        REQUIRE(points[0]->EdgeOfFlightLineFlag() == false);
        REQUIRE(points[0]->Classification() == 5);
        REQUIRE(points[0]->ScanAngle() == 1667);
        REQUIRE_THAT(points[0]->ScanAngleFloat(), Catch::Matchers::WithinAbs(10, 0.01));
        REQUIRE(points[0]->UserData() == 124);
        REQUIRE(points[0]->PointSourceID() == 7327);
        REQUIRE_THAT(points[0]->GPSTime(), Catch::Matchers::WithinAbs(246098.8141472744, 0.0001));
        REQUIRE(points[0]->Red() == 16896);
        REQUIRE(points[0]->Green() == 65280);
        REQUIRE(points[0]->Blue() == 1536);
        REQUIRE(points[0]->PointFormatID() == 7);
        REQUIRE(points[0]->PointRecordLength() == 36);
        REQUIRE(points[0]->ExtraBytes().empty());
        REQUIRE_THROWS(points[0]->NIR());

        // Setters
        points[0]->UnscaledX(std::numeric_limits<int32_t>::max());
        points[0]->UnscaledY(std::numeric_limits<int32_t>::max());
        points[0]->UnscaledZ(std::numeric_limits<int32_t>::max());
        points[0]->Intensity(std::numeric_limits<uint16_t>::max());
        points[0]->NumberOfReturns(7);
        points[0]->ReturnNumber(7);
        points[0]->ScanDirectionFlag(false);
        points[0]->EdgeOfFlightLineFlag(true);
        points[0]->Classification(31);
        points[0]->ScanAngle(90);
        points[0]->UserData(std::numeric_limits<int8_t>::max());
        points[0]->PointSourceID(std::numeric_limits<int8_t>::max());
        points[0]->GPSTime(std::numeric_limits<double>::max());
        points[0]->Red(std::numeric_limits<uint16_t>::max());
        points[0]->Green(std::numeric_limits<uint16_t>::max());
        points[0]->Blue(std::numeric_limits<uint16_t>::max());
        REQUIRE_THROWS(points[0]->NIR(std::numeric_limits<uint16_t>::max()));

        REQUIRE(points[0]->UnscaledX() == std::numeric_limits<int32_t>::max());
        REQUIRE(points[0]->UnscaledY() == std::numeric_limits<int32_t>::max());
        REQUIRE(points[0]->UnscaledZ() == std::numeric_limits<int32_t>::max());
        REQUIRE(points[0]->Intensity() == std::numeric_limits<uint16_t>::max());
        REQUIRE(points[0]->NumberOfReturns() == 7);
        REQUIRE(points[0]->ReturnNumber() == 7);
        REQUIRE(points[0]->ScanDirectionFlag() == false);
        REQUIRE(points[0]->EdgeOfFlightLineFlag() == true);
        REQUIRE(points[0]->Classification() == 31);
        REQUIRE(points[0]->ScanAngle() == 90);
        REQUIRE(points[0]->UserData() == std::numeric_limits<int8_t>::max());
        REQUIRE(points[0]->PointSourceID() == std::numeric_limits<int8_t>::max());
        REQUIRE(points[0]->GPSTime() == std::numeric_limits<double>::max());
        REQUIRE(points[0]->Red() == std::numeric_limits<uint16_t>::max());
        REQUIRE(points[0]->Green() == std::numeric_limits<uint16_t>::max());
        REQUIRE(points[0]->Blue() == std::numeric_limits<uint16_t>::max());
    }
}
