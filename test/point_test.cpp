#include <cfloat>
#include <limits>

#include <catch2/catch.hpp>
#include <copc-lib/las/point2.hpp>

using namespace copc::las;
using namespace std;

TEST_CASE("Point tests", "[Point]")
{
    SECTION("Point Format Test")
    {
        REQUIRE_NOTHROW(Point2());
    }

    SECTION("Point with format LAS 1.0 Test")
    {
        auto point0 = Point2();
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
        REQUIRE(point0.ScanAngle() == 90);
        REQUIRE_THROWS(point0.ScanAngleRank(91));
        REQUIRE_THROWS(point0.ScanAngleRank(-91));

        // User Data
        point0.UserData(UINT8_MAX);
        REQUIRE(point0.UserData() == UINT8_MAX);

        // Point Source ID
        point0.PointSourceId(UINT16_MAX);
        REQUIRE(point0.PointSourceId() == UINT16_MAX);

        // Checks
        REQUIRE_THROWS(point0.FlagsBitFields(0));
        REQUIRE_THROWS(point0.FlagsBitFields());
        REQUIRE_THROWS(point0.ScannerChannel(0));
        REQUIRE_THROWS(point0.ScannerChannel());
        REQUIRE_THROWS(point0.Nir(UINT16_MAX));
        REQUIRE_THROWS(point0.Nir());
        REQUIRE_THROWS(point0.R(UINT16_MAX));
        REQUIRE_THROWS(point0.R());
        REQUIRE_THROWS(point0.G(UINT16_MAX));
        REQUIRE_THROWS(point0.G());
        REQUIRE_THROWS(point0.B(UINT16_MAX));
        REQUIRE_THROWS(point0.B());
        REQUIRE_THROWS(point0.GpsTime(DBL_MAX));
        REQUIRE_THROWS(point0.GpsTime());
        REQUIRE_THROWS(point0.ExtendedScanAngle(INT16_MAX));
        REQUIRE_THROWS(point0.ExtendedScanAngle());
        REQUIRE_THROWS(point0.Overlap(true));
        REQUIRE_THROWS(point0.Overlap());
        REQUIRE_THROWS(point0.ExtendedReturns(UINT8_MAX));
        REQUIRE_THROWS(point0.ExtendedReturns());
        REQUIRE_THROWS(point0.FlagsBitFields(UINT8_MAX));
        REQUIRE_THROWS(point0.FlagsBitFields());
        REQUIRE_NOTHROW(point0.ToString());

        auto point1 = Point2();
        point1.ToPointFormat(1);

        point1.GpsTime(DBL_MAX);
        REQUIRE(point1.GpsTime() == DBL_MAX);

        REQUIRE_THROWS(point0.R(UINT16_MAX));
        REQUIRE_THROWS(point0.R());
        REQUIRE_THROWS(point0.G(UINT16_MAX));
        REQUIRE_THROWS(point0.G());
        REQUIRE_THROWS(point0.B(UINT16_MAX));
        REQUIRE_THROWS(point0.B());
        REQUIRE_THROWS(point0.Nir(UINT16_MAX));
        REQUIRE_THROWS(point0.Nir());

        auto point2 = Point2();
        point2.ToPointFormat(2);

        point2.R(UINT16_MAX);
        REQUIRE(point2.R() == UINT16_MAX);
        point2.G(UINT16_MAX);
        REQUIRE(point2.G() == UINT16_MAX);
        point2.B(UINT16_MAX);
        REQUIRE(point2.B() == UINT16_MAX);

        point2.Rgb(UINT16_MAX / 2, UINT16_MAX / 2, UINT16_MAX / 2);
        REQUIRE(point2.R() == UINT16_MAX / 2);
        REQUIRE(point2.G() == UINT16_MAX / 2);
        REQUIRE(point2.B() == UINT16_MAX / 2);

        REQUIRE_THROWS(point2.GpsTime(DBL_MAX));
        REQUIRE_THROWS(point2.GpsTime());
        REQUIRE_THROWS(point2.Nir(UINT16_MAX));
        REQUIRE_THROWS(point2.Nir());

        auto point3 = Point2();
        point3.ToPointFormat(3);

        REQUIRE_NOTHROW(point3.GpsTime(DBL_MAX));
        REQUIRE_NOTHROW(point3.GpsTime());
        REQUIRE_NOTHROW(point3.Rgb(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_NOTHROW(point3.R());
        REQUIRE_NOTHROW(point3.G());
        REQUIRE_NOTHROW(point3.B());
        REQUIRE_THROWS(point3.Nir(UINT16_MAX));
        REQUIRE_THROWS(point3.Nir());

        auto point4 = Point2();
        point4.ToPointFormat(4);

        REQUIRE_NOTHROW(point4.GpsTime(DBL_MAX));
        REQUIRE_NOTHROW(point4.GpsTime());
        REQUIRE_THROWS(point4.Rgb(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_THROWS(point4.R());
        REQUIRE_THROWS(point4.G());
        REQUIRE_THROWS(point4.B());
        REQUIRE_THROWS(point4.Nir(UINT16_MAX));
        REQUIRE_THROWS(point4.Nir());

        auto point5 = Point2();
        point5.ToPointFormat(5);

        REQUIRE_NOTHROW(point5.GpsTime(DBL_MAX));
        REQUIRE_NOTHROW(point5.GpsTime());
        REQUIRE_NOTHROW(point5.Rgb(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_NOTHROW(point5.R());
        REQUIRE_NOTHROW(point5.G());
        REQUIRE_NOTHROW(point5.B());
        REQUIRE_THROWS(point5.Nir(UINT16_MAX));
        REQUIRE_THROWS(point5.Nir());
    }

    SECTION("Point with format LAS 1.4 Test")
    {
        auto point6 = Point2();
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
        point6.ExtendedReturns(UINT8_MAX);
        REQUIRE(point6.ExtendedReturns() == UINT8_MAX);

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
        point6.FlagsBitFields(UINT8_MAX);
        REQUIRE(point6.FlagsBitFields() == UINT8_MAX);

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
        REQUIRE_THROWS(point6.ExtendedScanAngle(-30001));
        point6.ExtendedScanAngle(30000);
        REQUIRE(point6.ExtendedScanAngle() == 30000);
        REQUIRE_THROWS(point6.ExtendedScanAngle(30001));

        // User Data
        point6.UserData(UINT8_MAX);
        REQUIRE(point6.UserData() == UINT8_MAX);

        // Point Source ID
        point6.PointSourceId(UINT16_MAX);
        REQUIRE(point6.PointSourceId() == UINT16_MAX);

        // GPS Time
        point6.GpsTime(DBL_MAX);
        REQUIRE(point6.GpsTime() == DBL_MAX);

        // Checks
        REQUIRE_THROWS(point6.Rgb(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_THROWS(point6.R());
        REQUIRE_THROWS(point6.G());
        REQUIRE_THROWS(point6.B());
        REQUIRE_THROWS(point6.Nir(UINT16_MAX));
        REQUIRE_THROWS(point6.Nir());
        REQUIRE_THROWS(point6.ScanAngleRank());
        REQUIRE_THROWS(point6.ScanAngleRank(INT8_MAX));
        REQUIRE_THROWS(point6.ReturnsScanDirEofBitFields(UINT8_MAX));
        REQUIRE_THROWS(point6.ReturnsScanDirEofBitFields());
        REQUIRE_THROWS(point6.ClassificationBitFields(UINT8_MAX));
        REQUIRE_THROWS(point6.ClassificationBitFields());
        REQUIRE_NOTHROW(point6.ToString());

        auto point7 = Point2();
        point7.ToPointFormat(7);

        point7.Rgb(UINT16_MAX, UINT16_MAX, UINT16_MAX);
        REQUIRE(point7.R() == UINT16_MAX);
        REQUIRE(point7.G() == UINT16_MAX);
        REQUIRE(point7.B() == UINT16_MAX);

        REQUIRE_THROWS(point7.Nir(UINT16_MAX));
        REQUIRE_THROWS(point7.Nir());

        auto point8 = Point2();
        point8.ToPointFormat(8);

        point8.Nir(UINT16_MAX);
        REQUIRE(point8.Nir() == UINT16_MAX);

        auto point9 = Point2();
        point9.ToPointFormat(9);

        REQUIRE_THROWS(point9.Rgb(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_THROWS(point9.R());
        REQUIRE_THROWS(point9.G());
        REQUIRE_THROWS(point9.B());
        REQUIRE_THROWS(point9.Nir(UINT16_MAX));
        REQUIRE_THROWS(point9.Nir());

        // No need to test point10, it has all attributes and they have all been tested already
    }

    SECTION("Point conversion")
    {
        auto point = Point2();
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
        point.Synthetic(true);
        point.KeyPoint(false);
        point.Withheld(true);
        point.ExtendedScanAngle(28000);

        point.ToPointFormat(0);

        REQUIRE(point.ReturnNumber() == 7);
        REQUIRE(point.NumberOfReturns() == 7);
        REQUIRE(point.Classification() == 31);
        REQUIRE(point.ScanAngleRank() == 90);

        point.ToPointFormat(6);

        REQUIRE(point.ReturnNumber() == 7);
        REQUIRE(point.NumberOfReturns() == 7);
        REQUIRE(point.Classification() == 31);
        REQUIRE(point.ExtendedScanAngle() == 15000);

    }
}