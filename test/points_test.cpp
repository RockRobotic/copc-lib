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
        auto points = Points(3, {1, 1, 1}, {0, 0, 0}, 4);
        REQUIRE(points.PointFormatID() == 3);
        REQUIRE(points.PointRecordLength() == 38);
        REQUIRE(points.Get().empty());

        auto point_vec = std::vector<Point>();
        auto point1 = Point(3, 4);
        point1.UnscaledX(11);
        point1.UnscaledY(11);
        point1.UnscaledZ(11);
        point_vec.push_back(point1);

        auto point2 = points.CreatePoint();
        point_vec.push_back(point2);

        auto point3 = Point(3, 4);
        point_vec.push_back(point3);

        points = Points(point_vec);
        REQUIRE(points.PointFormatID() == 3);
        REQUIRE(points.PointRecordLength() == 38);
        for (const auto &point : points.Get())
            REQUIRE(point.PointFormatID() == 3);
        REQUIRE(points.PointRecordLength() == 38);
        REQUIRE(points.Get(0).UnscaledX() == 11);
        REQUIRE(points.Get(0).UnscaledY() == 11);
        REQUIRE(points.Get(0).UnscaledZ() == 11);

        points.ToString();
    }

    SECTION("Adding Point to Points")
    {
        auto points = Points(3, {1, 1, 1}, {0, 0, 0}, 0);
        auto point = Point(3, 0);
        point.UnscaledX(11);
        point.UnscaledY(11);
        point.UnscaledZ(11);

        points.AddPoint(point);

        REQUIRE(points.Get().size() == 1);
        REQUIRE(points.Get(0).UnscaledX() == 11);
        REQUIRE(points.Get(0).UnscaledY() == 11);
        REQUIRE(points.Get(0).UnscaledZ() == 11);

        point = Point(3, 0);
        point.UnscaledX(22);
        point.UnscaledY(22);
        point.UnscaledZ(22);

        points.AddPoint(point);
        REQUIRE(points.Get().size() == 2);
        REQUIRE(points.Get(1).UnscaledX() == 22);
        REQUIRE(points.Get(1).UnscaledY() == 22);
        REQUIRE(points.Get(1).UnscaledZ() == 22);

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
