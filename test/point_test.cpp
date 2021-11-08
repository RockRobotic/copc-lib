#include <limits>
#include <sstream>

#include <catch2/catch.hpp>
#include <copc-lib/geometry/box.hpp>
#include <copc-lib/geometry/vector3.hpp>
#include <copc-lib/las/point.hpp>

using namespace copc::las;
using namespace std;
using Catch::Matchers::WithinAbs;

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
        point6.UnscaledX(std::numeric_limits<int32_t>::max());
        point6.UnscaledY(std::numeric_limits<int32_t>::max());
        point6.UnscaledZ(std::numeric_limits<int32_t>::max());
        REQUIRE(point6.UnscaledX() == std::numeric_limits<int32_t>::max());
        REQUIRE(point6.UnscaledY() == std::numeric_limits<int32_t>::max());
        REQUIRE(point6.UnscaledZ() == std::numeric_limits<int32_t>::max());

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

        point.UnscaledX(4);
        REQUIRE(point != point_other);
        point_other.UnscaledX(4);
        REQUIRE(point == point_other);

        point.UnscaledY(4);
        REQUIRE(point != point_other);
        point_other.UnscaledY(4);
        REQUIRE(point == point_other);

        point.UnscaledZ(4);
        REQUIRE(point != point_other);
        point_other.UnscaledZ(4);
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
        REQUIRE(point.ExtraBytes().size() == 0);
        REQUIRE(point.EbByteSize() == 0);

        point = Point(6, {}, {}, 5);
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
        auto point = Point(8, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), 2);

        point.UnscaledX(4);
        point.UnscaledY(4);
        point.UnscaledZ(4);

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
        auto orig_point =
            Point(6, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), 2); // use two extra bytes
        orig_point.UnscaledX(20);
        orig_point.UnscaledY(-20);
        orig_point.UnscaledZ(100000);
        orig_point.ScanAngle(2000);

        // Format 6
        auto point = orig_point;
        point.Pack(ss);
        auto point_other =
            *Point::Unpack(ss, 6, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), point.EbByteSize());
        REQUIRE(point == point_other);

        // Format 7
        point.ToPointFormat(7);
        point.Pack(ss);
        point_other =
            *Point::Unpack(ss, 7, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), point.EbByteSize());
        REQUIRE(point == point_other);

        // Format 8
        point.ToPointFormat(8);
        point.Pack(ss);
        point_other =
            *Point::Unpack(ss, 8, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), point.EbByteSize());
        REQUIRE(point == point_other);
    }

    SECTION("Scaled XYZ")
    {
        int8_t pfid = 8;

        SECTION("No scale and offset")
        {
            auto point = Point(pfid, {1, 1, 1}, {0, 0, 0});

            point.UnscaledX(4);
            point.UnscaledY(4);
            point.UnscaledZ(4);

            REQUIRE(point.X() == 4);
            REQUIRE(point.Y() == 4);
            REQUIRE(point.Z() == 4);

            point.X(5);
            point.Y(6);
            point.Z(7);

            REQUIRE(point.UnscaledX() == 5);
            REQUIRE(point.UnscaledY() == 6);
            REQUIRE(point.UnscaledZ() == 7);
        }
        SECTION("Scale test")
        {
            auto point = Point(pfid, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset());

            point.UnscaledX(1);
            point.UnscaledY(2);
            point.UnscaledZ(3);

            REQUIRE(point.X() == 0.01);
            REQUIRE(point.Y() == 0.02);
            REQUIRE(point.Z() == 0.03);

            point.X(200);
            point.Y(300);
            point.Z(400);

            REQUIRE(point.UnscaledX() == 200 * 100);
            REQUIRE(point.UnscaledY() == 300 * 100);
            REQUIRE(point.UnscaledZ() == 400 * 100);
        }
        SECTION("Offset test")
        {
            copc::Vector3 scale = {1, 1, 1};
            copc::Vector3 offset = {50001.456, 4443.123, -255.001};
            auto point = Point(pfid, scale, offset);

            point.UnscaledX(0);
            point.UnscaledY(-800);
            point.UnscaledZ(3);

            REQUIRE(point.X() == 0 + offset.x);
            REQUIRE(point.Y() == -800 + offset.y);
            REQUIRE(point.Z() == 3 + offset.z);

            point.X(50502.888);
            point.Y(4002.111);
            point.Z(-80.5);

            // static_cast<T>(std::round((value - offset) / scale));
            REQUIRE(point.UnscaledX() == 501); // 50502.888 - 50001.456 = 501.432
            REQUIRE(point.UnscaledY() == -441);
            REQUIRE(point.UnscaledZ() == 175); // -80.5 - -255.001 = 255.001 - 80.5 = 175

            // (value * scale) + offset
            REQUIRE_THAT(point.X(), WithinAbs(50502.456, 0.000001));
            REQUIRE_THAT(point.Y(), WithinAbs(4002.123, 0.000001));
            REQUIRE_THAT(point.Z(), WithinAbs(-80.001, 0.000001));
        }
        SECTION("Scale and Offset test")
        {
            auto point = Point(pfid, {0.001, 0.001, 0.001}, {50001.456, 4443.123, -255.001});

            point.UnscaledX(0);
            point.UnscaledY(-800);
            point.UnscaledZ(300532);

            REQUIRE_THAT(point.X(), WithinAbs(50001.456, 0.000001));
            REQUIRE_THAT(point.Y(), WithinAbs(4442.323, 0.000001));
            REQUIRE_THAT(point.Z(), WithinAbs(45.531, 0.000001));

            point.X(50502.888);
            point.Y(4002.111);
            point.Z(-80.5);

            // static_cast<T>(std::round((value - offset) / scale));
            REQUIRE(point.UnscaledX() == 501432);
            REQUIRE(point.UnscaledY() == -441012);
            REQUIRE(point.UnscaledZ() == 174501);

            // (value * scale) + offset
            REQUIRE_THAT(point.X(), WithinAbs(50502.888, 0.000001));
            REQUIRE_THAT(point.Y(), WithinAbs(4002.111, 0.000001));
            REQUIRE_THAT(point.Z(), WithinAbs(-80.5, 0.000001));
        }
        SECTION("Precision checks")
        {
            {
                auto point = Point(pfid, {0.000001, 0.000001, 0.000001}, {0, 0, 0});

                REQUIRE_THROWS(point.X(50501132.888123));
            }
            {
                auto point = Point(pfid, {1, 1, 1}, {-8001100065, 0, 0});
                REQUIRE_THROWS(point.X(0));
            }
        }
    }
    SECTION("Within")
    {
        auto point = Point(6, {1, 1, 1}, copc::Vector3::DefaultOffset());

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
