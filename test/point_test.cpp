#include <limits>
#include <sstream>

#include <catch2/catch.hpp>
#include <copc-lib/hierarchy/key.hpp>
#include <copc-lib/las/header.hpp>
#include <copc-lib/las/point.hpp>

using namespace copc::las;
using namespace std;
using Catch::Matchers::WithinAbs;

TEST_CASE("Point tests", "[Point]")
{
    SECTION("Point Format Initialization")
    {

        auto point = Point(0);

        REQUIRE(point.HasExtendedPoint() == false);
        REQUIRE(point.HasGPSTime() == false);
        REQUIRE(point.HasRGB() == false);
        REQUIRE(point.HasNIR() == false);

        auto point_ext = Point(8);

        REQUIRE(point_ext.HasExtendedPoint() == true);
        REQUIRE(point_ext.HasGPSTime() == true);
        REQUIRE(point_ext.HasRGB() == true);
        REQUIRE(point_ext.HasNIR() == true);
    }

    SECTION("Point with format LAS 1.0 Test")
    {
        auto point0 = Point(0);
        // Position
        point0.UnscaledX(std::numeric_limits<int>::max());
        point0.UnscaledY(std::numeric_limits<int>::max());
        point0.UnscaledZ(std::numeric_limits<int>::max());
        REQUIRE(point0.UnscaledX() == std::numeric_limits<int>::max());
        REQUIRE(point0.UnscaledY() == std::numeric_limits<int>::max());
        REQUIRE(point0.UnscaledZ() == std::numeric_limits<int>::max());

        // Intensity
        point0.Intensity(std::numeric_limits<unsigned short>::max());
        REQUIRE(point0.Intensity() == std::numeric_limits<unsigned short>::max());

        // Return Number
        point0.ReturnNumber(7);
        REQUIRE(point0.ReturnNumber() == 7);
        REQUIRE_THROWS(point0.ReturnNumber(8));

        // Number of Returns
        point0.NumberOfReturns(7);
        REQUIRE(point0.NumberOfReturns() == 7);
        REQUIRE_THROWS(point0.NumberOfReturns(8));

        // Scan Direction
        point0.ScanDirectionFlag(true);
        REQUIRE(point0.ScanDirectionFlag() == true);

        // Edge of Flight Line
        point0.EdgeOfFlightLineFlag(true);
        REQUIRE(point0.EdgeOfFlightLineFlag() == true);

        // ReturnsScanDirEofBitField
        point0.ReturnsScanDirEofBitFields(172);
        REQUIRE(point0.ReturnNumber() == 4);
        REQUIRE(point0.NumberOfReturns() == 5);
        REQUIRE(point0.ScanDirectionFlag() == false);
        REQUIRE(point0.EdgeOfFlightLineFlag() == true);

        // Classification
        point0.Classification(31);
        REQUIRE(point0.Classification() == 31);
        REQUIRE_THROWS(point0.Classification(32));

        // Synthetic
        point0.Synthetic(true);
        REQUIRE(point0.Synthetic() == true);

        // KeyPoint
        point0.KeyPoint(true);
        REQUIRE(point0.KeyPoint() == true);

        // Withheld
        point0.Withheld(true);
        REQUIRE(point0.Withheld() == true);

        // Classification Bitfield
        point0.ClassificationBitFields(std::numeric_limits<unsigned char>::max());
        REQUIRE(point0.ClassificationBitFields() == std::numeric_limits<unsigned char>::max());
        point0.ClassificationBitFields(78);
        REQUIRE(point0.Classification() == 14);
        REQUIRE(point0.Synthetic() == false);
        REQUIRE(point0.KeyPoint() == true);
        REQUIRE(point0.Withheld() == false);

        // Scan Angle
        point0.ScanAngleRank(90);
        REQUIRE(point0.ScanAngleRank() == 90);
        REQUIRE(point0.ScanAngle() == 90.0);
        REQUIRE_THROWS(point0.ScanAngleRank(91));
        REQUIRE_THROWS(point0.ScanAngleRank(-91));

        // User Data
        point0.UserData(std::numeric_limits<unsigned char>::max());
        REQUIRE(point0.UserData() == std::numeric_limits<unsigned char>::max());

        // Point Source ID
        point0.PointSourceID(std::numeric_limits<unsigned short>::max());
        REQUIRE(point0.PointSourceID() == std::numeric_limits<unsigned short>::max());

        REQUIRE(point0.PointRecordLength() == 20);

        // Checks
        REQUIRE_THROWS(point0.ExtendedFlagsBitFields(0));
        REQUIRE_THROWS(point0.ExtendedFlagsBitFields());
        REQUIRE_THROWS(point0.ScannerChannel(0));
        REQUIRE_THROWS(point0.ScannerChannel());
        REQUIRE_THROWS(point0.NIR(std::numeric_limits<unsigned short>::max()));
        REQUIRE_THROWS(point0.NIR());
        REQUIRE_THROWS(point0.Red(std::numeric_limits<unsigned short>::max()));
        REQUIRE_THROWS(point0.Red());
        REQUIRE_THROWS(point0.Green(std::numeric_limits<unsigned short>::max()));
        REQUIRE_THROWS(point0.Green());
        REQUIRE_THROWS(point0.Blue(std::numeric_limits<unsigned short>::max()));
        REQUIRE_THROWS(point0.Blue());
        REQUIRE_THROWS(point0.GPSTime(std::numeric_limits<double>::max()));
        REQUIRE_THROWS(point0.GPSTime());
        REQUIRE_THROWS(point0.ExtendedScanAngle(std::numeric_limits<short>::max()));
        REQUIRE_THROWS(point0.ExtendedScanAngle());
        REQUIRE_THROWS(point0.Overlap(true));
        REQUIRE_THROWS(point0.Overlap());
        REQUIRE_THROWS(point0.ExtendedReturnsBitFields(std::numeric_limits<unsigned char>::max()));
        REQUIRE_THROWS(point0.ExtendedReturnsBitFields());
        REQUIRE_THROWS(point0.ExtendedFlagsBitFields(std::numeric_limits<unsigned char>::max()));
        REQUIRE_THROWS(point0.ExtendedFlagsBitFields());
        REQUIRE_NOTHROW(point0.ToString());

        auto point1 = Point(1);

        point1.GPSTime(std::numeric_limits<double>::max());
        REQUIRE(point1.GPSTime() == std::numeric_limits<double>::max());
        REQUIRE(point1.PointRecordLength() == 28);

        REQUIRE_THROWS(point0.Red(std::numeric_limits<unsigned short>::max()));
        REQUIRE_THROWS(point0.Red());
        REQUIRE_THROWS(point0.Green(std::numeric_limits<unsigned short>::max()));
        REQUIRE_THROWS(point0.Green());
        REQUIRE_THROWS(point0.Blue(std::numeric_limits<unsigned short>::max()));
        REQUIRE_THROWS(point0.Blue());
        REQUIRE_THROWS(point0.NIR(std::numeric_limits<unsigned short>::max()));
        REQUIRE_THROWS(point0.NIR());

        auto point2 = Point(2);
        REQUIRE(point2.PointRecordLength() == 26);

        point2.Red(std::numeric_limits<unsigned short>::max());
        REQUIRE(point2.Red() == std::numeric_limits<unsigned short>::max());
        point2.Green(std::numeric_limits<unsigned short>::max());
        REQUIRE(point2.Green() == std::numeric_limits<unsigned short>::max());
        point2.Blue(std::numeric_limits<unsigned short>::max());
        REQUIRE(point2.Blue() == std::numeric_limits<unsigned short>::max());

        point2.RGB(std::numeric_limits<unsigned short>::max() / 2, std::numeric_limits<unsigned short>::max() / 2,
                   std::numeric_limits<unsigned short>::max() / 2);
        REQUIRE(point2.Red() == std::numeric_limits<unsigned short>::max() / 2);
        REQUIRE(point2.Green() == std::numeric_limits<unsigned short>::max() / 2);
        REQUIRE(point2.Blue() == std::numeric_limits<unsigned short>::max() / 2);

        REQUIRE_THROWS(point2.GPSTime(std::numeric_limits<double>::max()));
        REQUIRE_THROWS(point2.GPSTime());
        REQUIRE_THROWS(point2.NIR(std::numeric_limits<unsigned short>::max()));
        REQUIRE_THROWS(point2.NIR());

        auto point3 = Point(3);
        REQUIRE(point3.PointRecordLength() == 34);

        REQUIRE_NOTHROW(point3.GPSTime(std::numeric_limits<double>::max()));
        REQUIRE_NOTHROW(point3.GPSTime());
        REQUIRE_NOTHROW(point3.RGB(std::numeric_limits<unsigned short>::max(),
                                   std::numeric_limits<unsigned short>::max(),
                                   std::numeric_limits<unsigned short>::max()));
        REQUIRE_NOTHROW(point3.Red());
        REQUIRE_NOTHROW(point3.Green());
        REQUIRE_NOTHROW(point3.Blue());
        REQUIRE_THROWS(point3.NIR(std::numeric_limits<unsigned short>::max()));
        REQUIRE_THROWS(point3.NIR());
    }

    SECTION("Point with format LAS 1.4 Test")
    {
        auto point6 = Point(6);
        // Position
        point6.UnscaledX(std::numeric_limits<int>::max());
        point6.UnscaledY(std::numeric_limits<int>::max());
        point6.UnscaledZ(std::numeric_limits<int>::max());
        REQUIRE(point6.UnscaledX() == std::numeric_limits<int>::max());
        REQUIRE(point6.UnscaledY() == std::numeric_limits<int>::max());
        REQUIRE(point6.UnscaledZ() == std::numeric_limits<int>::max());

        // Intensity
        point6.Intensity(std::numeric_limits<unsigned short>::max());
        REQUIRE(point6.Intensity() == std::numeric_limits<unsigned short>::max());

        // Return BitField
        point6.ExtendedReturnsBitFields(std::numeric_limits<unsigned char>::max());
        REQUIRE(point6.ExtendedReturnsBitFields() == std::numeric_limits<unsigned char>::max());

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
        point6.ExtendedFlagsBitFields(std::numeric_limits<unsigned char>::max());
        REQUIRE(point6.ExtendedFlagsBitFields() == std::numeric_limits<unsigned char>::max());

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
        point6.Classification(std::numeric_limits<unsigned char>::max());
        REQUIRE(point6.Classification() == std::numeric_limits<unsigned char>::max());
        point6.Classification(0);
        REQUIRE(point6.Classification() == 0);

        // Scan Angle
        point6.ExtendedScanAngle(-30000);
        REQUIRE(point6.ExtendedScanAngle() == -30000);
        REQUIRE(point6.ScanAngle() == -180.0);
        REQUIRE_THROWS(point6.ExtendedScanAngle(-30001));
        point6.ExtendedScanAngle(30000);
        REQUIRE(point6.ExtendedScanAngle() == 30000);
        REQUIRE(point6.ScanAngle() == 180.0);
        REQUIRE_THROWS(point6.ExtendedScanAngle(30001));

        // User Data
        point6.UserData(std::numeric_limits<unsigned char>::max());
        REQUIRE(point6.UserData() == std::numeric_limits<unsigned char>::max());

        // Point Source ID
        point6.PointSourceID(std::numeric_limits<unsigned short>::max());
        REQUIRE(point6.PointSourceID() == std::numeric_limits<unsigned short>::max());

        // GPS Time
        point6.GPSTime(std::numeric_limits<double>::max());
        REQUIRE(point6.GPSTime() == std::numeric_limits<double>::max());

        // Point Record Length
        REQUIRE(point6.PointRecordLength() == 30);

        // Checks
        REQUIRE_THROWS(point6.RGB(std::numeric_limits<unsigned short>::max(),
                                  std::numeric_limits<unsigned short>::max(),
                                  std::numeric_limits<unsigned short>::max()));
        REQUIRE_THROWS(point6.Red());
        REQUIRE_THROWS(point6.Green());
        REQUIRE_THROWS(point6.Blue());
        REQUIRE_THROWS(point6.NIR(std::numeric_limits<unsigned short>::max()));
        REQUIRE_THROWS(point6.NIR());
        REQUIRE_THROWS(point6.ScanAngleRank());
        REQUIRE_THROWS(point6.ScanAngleRank(INT8_MAX));
        REQUIRE_THROWS(point6.ReturnsScanDirEofBitFields(std::numeric_limits<unsigned char>::max()));
        REQUIRE_THROWS(point6.ReturnsScanDirEofBitFields());
        REQUIRE_THROWS(point6.ClassificationBitFields(std::numeric_limits<unsigned char>::max()));
        REQUIRE_THROWS(point6.ClassificationBitFields());
        REQUIRE_NOTHROW(point6.ToString());

        auto point7 = Point(7);

        point7.RGB(std::numeric_limits<unsigned short>::max(), std::numeric_limits<unsigned short>::max(),
                   std::numeric_limits<unsigned short>::max());
        REQUIRE(point7.Red() == std::numeric_limits<unsigned short>::max());
        REQUIRE(point7.Green() == std::numeric_limits<unsigned short>::max());
        REQUIRE(point7.Blue() == std::numeric_limits<unsigned short>::max());
        REQUIRE(point7.PointRecordLength() == 36);

        REQUIRE_THROWS(point7.NIR(std::numeric_limits<unsigned short>::max()));
        REQUIRE_THROWS(point7.NIR());

        auto point8 = Point(8);

        point8.NIR(std::numeric_limits<unsigned short>::max());
        REQUIRE(point8.NIR() == std::numeric_limits<unsigned short>::max());
        REQUIRE(point8.PointRecordLength() == 38);
    }

    SECTION("Point conversion Point10")
    {
        auto point = Point(1);

        REQUIRE(point.GPSTime() == 0);

        point.ToPointFormat(2);
        REQUIRE(point.Red() == 0);
        REQUIRE(point.Green() == 0);
        REQUIRE(point.Blue() == 0);

        point.ToPointFormat(3);
        REQUIRE(point.GPSTime() == 0);
        REQUIRE(point.Red() == 0);
        REQUIRE(point.Green() == 0);
        REQUIRE(point.Blue() == 0);
    }

    SECTION("Point conversion Point14")
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
        REQUIRE(point.NIR() == 0);
    }

    SECTION("Point conversion Point10 to Poin14")
    {
        auto point = Point(0);

        point.ReturnNumber(5);
        point.NumberOfReturns(6);
        point.ScanDirectionFlag(false);
        point.EdgeOfFlightLineFlag(true);
        point.Classification(15);
        point.Synthetic(true);
        point.KeyPoint(false);
        point.Withheld(true);
        point.ScanAngleRank(45);

        point.ToPointFormat(6);

        REQUIRE(point.ReturnNumber() == 5);
        REQUIRE(point.NumberOfReturns() == 6);
        REQUIRE(point.ScanDirectionFlag() == false);
        REQUIRE(point.EdgeOfFlightLineFlag() == true);
        REQUIRE(point.Classification() == 15);
        REQUIRE(point.Synthetic() == true);
        REQUIRE(point.KeyPoint() == false);
        REQUIRE(point.Withheld() == true);
        REQUIRE(point.Overlap() == false);
        REQUIRE(point.ExtendedScanAngle() == 7500);
        REQUIRE(point.ScannerChannel() == 0);
        REQUIRE_THROWS(point.ReturnsScanDirEofBitFields());

        point.ToPointFormat(0);

        REQUIRE(point.ReturnNumber() == 5);
        REQUIRE(point.NumberOfReturns() == 6);
        REQUIRE(point.ScanDirectionFlag() == false);
        REQUIRE(point.EdgeOfFlightLineFlag() == true);
        REQUIRE(point.Classification() == 15);
        REQUIRE(point.Synthetic() == true);
        REQUIRE(point.KeyPoint() == false);
        REQUIRE(point.Withheld() == true);
        REQUIRE(point.ScanAngleRank() == 45);

        point.ToPointFormat(6);

        point.ReturnNumber(13);
        point.NumberOfReturns(14);
        point.Classification(134);
        point.ScannerChannel(2);
        point.Synthetic(true);
        point.KeyPoint(false);
        point.Withheld(true);
        point.ExtendedScanAngle(28000);

        point.ToPointFormat(0);

        REQUIRE(point.ReturnNumber() == 7);
        REQUIRE(point.NumberOfReturns() == 7);
        REQUIRE(point.Classification() == 31);
        REQUIRE(point.ScanAngleRank() == 90);
        REQUIRE_THROWS(point.Overlap());
        REQUIRE_THROWS(point.ScannerChannel());
        REQUIRE_THROWS(point.ExtendedReturnsBitFields());
        REQUIRE_THROWS(point.ExtendedFlagsBitFields());

        point.ToPointFormat(6);

        REQUIRE(point.ReturnNumber() == 7);
        REQUIRE(point.NumberOfReturns() == 7);
        REQUIRE(point.Classification() == 31);
        REQUIRE(point.ExtendedScanAngle() == 15000);
        REQUIRE(point.Overlap() == false);
        REQUIRE(point.ScannerChannel() == 0);
    }

    SECTION("Operator == and !=")
    {

        // Format 0
        auto point = Point(0);
        auto point_other = Point(0);

        REQUIRE(point == point_other);

        // Format 1
        point_other.ToPointFormat(1);
        REQUIRE(point != point_other);
        point.ToPointFormat(1);
        REQUIRE(point == point_other);

        // Format 2
        point_other.ToPointFormat(2);
        REQUIRE(point != point_other);
        point.ToPointFormat(2);
        REQUIRE(point == point_other);

        // Format 3
        point_other.ToPointFormat(3);
        REQUIRE(point != point_other);
        point.ToPointFormat(3);
        REQUIRE(point == point_other);

        // Format 6
        point_other.ToPointFormat(6);
        REQUIRE(point != point_other);
        point.ToPointFormat(6);
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

        point.PointSourceID(4);
        REQUIRE(point != point_other);
        point_other.PointSourceID(4);
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

        point.NIR(4);
        REQUIRE(point != point_other);
        point_other.NIR(4);
        REQUIRE(point == point_other);
    }

    SECTION("Point ExtraByte")
    {
        auto point = Point(0);
        REQUIRE(point.PointFormatID() == 0);
        REQUIRE(point.PointRecordLength() == 20);
        REQUIRE_THROWS(point.ExtraBytes(std::vector<uint8_t>{2, 3, 4}));
        REQUIRE(point.ExtraBytes().size() == 0);
        REQUIRE(point.NumExtraBytes() == 0);

        point = Point(0, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), 5);
        REQUIRE(point.PointFormatID() == 0);
        REQUIRE(point.PointRecordLength() == 20 + 5);
        REQUIRE(point.NumExtraBytes() == 5);
        REQUIRE(point.ExtraBytes().size() == 5);
        REQUIRE_THROWS(point.ExtraBytes(std::vector<uint8_t>{2, 3, 4}));
        REQUIRE_NOTHROW(point.ExtraBytes(std::vector<uint8_t>{2, 3, 4, 5, 6}));
        REQUIRE(point.ExtraBytes() == std::vector<uint8_t>{2, 3, 4, 5, 6});

        point.ToPointFormat(6);
        REQUIRE(point.PointFormatID() == 6);
        REQUIRE(point.PointRecordLength() == 30 + 5);
        REQUIRE(point.NumExtraBytes() == 5);
        REQUIRE(point.ExtraBytes().size() == 5);
        REQUIRE(point.ExtraBytes() == std::vector<uint8_t>{2, 3, 4, 5, 6});
        REQUIRE_THROWS(point.ExtraBytes(std::vector<uint8_t>{2, 3, 4}));
    }

    SECTION("Operator =")
    {
        auto point = Point(8, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), 2);

        point.UnscaledX(4);
        point.UnscaledY(4);
        point.UnscaledZ(4);

        point.GPSTime(4.0);
        point.Red(4.0);
        point.NIR(4.0);

        point.ExtraBytes(std::vector<uint8_t>{2, 5});

        auto point_other = Point(0);
        point_other = point;

        REQUIRE(point == point_other);
    }

    SECTION("Packing and Unpacking")
    {

        std::stringstream ss;
        auto orig_point =
            Point(0, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), 2); // use two extra bytes
        orig_point.UnscaledX(20);
        orig_point.UnscaledY(-20);
        orig_point.UnscaledZ(100000);
        orig_point.ScanAngleRank(90);
        orig_point.ClassificationBitFields(124);

        // Format 0
        auto point = orig_point;
        point.Pack(ss);
        auto point_other =
            Point::Unpack(ss, 0, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), point.NumExtraBytes());
        REQUIRE(point == point_other);

        // Format 1
        point.ToPointFormat(1);
        point.Pack(ss);
        point_other =
            Point::Unpack(ss, 1, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), point.NumExtraBytes());
        REQUIRE(point == point_other);

        // Format 2
        point.ToPointFormat(2);
        point.Pack(ss);
        point_other =
            Point::Unpack(ss, 2, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), point.NumExtraBytes());
        REQUIRE(point == point_other);

        // Format 3
        point.ToPointFormat(3);
        point.Pack(ss);
        point_other =
            Point::Unpack(ss, 3, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), point.NumExtraBytes());
        REQUIRE(point == point_other);

        // Format 6
        point.ToPointFormat(6);
        point.Pack(ss);
        point_other =
            Point::Unpack(ss, 6, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), point.NumExtraBytes());
        REQUIRE(point == point_other);

        // Format 7
        point.ToPointFormat(7);
        point.Pack(ss);
        point_other =
            Point::Unpack(ss, 7, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), point.NumExtraBytes());
        REQUIRE(point == point_other);

        // Format 8
        point.ToPointFormat(8);
        point.Pack(ss);
        point_other =
            Point::Unpack(ss, 8, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), point.NumExtraBytes());
        REQUIRE(point == point_other);

        point.ToPointFormat(0);
        REQUIRE(point == orig_point);
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
        auto point = Point(3, {1, 1, 1}, copc::Vector3::DefaultOffset());

        point.X(5);
        point.Y(5);
        point.Z(5);

        REQUIRE(point.Within(copc::Box::MaxBox()));
        REQUIRE(!point.Within(copc::Box::ZeroBox()));

        // 2D box
        REQUIRE(point.Within(copc::Box(0, 5, 0, 5)));
        REQUIRE(!point.Within(copc::Box(6, 10, 0, 5)));

        // 3D box
        REQUIRE(point.Within(copc::Box(0, 5, 0, 5, 0, 5)));
        REQUIRE(!point.Within(copc::Box(6, 10, 0, 5, 0, 5)));
    }
}
