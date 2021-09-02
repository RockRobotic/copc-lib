#include <cfloat>
#include <limits>

#include <catch2/catch.hpp>
#include <copc-lib/las/point.hpp>

using namespace copc::las;
using namespace std;

TEST_CASE("Internal tests", "[Point]")
{
    SECTION("Point10")
    {
        REQUIRE_NOTHROW(internal::Point10());
        auto point_10 = internal::Point10(INT32_MAX,
                                          INT32_MAX,
                                          INT32_MAX,
                                          UINT16_MAX,
                                          7,
                                          7,
                                          true,
                                          true,
                                          31,
                                          true,
                                          true,
                                          true,
                                          UINT8_MAX,
                                          90,
                                          UINT16_MAX);

        REQUIRE(point_10.X() == INT32_MAX);
        REQUIRE(point_10.Y() == INT32_MAX);
        REQUIRE(point_10.Z() == INT32_MAX);
        REQUIRE(point_10.Intensity() == UINT16_MAX);
        REQUIRE(point_10.ReturnNumber() == 7);
        REQUIRE(point_10.NumberOfReturns() == 7);
        REQUIRE(point_10.ScanDirectionFlag() == true);
        REQUIRE(point_10.EdgeOfFlightLineFlag() == true);
        REQUIRE(point_10.Classification() == 31);
        REQUIRE(point_10.Synthetic() == true);
        REQUIRE(point_10.KeyPoint() == true);
        REQUIRE(point_10.Withheld() == true);
        REQUIRE(point_10.UserData() == UINT8_MAX);
        REQUIRE(point_10.ScanAngleRank() == 90);
        REQUIRE(point_10.PointSourceId() == UINT16_MAX);

        // Conversion
        internal::Point14 point_14 = point_10.ToPoint14();
        REQUIRE(point_14.X() == INT32_MAX);
        REQUIRE(point_14.Y() == INT32_MAX);
        REQUIRE(point_14.Z() == INT32_MAX);
        REQUIRE(point_14.Intensity() == UINT16_MAX);
        REQUIRE(point_14.ReturnNumber() == 7);
        REQUIRE(point_14.NumberOfReturns() == 7);
        REQUIRE(point_14.Synthetic() == true);
        REQUIRE(point_14.KeyPoint() == true);
        REQUIRE(point_14.Withheld() == true);
        REQUIRE(point_14.Overlap() == false);
        REQUIRE(point_14.ScannerChannel() == 0);
        REQUIRE(point_14.ScanDirectionFlag() == true);
        REQUIRE(point_14.EdgeOfFlightLineFlag() == true);
        REQUIRE(point_14.Classification() == 31);
        REQUIRE(point_14.UserData() == UINT8_MAX);
        REQUIRE(point_14.ScanAngle() == 0);
        REQUIRE(point_14.PointSourceId() == UINT16_MAX);
    }

    SECTION("Point14")
    {
        REQUIRE_NOTHROW(internal::Point14());
        auto point_14 = internal::Point14(INT32_MAX,
                                          INT32_MAX,
                                          INT32_MAX,
                                          UINT16_MAX,
                                          15,
                                          15,
                                          true,
                                          false,
                                          true,
                                          false,
                                          0,
                                          true,
                                          true,
                                          UINT8_MAX,
                                          UINT8_MAX,
                                          30000,
                                          UINT16_MAX);

        REQUIRE(point_14.X() == INT32_MAX);
        REQUIRE(point_14.Y() == INT32_MAX);
        REQUIRE(point_14.Z() == INT32_MAX);
        REQUIRE(point_14.Intensity() == UINT16_MAX);
        REQUIRE(point_14.ReturnNumber() == 15);
        REQUIRE(point_14.NumberOfReturns() == 15);
        REQUIRE(point_14.Synthetic() == true);
        REQUIRE(point_14.KeyPoint() == false);
        REQUIRE(point_14.Withheld() == true);
        REQUIRE(point_14.Overlap() == false);
        REQUIRE(point_14.ScannerChannel() == 0);
        REQUIRE(point_14.ScanDirectionFlag() == true);
        REQUIRE(point_14.EdgeOfFlightLineFlag() == true);
        REQUIRE(point_14.Classification() == UINT8_MAX);
        REQUIRE(point_14.UserData() == UINT8_MAX);
        REQUIRE(point_14.ScanAngle() == 30000);
        REQUIRE(point_14.PointSourceId() == UINT16_MAX);

        // Conversion
        internal::Point10 point_10 = point_14.ToPoint10();
        REQUIRE(point_10.X() == INT32_MAX);
        REQUIRE(point_10.Y() == INT32_MAX);
        REQUIRE(point_10.Z() == INT32_MAX);
        REQUIRE(point_10.Intensity() == UINT16_MAX);
        REQUIRE(point_10.ReturnNumber() == 7);
        REQUIRE(point_10.NumberOfReturns() == 7);
        REQUIRE(point_10.ScanDirectionFlag() == true);
        REQUIRE(point_10.EdgeOfFlightLineFlag() == true);
        REQUIRE(point_10.Classification() == 31);
        REQUIRE(point_10.Synthetic() == true);
        REQUIRE(point_10.KeyPoint() == false);
        REQUIRE(point_10.Withheld() == true);
        REQUIRE(point_10.UserData() == UINT8_MAX);
        REQUIRE(point_10.ScanAngleRank() == 0);
        REQUIRE(point_10.PointSourceId() == UINT16_MAX);
    }
}

TEST_CASE("Point tests", "[Point]")
{
    SECTION("Point Format Test")
    {

        // Test format 0-10
        for (int i = 0; i < 11; i++) {
            REQUIRE_NOTHROW(Point(i));
        }
        // Test check for other values
        REQUIRE_THROWS(Point(-1));
        REQUIRE_THROWS(Point(11));
    }

    SECTION("Point with format LAS 1.0 Test")
    {
        auto point0 = Point(0);
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
        point0.ScanDirFlag(true);
        REQUIRE(point0.ScanDirFlag() == true);

        // Edge of Flight Line
        point0.EdgeOfFlightLineFlag(true);
        REQUIRE(point0.EdgeOfFlightLineFlag() == true);

        // ReturnsScanDirEofBitField
        point0.ReturnsScanDirEofBitFields(172);
        REQUIRE(point0.ReturnNumber() == 4);
        REQUIRE(point0.NumberOfReturns() == 5);
        REQUIRE(point0.ScanDirFlag() == false);
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
        REQUIRE_THROWS(point0.SetNirVal(UINT16_MAX));
        REQUIRE_THROWS(point0.GetNirVal());
        REQUIRE_THROWS(point0.R(UINT16_MAX));
        REQUIRE_THROWS(point0.R());
        REQUIRE_THROWS(point0.G(UINT16_MAX));
        REQUIRE_THROWS(point0.G());
        REQUIRE_THROWS(point0.B(UINT16_MAX));
        REQUIRE_THROWS(point0.B());
        REQUIRE_THROWS(point0.SetGpsTimeVal(DBL_MAX));
        REQUIRE_THROWS(point0.GetGpsTimeVal());
        REQUIRE_THROWS(point0.ScanAngle(INT16_MAX));
        REQUIRE_THROWS(point0.ScanAngle());
        REQUIRE_THROWS(point0.Overlap(true));
        REQUIRE_THROWS(point0.Overlap());
        REQUIRE_THROWS(point0.ReturnsBitFields(UINT8_MAX));
        REQUIRE_THROWS(point0.ReturnsBitFields());
        REQUIRE_THROWS(point0.FlagsBitFields(UINT8_MAX));
        REQUIRE_THROWS(point0.FlagsBitFields());
        REQUIRE_NOTHROW(point0.ToString());

        auto point1 = Point(1);

        point1.SetGpsTimeVal(DBL_MAX);
        REQUIRE(point1.GetGpsTimeVal() == DBL_MAX);

        REQUIRE_THROWS(point0.R(UINT16_MAX));
        REQUIRE_THROWS(point0.R());
        REQUIRE_THROWS(point0.G(UINT16_MAX));
        REQUIRE_THROWS(point0.G());
        REQUIRE_THROWS(point0.B(UINT16_MAX));
        REQUIRE_THROWS(point0.B());
        REQUIRE_THROWS(point0.SetNirVal(UINT16_MAX));
        REQUIRE_THROWS(point0.GetNirVal());

        auto point2 = Point(2);

        point2.R(UINT16_MAX);
        REQUIRE(point2.R() == UINT16_MAX);
        point2.G(UINT16_MAX);
        REQUIRE(point2.G() == UINT16_MAX);
        point2.B(UINT16_MAX);
        REQUIRE(point2.B() == UINT16_MAX);

        point2.SetRgb(UINT16_MAX / 2, UINT16_MAX / 2, UINT16_MAX / 2);
        REQUIRE(point2.R() == UINT16_MAX / 2);
        REQUIRE(point2.G() == UINT16_MAX / 2);
        REQUIRE(point2.B() == UINT16_MAX / 2);

        REQUIRE_THROWS(point2.SetGpsTimeVal(DBL_MAX));
        REQUIRE_THROWS(point2.GetGpsTimeVal());
        REQUIRE_THROWS(point2.SetNirVal(UINT16_MAX));
        REQUIRE_THROWS(point2.GetNirVal());

        auto point3 = Point(3);

        REQUIRE_NOTHROW(point3.SetGpsTimeVal(DBL_MAX));
        REQUIRE_NOTHROW(point3.GetGpsTimeVal());
        REQUIRE_NOTHROW(point3.SetRgb(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_NOTHROW(point3.R());
        REQUIRE_NOTHROW(point3.G());
        REQUIRE_NOTHROW(point3.B());
        REQUIRE_THROWS(point3.SetNirVal(UINT16_MAX));
        REQUIRE_THROWS(point3.GetNirVal());

        auto point4 = Point(4);

        REQUIRE_NOTHROW(point4.SetGpsTimeVal(DBL_MAX));
        REQUIRE_NOTHROW(point4.GetGpsTimeVal());
        REQUIRE_THROWS(point4.SetRgb(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_THROWS(point4.R());
        REQUIRE_THROWS(point4.G());
        REQUIRE_THROWS(point4.B());
        REQUIRE_THROWS(point4.SetNirVal(UINT16_MAX));
        REQUIRE_THROWS(point4.GetNirVal());

        auto point5 = Point(5);

        REQUIRE_NOTHROW(point5.SetGpsTimeVal(DBL_MAX));
        REQUIRE_NOTHROW(point5.GetGpsTimeVal());
        REQUIRE_NOTHROW(point5.SetRgb(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_NOTHROW(point5.R());
        REQUIRE_NOTHROW(point5.G());
        REQUIRE_NOTHROW(point5.B());
        REQUIRE_THROWS(point5.SetNirVal(UINT16_MAX));
        REQUIRE_THROWS(point5.GetNir());
    }

    SECTION("Point with format LAS 1.4 Test")
    {
        auto point6 = Point(6);
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
        point6.ReturnsBitFields(UINT8_MAX);
        REQUIRE(point6.ReturnsBitFields() == UINT8_MAX);

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
        point6.ScanDirFlag(true);
        REQUIRE(point6.ScanDirFlag() == true);
        point6.ScanDirFlag(false);
        REQUIRE(point6.ScanDirFlag() == false);

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
        point6.ScanAngle(-30000);
        REQUIRE(point6.ScanAngle() == -30000);
        REQUIRE_THROWS(point6.ScanAngle(-30001));
        point6.ScanAngle(30000);
        REQUIRE(point6.ScanAngle() == 30000);
        REQUIRE_THROWS(point6.ScanAngle(30001));

        // User Data
        point6.UserData(UINT8_MAX);
        REQUIRE(point6.UserData() == UINT8_MAX);

        // Point Source ID
        point6.PointSourceId(UINT16_MAX);
        REQUIRE(point6.PointSourceId() == UINT16_MAX);

        // GPS Time
        point6.SetGpsTimeVal(DBL_MAX);
        REQUIRE(point6.GetGpsTimeVal() == DBL_MAX);

        // Checks
        REQUIRE_THROWS(point6.SetRgb(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_THROWS(point6.R());
        REQUIRE_THROWS(point6.G());
        REQUIRE_THROWS(point6.B());
        REQUIRE_THROWS(point6.SetNirVal(UINT16_MAX));
        REQUIRE_THROWS(point6.GetNirVal());
        REQUIRE_THROWS(point6.ScanAngleRank());
        REQUIRE_THROWS(point6.ScanAngleRank(INT8_MAX));
        REQUIRE_THROWS(point6.ReturnsScanDirEofBitFields(UINT8_MAX));
        REQUIRE_THROWS(point6.ReturnsScanDirEofBitFields());
        REQUIRE_THROWS(point6.ClassificationBitFields(UINT8_MAX));
        REQUIRE_THROWS(point6.ClassificationBitFields());
        REQUIRE_NOTHROW(point6.ToString());

        auto point7 = Point(7);

        point7.SetRgb(UINT16_MAX, UINT16_MAX, UINT16_MAX);
        REQUIRE(point7.R() == UINT16_MAX);
        REQUIRE(point7.G() == UINT16_MAX);
        REQUIRE(point7.B() == UINT16_MAX);

        REQUIRE_THROWS(point7.SetNirVal(UINT16_MAX));
        REQUIRE_THROWS(point7.GetNirVal());

        auto point8 = Point(8);

        point8.SetNirVal(UINT16_MAX);
        REQUIRE(point8.GetNirVal() == UINT16_MAX);

        auto point9 = Point(9);

        REQUIRE_THROWS(point9.SetRgb(UINT16_MAX, UINT16_MAX, UINT16_MAX));
        REQUIRE_THROWS(point9.R());
        REQUIRE_THROWS(point9.G());
        REQUIRE_THROWS(point9.B());
        REQUIRE_THROWS(point9.SetNirVal(UINT16_MAX));
        REQUIRE_THROWS(point9.GetNirVal());

        // No need to test point10, it has all attributes and they have all been tested already
    }

    SECTION("Point conversion")
    {
        auto point0 = Point(0);
        point0.X(12345);
        point0.Y(12345);
        point0.Z(12345);

        REQUIRE_THROWS(point0.ScannerChannel());
        REQUIRE_THROWS(point0.GetGpsTimeVal());
        REQUIRE_THROWS(point0.GetRgb());
        REQUIRE_THROWS(point0.GetNirVal());

        point0.ToPointFormat(1);

        REQUIRE(point0.X() == 12345);
        REQUIRE(point0.Y() == 12345);
        REQUIRE(point0.Z() == 12345);

        REQUIRE_THROWS(point0.ScannerChannel());
        REQUIRE(point0.GetGpsTimeVal() == 0);
        REQUIRE_THROWS(point0.GetRgb());
        REQUIRE_THROWS(point0.GetNirVal());

        point0.ToPointFormat(2);

        REQUIRE_THROWS(point0.ScannerChannel());
        REQUIRE_THROWS(point0.GetGpsTimeVal());
        REQUIRE(point0.R() == 0);
        REQUIRE(point0.G() == 0);
        REQUIRE(point0.B() == 0);
        REQUIRE_THROWS(point0.GetNirVal());

        point0.ToPointFormat(3);

        REQUIRE_THROWS(point0.ScannerChannel());
        REQUIRE(point0.GetGpsTimeVal() == 0);
        REQUIRE(point0.R() == 0);
        REQUIRE(point0.G() == 0);
        REQUIRE(point0.B() == 0);
        REQUIRE_THROWS(point0.GetNirVal());

        point0.ToPointFormat(4);

        REQUIRE_THROWS(point0.ScannerChannel());
        REQUIRE(point0.GetGpsTimeVal() == 0);
        REQUIRE_THROWS(point0.GetRgb());
        REQUIRE_THROWS(point0.GetNirVal());

        point0.ToPointFormat(5);

        REQUIRE_THROWS(point0.ScannerChannel());
        REQUIRE(point0.GetGpsTimeVal() == 0);
        REQUIRE(point0.R() == 0);
        REQUIRE(point0.G() == 0);
        REQUIRE(point0.B() == 0);
        REQUIRE_THROWS(point0.GetNirVal());

        point0.ToPointFormat(6);

        REQUIRE_THROWS(point0.ScanAngleRank());
        REQUIRE(point0.ScannerChannel() == 0);
        REQUIRE(point0.GetGpsTimeVal() == 0);
        REQUIRE_THROWS(point0.GetRgb());
        REQUIRE_THROWS(point0.GetNirVal());

        point0.ToPointFormat(7);

        REQUIRE_THROWS(point0.ScanAngleRank());
        REQUIRE(point0.ScannerChannel() == 0);
        REQUIRE(point0.GetGpsTimeVal() == 0);
        REQUIRE(point0.R() == 0);
        REQUIRE(point0.G() == 0);
        REQUIRE(point0.B() == 0);
        REQUIRE_THROWS(point0.GetNirVal());

        point0.ToPointFormat(8);

        REQUIRE_THROWS(point0.ScanAngleRank());
        REQUIRE(point0.ScannerChannel() == 0);
        REQUIRE(point0.GetGpsTimeVal() == 0);
        REQUIRE(point0.R() == 0);
        REQUIRE(point0.G() == 0);
        REQUIRE(point0.B() == 0);
        REQUIRE(point0.GetNirVal() == 0);

        point0.ToPointFormat(9);

        REQUIRE_THROWS(point0.ScanAngleRank());
        REQUIRE(point0.ScannerChannel() == 0);
        REQUIRE(point0.GetGpsTimeVal() == 0);
        REQUIRE_THROWS(point0.GetRgb());
        REQUIRE_THROWS(point0.GetNirVal());

        point0.ToPointFormat(10);

        REQUIRE_THROWS(point0.ScanAngleRank());
        REQUIRE(point0.ScannerChannel() == 0);
        REQUIRE(point0.GetGpsTimeVal() == 0);
        REQUIRE(point0.R() == 0);
        REQUIRE(point0.G() == 0);
        REQUIRE(point0.B() == 0);
        REQUIRE(point0.GetNirVal() == 0);
    }
}