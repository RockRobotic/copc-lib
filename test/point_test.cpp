#include <cfloat>
#include <limits>
#include <sstream>

#include <catch2/catch.hpp>
#include <copc-lib/las/point.hpp>

using namespace copc::las;
using namespace std;

TEST_CASE("Point tests", "[Point]")
{
    SECTION("Point Format Initialization")
    {

        auto point = Point();

        REQUIRE(point.HasExtendedPoint() == false);
        REQUIRE(point.HasGPSTime() == false);
        REQUIRE(point.HasRGB() == false);
        REQUIRE(point.HasNIR() == false);

        point.ToPointFormat(5);
        REQUIRE(point.X() == 0);
        REQUIRE(point.Y() == 0);
        REQUIRE(point.Z() == 0);
        REQUIRE(point.Intensity() == 0);
        REQUIRE(point.ReturnNumber() == 0);
        REQUIRE(point.NumberOfReturns() == 0);
        REQUIRE(point.ScanDirectionFlag() == false);
        REQUIRE(point.EdgeOfFlightLineFlag() == false);
        REQUIRE(point.Classification() == 0);
        REQUIRE(point.Synthetic() == false);
        REQUIRE(point.KeyPoint() == false);
        REQUIRE(point.Withheld() == false);
        REQUIRE(point.ScanAngleRank() == 0);
        REQUIRE(point.ScanAngle() == 0.0);
        REQUIRE(point.UserData() == 0);
        REQUIRE(point.PointSourceId() == 0);
        REQUIRE(point.GPSTime() == 0);
        REQUIRE(point.Red() == 0);
        REQUIRE(point.Green() == 0);
        REQUIRE(point.Blue() == 0);
        REQUIRE(point.HasExtendedPoint() == false);
        REQUIRE(point.HasGPSTime() == true);
        REQUIRE(point.HasRGB() == true);
        REQUIRE(point.HasNIR() == false);

        auto point_ext = Point();
        point_ext.ToPointFormat(10);

        REQUIRE(point_ext.X() == 0);
        REQUIRE(point_ext.Y() == 0);
        REQUIRE(point_ext.Z() == 0);
        REQUIRE(point_ext.Intensity() == 0);
        REQUIRE(point_ext.ReturnNumber() == 0);
        REQUIRE(point_ext.NumberOfReturns() == 0);
        REQUIRE(point_ext.ScanDirectionFlag() == false);
        REQUIRE(point_ext.EdgeOfFlightLineFlag() == false);
        REQUIRE(point_ext.Classification() == 0);
        REQUIRE(point_ext.Synthetic() == false);
        REQUIRE(point_ext.KeyPoint() == false);
        REQUIRE(point_ext.Withheld() == false);
        REQUIRE(point_ext.ExtendedScanAngle() == 0);
        REQUIRE(point_ext.ScanAngle() == 0.0);
        REQUIRE(point_ext.UserData() == 0);
        REQUIRE(point_ext.PointSourceId() == 0);
        REQUIRE(point_ext.GPSTime() == 0);
        REQUIRE(point_ext.Red() == 0);
        REQUIRE(point_ext.Green() == 0);
        REQUIRE(point_ext.Blue() == 0);
        REQUIRE(point_ext.NIR() == 0);
        REQUIRE(point_ext.HasExtendedPoint() == true);
        REQUIRE(point_ext.HasGPSTime() == true);
        REQUIRE(point_ext.HasRGB() == true);
        REQUIRE(point_ext.HasNIR() == true);
    }

    SECTION("Point with format LAS 1.0 Test")
    {
        auto point0 = Point();
        // Position
        point0.X(INT32_MAX);
        point0.Y(INT32_MAX);
        point0.Z(INT32_MAX);
        REQUIRE(point0.X() == INT32_MAX);
        REQUIRE(point0.Y() == INT32_MAX);
        REQUIRE(point0.Z() == INT32_MAX);

        // Intensity
        point0.Intensity(INT8_MAX);
        REQUIRE(point0.Intensity() == INT8_MAX);

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
        point0.ClassificationBitFields(UINT8_MAX);
        REQUIRE(point0.ClassificationBitFields() == UINT8_MAX);
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
        point0.UserData(UINT8_MAX);
        REQUIRE(point0.UserData() == UINT8_MAX);

        // Point Source ID
        point0.PointSourceId(UINT16_MAX);
        REQUIRE(point0.PointSourceId() == UINT16_MAX);

        // Checks
        REQUIRE_THROWS(point0.ExtendedFlagsBitFields(0));
        REQUIRE_THROWS(point0.ExtendedFlagsBitFields());
        REQUIRE_THROWS(point0.ScannerChannel(0));
        REQUIRE_THROWS(point0.ScannerChannel());
        REQUIRE_THROWS(point0.NIR(UINT16_MAX));
        REQUIRE_THROWS(point0.NIR());
        REQUIRE_THROWS(point0.Red(UINT16_MAX));
        REQUIRE_THROWS(point0.Red());
        REQUIRE_THROWS(point0.Green(UINT16_MAX));
        REQUIRE_THROWS(point0.Green());
        REQUIRE_THROWS(point0.Blue(UINT16_MAX));
        REQUIRE_THROWS(point0.Blue());
        REQUIRE_THROWS(point0.GPSTime(DBL_MAX));
        REQUIRE_THROWS(point0.GPSTime());
        REQUIRE_THROWS(point0.ExtendedScanAngle(INT16_MAX));
        REQUIRE_THROWS(point0.ExtendedScanAngle());
        REQUIRE_THROWS(point0.Overlap(true));
        REQUIRE_THROWS(point0.Overlap());
        REQUIRE_THROWS(point0.ExtendedReturnsBitFields(UINT8_MAX));
        REQUIRE_THROWS(point0.ExtendedReturnsBitFields());
        REQUIRE_THROWS(point0.ExtendedFlagsBitFields(UINT8_MAX));
        REQUIRE_THROWS(point0.ExtendedFlagsBitFields());
        REQUIRE_NOTHROW(point0.ToString());

        auto point1 = Point();
        point1.ToPointFormat(1);

        point1.GPSTime(DBL_MAX);
        REQUIRE(point1.GPSTime() == DBL_MAX);

        REQUIRE_THROWS(point0.Red(UINT16_MAX));
        REQUIRE_THROWS(point0.Red());
        REQUIRE_THROWS(point0.Green(UINT16_MAX));
        REQUIRE_THROWS(point0.Green());
        REQUIRE_THROWS(point0.Blue(UINT16_MAX));
        REQUIRE_THROWS(point0.Blue());
        REQUIRE_THROWS(point0.NIR(UINT16_MAX));
        REQUIRE_THROWS(point0.NIR());

        auto point2 = Point();
        point2.ToPointFormat(2);

        point2.Red(UINT16_MAX);
        REQUIRE(point2.Red() == UINT16_MAX);
        point2.Green(UINT16_MAX);
        REQUIRE(point2.Green() == UINT16_MAX);
        point2.Blue(UINT16_MAX);
        REQUIRE(point2.Blue() == UINT16_MAX);

        point2.RGB(UINT16_MAX / 2, UINT16_MAX / 2, UINT16_MAX / 2);
        REQUIRE(point2.Red() == UINT16_MAX / 2);
        REQUIRE(point2.Green() == UINT16_MAX / 2);
        REQUIRE(point2.Blue() == UINT16_MAX / 2);

        REQUIRE_THROWS(point2.GPSTime(DBL_MAX));
        REQUIRE_THROWS(point2.GPSTime());
        REQUIRE_THROWS(point2.NIR(UINT16_MAX));
        REQUIRE_THROWS(point2.NIR());

        auto point3 = Point();
        point3.ToPointFormat(3);

        REQUIRE_NOTHROW(point3.GPSTime(DBL_MAX));
        REQUIRE_NOTHROW(point3.GPSTime());
        REQUIRE_NOTHROW(point3.RGB(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_NOTHROW(point3.Red());
        REQUIRE_NOTHROW(point3.Green());
        REQUIRE_NOTHROW(point3.Blue());
        REQUIRE_THROWS(point3.NIR(UINT16_MAX));
        REQUIRE_THROWS(point3.NIR());

        auto point4 = Point();
        point4.ToPointFormat(4);

        REQUIRE_NOTHROW(point4.GPSTime(DBL_MAX));
        REQUIRE_NOTHROW(point4.GPSTime());
        REQUIRE_THROWS(point4.RGB(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_THROWS(point4.Red());
        REQUIRE_THROWS(point4.Green());
        REQUIRE_THROWS(point4.Blue());
        REQUIRE_THROWS(point4.NIR(UINT16_MAX));
        REQUIRE_THROWS(point4.NIR());

        auto point5 = Point();
        point5.ToPointFormat(5);

        REQUIRE_NOTHROW(point5.GPSTime(DBL_MAX));
        REQUIRE_NOTHROW(point5.GPSTime());
        REQUIRE_NOTHROW(point5.RGB(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_NOTHROW(point5.Red());
        REQUIRE_NOTHROW(point5.Green());
        REQUIRE_NOTHROW(point5.Blue());
        REQUIRE_THROWS(point5.NIR(UINT16_MAX));
        REQUIRE_THROWS(point5.NIR());
    }

    SECTION("Point with format LAS 1.4 Test")
    {
        auto point6 = Point();
        point6.ToPointFormat(6);
        // Position
        point6.X(INT32_MAX);
        point6.Y(INT32_MAX);
        point6.Z(INT32_MAX);
        REQUIRE(point6.X() == INT32_MAX);
        REQUIRE(point6.Y() == INT32_MAX);
        REQUIRE(point6.Z() == INT32_MAX);

        // Intensity
        point6.Intensity(INT8_MAX);
        REQUIRE(point6.Intensity() == INT8_MAX);

        // Return BitField
        point6.ExtendedReturnsBitFields(UINT8_MAX);
        REQUIRE(point6.ExtendedReturnsBitFields() == UINT8_MAX);

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
        point6.ExtendedFlagsBitFields(UINT8_MAX);
        REQUIRE(point6.ExtendedFlagsBitFields() == UINT8_MAX);

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
        point6.Classification(UINT8_MAX);
        REQUIRE(point6.Classification() == UINT8_MAX);
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
        point6.UserData(UINT8_MAX);
        REQUIRE(point6.UserData() == UINT8_MAX);

        // Point Source ID
        point6.PointSourceId(UINT16_MAX);
        REQUIRE(point6.PointSourceId() == UINT16_MAX);

        // GPS Time
        point6.GPSTime(DBL_MAX);
        REQUIRE(point6.GPSTime() == DBL_MAX);

        // Checks
        REQUIRE_THROWS(point6.RGB(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_THROWS(point6.Red());
        REQUIRE_THROWS(point6.Green());
        REQUIRE_THROWS(point6.Blue());
        REQUIRE_THROWS(point6.NIR(UINT16_MAX));
        REQUIRE_THROWS(point6.NIR());
        REQUIRE_THROWS(point6.ScanAngleRank());
        REQUIRE_THROWS(point6.ScanAngleRank(INT8_MAX));
        REQUIRE_THROWS(point6.ReturnsScanDirEofBitFields(UINT8_MAX));
        REQUIRE_THROWS(point6.ReturnsScanDirEofBitFields());
        REQUIRE_THROWS(point6.ClassificationBitFields(UINT8_MAX));
        REQUIRE_THROWS(point6.ClassificationBitFields());
        REQUIRE_NOTHROW(point6.ToString());

        auto point7 = Point();
        point7.ToPointFormat(7);

        point7.RGB(UINT16_MAX, UINT16_MAX, UINT16_MAX);
        REQUIRE(point7.Red() == UINT16_MAX);
        REQUIRE(point7.Green() == UINT16_MAX);
        REQUIRE(point7.Blue() == UINT16_MAX);

        REQUIRE_THROWS(point7.NIR(UINT16_MAX));
        REQUIRE_THROWS(point7.NIR());

        auto point8 = Point();
        point8.ToPointFormat(8);

        point8.NIR(UINT16_MAX);
        REQUIRE(point8.NIR() == UINT16_MAX);

        auto point9 = Point();
        point9.ToPointFormat(9);

        REQUIRE_THROWS(point9.RGB(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_THROWS(point9.Red());
        REQUIRE_THROWS(point9.Green());
        REQUIRE_THROWS(point9.Blue());
        REQUIRE_THROWS(point9.NIR(UINT16_MAX));
        REQUIRE_THROWS(point9.NIR());

        // No need to test point10, it has all attributes and they have all been tested already
    }

    SECTION("Point conversion Point10")
    {
        auto point = Point();

        point.ToPointFormat(1);
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

        point.ToPointFormat(4);
        REQUIRE(point.GPSTime() == 0);

        point.ToPointFormat(5);
        REQUIRE(point.GPSTime() == 0);
        REQUIRE(point.Red() == 0);
        REQUIRE(point.Green() == 0);
        REQUIRE(point.Blue() == 0);
    }

    SECTION("Point conversion Point10")
    {
        auto point = Point();

        point.ToPointFormat(6);
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

        point.ToPointFormat(9);
        REQUIRE(point.GPSTime() == 0);
        REQUIRE(point.ScannerChannel() == 0);
        REQUIRE(point.Overlap() == 0);
        REQUIRE(point.GPSTime() == 0);

        point.ToPointFormat(10);
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
        auto point = Point();
        point.ToPointFormat(0);

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
        auto point = Point();
        auto point_other = Point();

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

        // Format 4
        point_other.ToPointFormat(4);
        REQUIRE(point != point_other);
        point.ToPointFormat(4);
        REQUIRE(point == point_other);

        // Format 5
        point_other.ToPointFormat(5);
        REQUIRE(point != point_other);
        point.ToPointFormat(5);
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

        // Format 9
        point_other.ToPointFormat(9);
        REQUIRE(point != point_other);
        point.ToPointFormat(9);
        REQUIRE(point == point_other);

        // Format 10
        point_other.ToPointFormat(10);
        REQUIRE(point != point_other);
        point.ToPointFormat(10);
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

        point.NIR(4);
        REQUIRE(point != point_other);
        point_other.NIR(4);
        REQUIRE(point == point_other);
    }

    SECTION("Operator =")
    {
        auto point = Point();
        point.ToPointFormat(10);

        point.X(4);
        point.Y(4);
        point.Z(4);

        point.GPSTime(4.0);
        point.Red(4.0);
        point.NIR(4.0);

        auto point_other = Point();
        point_other = point;

        REQUIRE(point == point_other);
    }

    SECTION("Packing and Unpacking")
    {

        std::stringstream ss;
        auto point = Point();
        auto point_other = Point();

        // Format 0
        point.ToPointFormat(0);
        point.Pack(ss);
        point_other.Unpack(ss, 0, point.PointRecordLength());
        REQUIRE(point == point_other);

        // Format 1
        point.ToPointFormat(1);
        point.Pack(ss);
        point_other.Unpack(ss, 1, point.PointRecordLength());
        REQUIRE(point == point_other);

        // Format 2
        point.ToPointFormat(2);
        point.Pack(ss);
        point_other.Unpack(ss, 2, point.PointRecordLength());
        REQUIRE(point == point_other);

        // Format 3
        point.ToPointFormat(3);
        point.Pack(ss);
        point_other.Unpack(ss, 3, point.PointRecordLength());
        REQUIRE(point == point_other);

        // Format 4
        point.ToPointFormat(4);
        point.Pack(ss);
        point_other.Unpack(ss, 4, point.PointRecordLength());
        REQUIRE(point == point_other);

        // Format 5
        point.ToPointFormat(5);
        point.Pack(ss);
        point_other.Unpack(ss, 5, point.PointRecordLength());
        REQUIRE(point == point_other);

        // Format 6
        point.ToPointFormat(6);
        point.Pack(ss);
        point_other.Unpack(ss, 6, point.PointRecordLength());
        REQUIRE(point == point_other);

        // Format 7
        point.ToPointFormat(7);
        point.Pack(ss);
        point_other.Unpack(ss, 7, point.PointRecordLength());
        REQUIRE(point == point_other);

        // Format 8
        point.ToPointFormat(8);
        point.Pack(ss);
        point_other.Unpack(ss, 8, point.PointRecordLength());
        REQUIRE(point == point_other);

        // Format 9
        point.ToPointFormat(9);
        point.Pack(ss);
        point_other.Unpack(ss, 9, point.PointRecordLength());
        REQUIRE(point == point_other);

        // Format 10
        point.ToPointFormat(10);
        point.Pack(ss);
        point_other.Unpack(ss, 10, point.PointRecordLength());
        REQUIRE(point == point_other);
    }
}
