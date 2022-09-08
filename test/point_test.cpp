#include <limits>
#include <sstream>

#include <catch2/catch.hpp>
#include <copc-lib/geometry/box.hpp>
#include <copc-lib/geometry/vector3.hpp>
#include <copc-lib/las/point.hpp>
#include <copc-lib/las/points.hpp>

using namespace copc::las;
using namespace std;
using Catch::Matchers::WithinAbs;

Point PackAndUnpack(Point p, copc::Vector3 scale, copc::Vector3 offset)
{
    std::stringstream out;
    p.Pack(out, scale, offset);
    return *Point::Unpack(out, p.PointFormatId(), scale, offset, p.EbByteSize());
}

TEST_CASE("Point tests", "[Point]")
{
    SECTION("Point Format Initialization")
    {

        auto point = Point(6);

        REQUIRE(point.HasRgb() == false);
        REQUIRE(point.HasNir() == false);

        auto point_ext = Point(8);

        REQUIRE(point_ext.HasRgb() == true);
        REQUIRE(point_ext.HasNir() == true);
    }

    SECTION("Point Test")
    {
        auto point6 = Point(6);
        // Position
        point6.X(0.05);
        point6.Y(1.11);
        point6.Z(-9.99);
        REQUIRE_THAT(point6.X(), WithinAbs(0.05, 0.000001));
        REQUIRE_THAT(point6.Y(), WithinAbs(1.11, 0.000001));
        REQUIRE_THAT(point6.Z(), WithinAbs(-9.99, 0.000001));

        // Intensity
        point6.Intensity(std::numeric_limits<uint16_t>::max());
        REQUIRE(point6.Intensity() == std::numeric_limits<uint16_t>::max());

        // Return BitField
        point6.ReturnsBitField(std::numeric_limits<uint8_t>::max());
        REQUIRE(point6.ReturnsBitField() == std::numeric_limits<uint8_t>::max());

        // Return Number
        point6.ReturnNumber(0);
        REQUIRE(point6.ReturnNumber() == 0);
        point6.ReturnNumber(15);
        REQUIRE(point6.ReturnNumber() == 15);
        REQUIRE_THROWS(point6.ReturnNumber(16));

        // Number return
        point6.NumberOfReturns(0);
        REQUIRE(point6.NumberOfReturns() == 0);
        point6.NumberOfReturns(15);
        REQUIRE(point6.NumberOfReturns() == 15);
        REQUIRE_THROWS(point6.NumberOfReturns(16));

        // Flags
        point6.FlagsBitField(std::numeric_limits<uint8_t>::max());
        REQUIRE(point6.FlagsBitField() == std::numeric_limits<uint8_t>::max());

        // Synthetic
        point6.Synthetic(false);
        REQUIRE(point6.Synthetic() == false);
        point6.Synthetic(true);
        REQUIRE(point6.Synthetic() == true);

        // KeyPoint
        point6.Withheld(false);
        REQUIRE(point6.Withheld() == false);
        point6.Withheld(true);
        REQUIRE(point6.Withheld() == true);

        // Withheld
        point6.Withheld(false);
        REQUIRE(point6.Withheld() == false);
        point6.Withheld(true);
        REQUIRE(point6.Withheld() == true);

        // Overlap
        point6.Overlap(false);
        REQUIRE(point6.Overlap() == false);
        point6.Overlap(true);
        REQUIRE(point6.Overlap() == true);

        // Scanner Channel
        point6.ScannerChannel(0);
        REQUIRE(point6.ScannerChannel() == 0);
        point6.ScannerChannel(3);
        REQUIRE(point6.ScannerChannel() == 3);
        REQUIRE_THROWS(point6.ScannerChannel(4));

        // Scan Direction
        point6.ScanDirectionFlag(true);
        REQUIRE(point6.ScanDirectionFlag() == true);
        point6.ScanDirectionFlag(false);
        REQUIRE(point6.ScanDirectionFlag() == false);

        // Edge of Flight Line
        point6.EdgeOfFlightLineFlag(true);
        REQUIRE(point6.EdgeOfFlightLineFlag() == true);
        point6.EdgeOfFlightLineFlag(false);
        REQUIRE(point6.EdgeOfFlightLineFlag() == false);

        // Classification
        point6.Classification(std::numeric_limits<uint8_t>::max());
        REQUIRE(point6.Classification() == std::numeric_limits<uint8_t>::max());
        point6.Classification(0);
        REQUIRE(point6.Classification() == 0);

        // Scan Angle
        point6.ScanAngle(-30000);
        REQUIRE(point6.ScanAngle() == -30000);
        REQUIRE(point6.ScanAngleDegrees() == -180.0);
        point6.ScanAngle(30000);
        REQUIRE(point6.ScanAngle() == 30000);
        REQUIRE(point6.ScanAngleDegrees() == 180.0);

        // User Data
        point6.UserData(std::numeric_limits<uint8_t>::max());
        REQUIRE(point6.UserData() == std::numeric_limits<uint8_t>::max());

        // Point Source ID
        point6.PointSourceId(std::numeric_limits<uint16_t>::max());
        REQUIRE(point6.PointSourceId() == std::numeric_limits<uint16_t>::max());

        // GPS Time
        point6.GPSTime(std::numeric_limits<double>::max());
        REQUIRE(point6.GPSTime() == std::numeric_limits<double>::max());

        // Point Record Length
        REQUIRE(point6.PointRecordLength() == 30);

        // Checks
        REQUIRE_THROWS(point6.Rgb(std::numeric_limits<uint16_t>::max(), std::numeric_limits<uint16_t>::max(),
                                  std::numeric_limits<uint16_t>::max()));
        REQUIRE_THROWS(point6.Red());
        REQUIRE_THROWS(point6.Green());
        REQUIRE_THROWS(point6.Blue());
        REQUIRE_THROWS(point6.Nir(std::numeric_limits<uint16_t>::max()));
        REQUIRE_THROWS(point6.Nir());
        REQUIRE_NOTHROW(point6.ToString());

        auto point7 = Point(7);

        point7.Rgb(std::numeric_limits<uint16_t>::max(), std::numeric_limits<uint16_t>::max(),
                   std::numeric_limits<uint16_t>::max());
        REQUIRE(point7.Red() == std::numeric_limits<uint16_t>::max());
        REQUIRE(point7.Green() == std::numeric_limits<uint16_t>::max());
        REQUIRE(point7.Blue() == std::numeric_limits<uint16_t>::max());
        REQUIRE(point7.PointRecordLength() == 36);

        REQUIRE_THROWS(point7.Nir(std::numeric_limits<uint16_t>::max()));
        REQUIRE_THROWS(point7.Nir());

        auto point8 = Point(8);

        point8.Nir(std::numeric_limits<uint16_t>::max());
        REQUIRE(point8.Nir() == std::numeric_limits<uint16_t>::max());
        REQUIRE(point8.PointRecordLength() == 38);
    }

    SECTION("Point format conversion")
    {
        auto point = Point(6);

        REQUIRE(point.GPSTime() == 0);
        REQUIRE(point.ScannerChannel() == 0);
        REQUIRE(point.Overlap() == 0);
        REQUIRE(point.GPSTime() == 0);

        point.ToPointFormat(7);
        REQUIRE(point.GPSTime() == 0);
        REQUIRE(point.ScannerChannel() == 0);
        REQUIRE(point.Overlap() == 0);
        REQUIRE(point.GPSTime() == 0);
        REQUIRE(point.Red() == 0);
        REQUIRE(point.Green() == 0);
        REQUIRE(point.Blue() == 0);

        point.ToPointFormat(8);
        REQUIRE(point.GPSTime() == 0);
        REQUIRE(point.ScannerChannel() == 0);
        REQUIRE(point.Overlap() == 0);
        REQUIRE(point.GPSTime() == 0);
        REQUIRE(point.Red() == 0);
        REQUIRE(point.Green() == 0);
        REQUIRE(point.Blue() == 0);
        REQUIRE(point.Nir() == 0);
    }

    SECTION("Point conversion")
    {
        auto point = Point(6);

        point.ReturnNumber(5);
        point.NumberOfReturns(6);
        point.ScanDirectionFlag(false);
        point.EdgeOfFlightLineFlag(true);
        point.Classification(15);
        point.Synthetic(true);
        point.KeyPoint(false);
        point.Withheld(true);
        point.ScanAngleDegrees(45);

        point.ToPointFormat(7);

        REQUIRE(point.ReturnNumber() == 5);
        REQUIRE(point.NumberOfReturns() == 6);
        REQUIRE(point.ScanDirectionFlag() == false);
        REQUIRE(point.EdgeOfFlightLineFlag() == true);
        REQUIRE(point.Classification() == 15);
        REQUIRE(point.Synthetic() == true);
        REQUIRE(point.KeyPoint() == false);
        REQUIRE(point.Withheld() == true);
        REQUIRE(point.Overlap() == false);
        REQUIRE(point.ScanAngleDegrees() == 45);
        REQUIRE(point.ScannerChannel() == 0);
        REQUIRE(point.Red() == 0);
        REQUIRE(point.Green() == 0);
        REQUIRE(point.Blue() == 0);
        REQUIRE_THROWS(point.Nir());

        point.Red(150);
        point.Green(200);
        point.Blue(250);

        point.ToPointFormat(8);

        REQUIRE(point.ReturnNumber() == 5);
        REQUIRE(point.NumberOfReturns() == 6);
        REQUIRE(point.ScanDirectionFlag() == false);
        REQUIRE(point.EdgeOfFlightLineFlag() == true);
        REQUIRE(point.Classification() == 15);
        REQUIRE(point.Synthetic() == true);
        REQUIRE(point.KeyPoint() == false);
        REQUIRE(point.Withheld() == true);
        REQUIRE(point.Overlap() == false);
        REQUIRE(point.ScanAngleDegrees() == 45);
        REQUIRE(point.ScannerChannel() == 0);
        REQUIRE(point.Red() == 150);
        REQUIRE(point.Green() == 200);
        REQUIRE(point.Blue() == 250);
        REQUIRE(point.Nir() == 0);
    }

    SECTION("Operator == and !=")
    {

        // Format 6
        auto point = Point(6);
        auto point_other = Point(6);
        REQUIRE(point == point_other);

        // Format 7
        point_other.ToPointFormat(7);
        REQUIRE(point != point_other);
        point.ToPointFormat(7);
        REQUIRE(point == point_other);

        // Format 8
        point_other.ToPointFormat(8);
        REQUIRE(point != point_other);
        point.ToPointFormat(8);
        REQUIRE(point == point_other);

        // Atributes

        point.X(4);
        REQUIRE(point != point_other);
        point_other.X(4);
        REQUIRE(point == point_other);

        point.Y(4);
        REQUIRE(point != point_other);
        point_other.Y(4);
        REQUIRE(point == point_other);

        point.Z(4);
        REQUIRE(point != point_other);
        point_other.Z(4);
        REQUIRE(point == point_other);

        point.Intensity(4);
        REQUIRE(point != point_other);
        point_other.Intensity(4);
        REQUIRE(point == point_other);

        point.ReturnNumber(4);
        REQUIRE(point != point_other);
        point_other.ReturnNumber(4);
        REQUIRE(point == point_other);

        point.NumberOfReturns(4);
        REQUIRE(point != point_other);
        point_other.NumberOfReturns(4);
        REQUIRE(point == point_other);

        point.Classification(4);
        REQUIRE(point != point_other);
        point_other.Classification(4);
        REQUIRE(point == point_other);

        point.ScanDirectionFlag(true);
        REQUIRE(point != point_other);
        point_other.ScanDirectionFlag(true);
        REQUIRE(point == point_other);

        point.EdgeOfFlightLineFlag(true);
        REQUIRE(point != point_other);
        point_other.EdgeOfFlightLineFlag(true);
        REQUIRE(point == point_other);

        point.Synthetic(true);
        REQUIRE(point != point_other);
        point_other.Synthetic(true);
        REQUIRE(point == point_other);

        point.KeyPoint(true);
        REQUIRE(point != point_other);
        point_other.KeyPoint(true);
        REQUIRE(point == point_other);

        point.Withheld(true);
        REQUIRE(point != point_other);
        point_other.Withheld(true);
        REQUIRE(point == point_other);

        point.Overlap(true);
        REQUIRE(point != point_other);
        point_other.Overlap(true);
        REQUIRE(point == point_other);

        point.ScannerChannel(2);
        REQUIRE(point != point_other);
        point_other.ScannerChannel(2);
        REQUIRE(point == point_other);

        point.UserData(4);
        REQUIRE(point != point_other);
        point_other.UserData(4);
        REQUIRE(point == point_other);

        point.PointSourceId(4);
        REQUIRE(point != point_other);
        point_other.PointSourceId(4);
        REQUIRE(point == point_other);

        point.GPSTime(4.0);
        REQUIRE(point != point_other);
        point_other.GPSTime(4.0);
        REQUIRE(point == point_other);

        point.Red(4);
        REQUIRE(point != point_other);
        point_other.Red(4);
        REQUIRE(point == point_other);

        point.Green(4);
        REQUIRE(point != point_other);
        point_other.Green(4);
        REQUIRE(point == point_other);

        point.Blue(4);
        REQUIRE(point != point_other);
        point_other.Blue(4);
        REQUIRE(point == point_other);

        point.Nir(4);
        REQUIRE(point != point_other);
        point_other.Nir(4);
        REQUIRE(point == point_other);
    }

    SECTION("Point ExtraByte")
    {
        auto point = Point(6);
        REQUIRE(point.PointFormatId() == 6);
        REQUIRE(point.PointRecordLength() == 30);
        REQUIRE_THROWS(point.ExtraBytes(std::vector<uint8_t>{2, 3, 4}));
        REQUIRE(point.ExtraBytes().empty());
        REQUIRE(point.EbByteSize() == 0);

        point = Point(6, 5);
        REQUIRE(point.PointFormatId() == 6);
        REQUIRE(point.PointRecordLength() == 30 + 5);
        REQUIRE(point.EbByteSize() == 5);
        REQUIRE(point.ExtraBytes().size() == 5);
        REQUIRE_THROWS(point.ExtraBytes(std::vector<uint8_t>{2, 3, 4}));
        REQUIRE_NOTHROW(point.ExtraBytes(std::vector<uint8_t>{2, 3, 4, 5, 6}));
        REQUIRE(point.ExtraBytes() == std::vector<uint8_t>{2, 3, 4, 5, 6});
    }

    SECTION("Operator =")
    {
        auto point = Point(8, 2);

        point.X(4);
        point.Y(4);
        point.Z(4);

        point.GPSTime(4.0);
        point.Red(4.0);
        point.Nir(4.0);

        point.ExtraBytes(std::vector<uint8_t>{2, 5});

        auto point_other = Point(6);
        point_other = point;

        REQUIRE(point == point_other);
    }

    SECTION("Packing and Unpacking")
    {

        std::stringstream ss;
        auto scale = copc::Vector3::DefaultScale();
        auto offset = copc::Vector3::DefaultOffset();
        auto orig_point = Point(6, 2); // use two extra bytes
        orig_point.X(20);
        orig_point.Y(-20);
        orig_point.Z(100000);
        orig_point.ScanAngle(2000);

        // Format 6
        auto point = orig_point;
        point.Pack(ss, scale, offset);
        auto point_other =
            *Point::Unpack(ss, 6, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), point.EbByteSize());
        REQUIRE(point == point_other);

        // Format 7
        point.ToPointFormat(7);
        point.Pack(ss, scale, offset);
        point_other =
            *Point::Unpack(ss, 7, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), point.EbByteSize());
        REQUIRE(point == point_other);

        // Format 8
        point.ToPointFormat(8);
        point.Pack(ss, scale, offset);
        point_other =
            *Point::Unpack(ss, 8, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), point.EbByteSize());
        REQUIRE(point == point_other);
    }

    SECTION("Scaled XYZ")
    {
        int8_t pfid = 8;

        SECTION("No scale and offset")
        {
            auto point = Point(pfid);

            point.X(4);
            point.Y(4.5);
            point.Z(-0.1);

            auto test_point = PackAndUnpack(point, copc::Vector3(1, 1, 1), copc::Vector3(0, 0, 0));

            REQUIRE(test_point.X() == 4);
            REQUIRE(test_point.Y() == 5);
            REQUIRE(test_point.Z() == 0);
        }
        SECTION("Scale test")
        {
            auto point = Point(pfid);

            point.X(0.01);
            point.Y(0.02);
            point.Z(0.03);

            auto test_point = PackAndUnpack(point, copc::Vector3(1, 1, 1), copc::Vector3(0, 0, 0));

            REQUIRE(test_point.X() == 0);
            REQUIRE(test_point.Y() == 0);
            REQUIRE(test_point.Z() == 0);

            test_point = PackAndUnpack(point, copc::Vector3(0.01, 0.01, 0.01), copc::Vector3(0, 0, 0));

            REQUIRE(test_point.X() == 0.01);
            REQUIRE(test_point.Y() == 0.02);
            REQUIRE(test_point.Z() == 0.03);
        }
        SECTION("Offset test")
        {
            copc::Vector3 scale = {1, 1, 1};
            copc::Vector3 offset = {50001.456, 4443.123, -255.001};
            auto point = Point(pfid);

            point.X(50502.888);
            point.Y(4002.111);
            point.Z(-80.5);

            auto test_point = PackAndUnpack(point, scale, offset);
            REQUIRE_THAT(test_point.X(), WithinAbs(50502.456, 0.000001));
            REQUIRE_THAT(test_point.Y(), WithinAbs(4002.123, 0.000001));
            REQUIRE_THAT(test_point.Z(), WithinAbs(-80.001, 0.000001));
        }
        SECTION("Scale and Offset test")
        {
            copc::Vector3 scale = {0.001, 0.001, 0.001};
            copc::Vector3 offset = {50001.456, 4443.123, -255.001};
            auto point = Point(pfid);

            point.X(50502.888);
            point.Y(4002.111);
            point.Z(-80.5);

            auto test_point = PackAndUnpack(point, scale, offset);
            REQUIRE_THAT(test_point.X(), WithinAbs(50502.888, 0.000001));
            REQUIRE_THAT(test_point.Y(), WithinAbs(4002.111, 0.000001));
            REQUIRE_THAT(test_point.Z(), WithinAbs(-80.5, 0.000001));
        }
        SECTION("Change Scale test")
        {
            copc::Vector3 offset = {50001.456, 4443.123, -255.001};
            copc::Vector3 scale = {0.001, 0.001, 0.001};
            copc::Vector3 new_scale = {1, 1, 1};
            // This tests the case that we want to change the point's scale when we pack it
            auto point = Point(pfid);

            point.X(50502.888);
            point.Y(4002.111);
            point.Z(-80.5);

            // (value * scale) + offset
            REQUIRE_THAT(point.X(), WithinAbs(50502.888, 0.000001));
            REQUIRE_THAT(point.Y(), WithinAbs(4002.111, 0.000001));
            REQUIRE_THAT(point.Z(), WithinAbs(-80.5, 0.000001));

            // When we pack the points, that's when we unscale the internally stored double.
            // so we end up with:
            // UnscaledX = static_cast<T>(std::round((50502.888 - 50001.456) / 1)) = 501
            // UnscaledY = static_cast<T>(std::round((4002.111 - 4443.123) / 1)) = -441
            // UnscaledZ = static_cast<T>(std::round((-80.5 - -255.001) / 1)) = 175

            // Then when we unpack the points, we do the inverse:
            // (value * scale) + offset
            // X = 501 * 1 + 50001.456 = 50502.456
            // Y = -441 * 1 + 4443.123 = 4002.123
            // Z = 175 * 1 + -255.001 = -80.001
            auto test_point = PackAndUnpack(point, new_scale, offset);

            REQUIRE_THAT(test_point.X(), WithinAbs(50502.456, 0.0001));
            REQUIRE_THAT(test_point.Y(), WithinAbs(4002.123, 0.0001));
            REQUIRE_THAT(test_point.Z(), WithinAbs(-80.001, 0.0001));
        }
        // todo: add change offset and change offset/scale tests
        SECTION("Precision checks")
        {
            {
                auto point = Point(pfid);
                point.X(50501132.888123);
                std::stringstream out;
                REQUIRE_THROWS(point.Pack(out, {0.000001, 0.000001, 0.000001}, {0, 0, 0}));
            }
            {
                auto point = Point(pfid);
                point.X(0);
                std::stringstream out;
                REQUIRE_THROWS(point.Pack(out, {1, 1, 1}, {-8001100065, 0, 0}));
            }
        }
    }
    SECTION("Within")
    {
        auto point = Point(6);

        point.X(5);
        point.Y(5);
        point.Z(5);

        REQUIRE(point.Within(copc::Box::MaxBox()));
        REQUIRE(!point.Within(copc::Box::EmptyBox()));

        // 2D box
        REQUIRE(point.Within(copc::Box(0, 0, 5, 5)));
        REQUIRE(!point.Within(copc::Box(6, 0, 10, 5)));

        // 3D box
        REQUIRE(point.Within(copc::Box(0, 0, 0, 5, 5, 5)));
        REQUIRE(!point.Within(copc::Box(6, 0, 0, 10, 5, 5)));
    }
}
