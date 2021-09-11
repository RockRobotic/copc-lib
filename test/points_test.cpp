#include <limits>

#include <catch2/catch.hpp>
#include <copc-lib/las/point.hpp>
#include <copc-lib/las/points.hpp>

using namespace copc::las;
using namespace std;

TEST_CASE("Points tests", "[Point]")
{
    SECTION("Points constructors")
    {
        auto points = Points(3, 4);
        REQUIRE(points.PointFormatID() == 3);
        REQUIRE(points.PointRecordLength() == 38);
        REQUIRE(points.Get().empty());

        auto point_vec = std::vector<Point>();
        auto point1 = Point(3, 4);
        point1.X(11);
        point1.Y(11);
        point1.Z(11);
        point_vec.push_back(point1);

        auto point2 = points.NewPoint();
        point_vec.push_back(point2);

        auto point3 = Point(3, 4);
        point_vec.push_back(point3);

        points = Points(point_vec);
        REQUIRE(points.PointFormatID() == 3);
        REQUIRE(points.PointRecordLength() == 38);
        for (const auto &point : points.Get())
            REQUIRE(point.PointFormatID() == 3);
        REQUIRE(points.PointRecordLength() == 38);
        REQUIRE(points.Get(0).X() == 11);
        REQUIRE(points.Get(0).Y() == 11);
        REQUIRE(points.Get(0).Z() == 11);

        points.ToString();
    }

    SECTION("Adding Point to Points")
    {
        auto points = Points(3, 0);
        auto point = Point(3, 0);
        point.X(11);
        point.Y(11);
        point.Z(11);

        points.AddPoint(point);

        REQUIRE(points.Get().size() == 1);
        REQUIRE(points.Get(0).X() == 11);
        REQUIRE(points.Get(0).Y() == 11);
        REQUIRE(points.Get(0).Z() == 11);

        point = Point(3, 0);
        point.X(22);
        point.Y(22);
        point.Z(22);

        points.AddPoint(point);
        REQUIRE(points.Get().size() == 2);
        REQUIRE(points.Get(1).X() == 22);
        REQUIRE(points.Get(1).Y() == 22);
        REQUIRE(points.Get(1).Z() == 22);

        // Test check on point format
        point = Point(6, 0);
        REQUIRE_THROWS(points.AddPoint(point));

        // Test check on extra bytes
        point = Point(3, 1);
        REQUIRE_THROWS(points.AddPoint(point));
    }

    SECTION("Adding Points to Points")
    {
        auto points = Points(std::vector<Point>(10, Point(3, 4)));
        auto points_other = Points(std::vector<Point>(10, Point(3, 4)));

        points.AddPoints(points_other);

        REQUIRE(points.Get().size() == 20);

        // Test check on point format
        points_other = Points(std::vector<Point>(10, Point(6, 4)));
        REQUIRE_THROWS(points.AddPoints(points_other));

        // Test check on extra bytes
        points_other = Points(std::vector<Point>(10, Point(3, 1)));
        REQUIRE_THROWS(points.AddPoints(points_other));
    }

    SECTION("Points format conversion")
    {
        auto points = Points(std::vector<Point>(10, Point(3, 4)));
        points.ToPointFormat(6);

        REQUIRE(points.PointFormatID() == 6);
        REQUIRE(points.PointRecordLength() == 38);
        REQUIRE(points.Get(1).PointFormatID() == 6);
        REQUIRE(points.Get(1).PointRecordLength() == 34);

        REQUIRE_THROWS(points.ToPointFormat(-1));
        REQUIRE_THROWS(points.ToPointFormat(11));
    }
}
