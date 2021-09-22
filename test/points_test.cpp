#include <limits>
#include <random>

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

        auto point_vec = std::vector<std::shared_ptr<Point>>();
        auto point1 = points.CreatePoint();
        point1->UnscaledX(11);
        point1->UnscaledY(11);
        point1->UnscaledZ(11);
        point_vec.push_back(point1);

        auto point2 = points.CreatePoint();
        point_vec.push_back(point2);

        auto point3 = points.CreatePoint();
        point_vec.push_back(point3);

        points = Points(point_vec);
        REQUIRE(points.PointFormatID() == 3);
        REQUIRE(points.PointRecordLength() == 38);
        for (const auto &point : points.Get())
            REQUIRE(point->PointFormatID() == 3);
        REQUIRE(points.PointRecordLength() == 38);
        REQUIRE(points.Get(0)->UnscaledX() == 11);
        REQUIRE(points.Get(0)->UnscaledY() == 11);
        REQUIRE(points.Get(0)->UnscaledZ() == 11);

        points.ToString();
    }

    SECTION("Adding Point to Points")
    {
        auto points = Points(3, {1, 1, 1}, {0, 0, 0}, 0);
        auto point = points.CreatePoint();
        point->UnscaledX(11);
        point->UnscaledY(11);
        point->UnscaledZ(11);

        points.AddPoint(point);

        REQUIRE(points.Get().size() == 1);
        REQUIRE(points.Get(0)->UnscaledX() == 11);
        REQUIRE(points.Get(0)->UnscaledY() == 11);
        REQUIRE(points.Get(0)->UnscaledZ() == 11);

        point = points.CreatePoint();
        point->UnscaledX(22);
        point->UnscaledY(22);
        point->UnscaledZ(22);

        points.AddPoint(point);
        REQUIRE(points.Get().size() == 2);
        REQUIRE(points.Get(1)->UnscaledX() == 22);
        REQUIRE(points.Get(1)->UnscaledY() == 22);
        REQUIRE(points.Get(1)->UnscaledZ() == 22);

        // Test check on point format
        point = std::make_shared<Point>(6, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), 0);
        REQUIRE_THROWS(points.AddPoint(point));

        // Test check on extra bytes
        point = std::make_shared<Point>(3, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), 1);
        REQUIRE_THROWS(points.AddPoint(point));
    }

    SECTION("Adding Points to Points")
    {
        auto points = Points(std::vector<std::shared_ptr<Point>>(
            10, std::make_shared<Point>(3, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), 4)));
        auto points_other = Points(std::vector<std::shared_ptr<Point>>(
            10, std::make_shared<Point>(3, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), 4)));

        points.AddPoints(points_other);

        REQUIRE(points.Get().size() == 20);

        // Test check on point format
        points_other = Points(std::vector<std::shared_ptr<Point>>(
            10, std::make_shared<Point>(6, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), 4)));
        REQUIRE_THROWS(points.AddPoints(points_other));

        // Test check on extra bytes
        points_other = Points(std::vector<std::shared_ptr<Point>>(
            10, std::make_shared<Point>(3, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), 1)));
        REQUIRE_THROWS(points.AddPoints(points_other));
    }

    SECTION("Points format conversion")
    {
        auto points = Points(std::vector<std::shared_ptr<Point>>(
            10, std::make_shared<Point>(3, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), 4)));
        points.ToPointFormat(6);

        REQUIRE(points.PointFormatID() == 6);
        REQUIRE(points.PointRecordLength() == 38);
        REQUIRE(points.Get(1)->PointFormatID() == 6);
        REQUIRE(points.Get(1)->PointRecordLength() == 34);

        REQUIRE_THROWS(points.ToPointFormat(-1));
        REQUIRE_THROWS(points.ToPointFormat(11));
    }

    SECTION("Points Group Accessors")
    {
        auto points = Points(3, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), 4);

        // generate points
        int num_points = 2000;
        for (int i = 0; i < num_points; i++)
        {
            auto p = points.CreatePoint();
            p->X(i);
            p->Y(i * 3);
            p->Z(i - 80);
            points.AddPoint(p);
        }

        REQUIRE(points.Size() == num_points);

        // test that the getters work
        auto X = points.X();
        auto Y = points.Y();
        auto Z = points.Z();
        for (int i = 0; i < num_points; i++)
        {
            REQUIRE(X[i] == i);
            REQUIRE(Y[i] == i * 3);
            REQUIRE(Z[i] == i - 80);
        }

        // generate vector of coordinates
        std::vector<double> Xn;
        std::vector<double> Yn;
        std::vector<double> Zn;

        REQUIRE_THROWS(points.X(Xn));
        REQUIRE_THROWS(points.Y(Yn));
        REQUIRE_THROWS(points.Z(Zn));

        for (int i = 0; i < num_points - 1; i++)
        {
            Xn.push_back(i * 50 + 8);
            Yn.push_back(i + 800);
            Zn.push_back(i * 4);
        }

        REQUIRE_THROWS(points.X(Xn));
        REQUIRE_THROWS(points.Y(Yn));
        REQUIRE_THROWS(points.Z(Zn));

        // add the last point
        Xn.push_back(1);
        Yn.push_back(2);
        Zn.push_back(3);

        // test setters
        REQUIRE_NOTHROW(points.X(Xn));
        REQUIRE_NOTHROW(points.Y(Yn));
        REQUIRE_NOTHROW(points.Z(Zn));

        for (int i = 0; i < num_points - 1; i++)
        {
            auto p = points.Get(i);
            REQUIRE(p->X() == i * 50 + 8);
            REQUIRE(p->Y() == i + 800);
            REQUIRE(p->Z() == i * 4);
        }

        // test last point
        auto last_point = points.Get(points.Size() - 1);
        REQUIRE(last_point->X() == 1);
        REQUIRE(last_point->Y() == 2);
        REQUIRE(last_point->Z() == 3);
    }

    SECTION("Points Indexers")
    {
        auto points = Points(3, copc::Vector3::DefaultScale(), copc::Vector3::DefaultOffset(), 4);

        // generate points
        int num_points = 2000;
        for (int i = 0; i < num_points; i++)
        {
            auto p = points.CreatePoint();
            p->X(i);
            p->Y(i * 3);
            p->Z(i - 80);
            points.AddPoint(p);
        }

        REQUIRE(points[0]->X() == 0);
        REQUIRE(points.Get(0)->X() == 0);
        REQUIRE(points[0]->Y() == 0);
        REQUIRE(points.Get(0)->Y() == 0);
        REQUIRE(points[0]->Z() == 0 - 80);
        REQUIRE(points.Get(0)->Z() == 0 - 80);

        REQUIRE(points[1999]->X() == 1999);
        REQUIRE(points.Get(1999)->X() == 1999);
        REQUIRE(points[1999]->Y() == 1999 * 3);
        REQUIRE(points.Get(1999)->Y() == 1999 * 3);
        REQUIRE(points[1999]->Z() == 1999 - 80);
        REQUIRE(points.Get(1999)->Z() == 1999 - 80);

        for (int i = 0; i < num_points; i++)
        {
            points[i]->X(-i);
            points[i]->Y(-i * 2);
            points[i]->Z(-i * 3);
        }

        REQUIRE(points[0]->X() == 0);
        REQUIRE(points.Get(0)->X() == 0);
        REQUIRE(points[0]->Y() == 0);
        REQUIRE(points.Get(0)->Y() == 0);
        REQUIRE(points[0]->Z() == 0);
        REQUIRE(points.Get(0)->Z() == 0);

        REQUIRE(points[1999]->X() == -1999);
        REQUIRE(points.Get(1999)->X() == -1999);
        REQUIRE(points[1999]->Y() == -1999 * 2);
        REQUIRE(points.Get(1999)->Y() == -1999 * 2);
        REQUIRE(points[1999]->Z() == -1999 * 3);
        REQUIRE(points.Get(1999)->Z() == -1999 * 3);

        // generate vector of coordinates
        std::vector<double> Xn;
        std::vector<double> Yn;
        std::vector<double> Zn;

        REQUIRE_THROWS(points.X(Xn));
        REQUIRE_THROWS(points.Y(Yn));
        REQUIRE_THROWS(points.Z(Zn));

        for (int i = 0; i < num_points - 1; i++)
        {
            Xn.push_back(i * 50 + 8);
            Yn.push_back(i + 800);
            Zn.push_back(i * 4);
        }

        REQUIRE_THROWS(points.X(Xn));
        REQUIRE_THROWS(points.Y(Yn));
        REQUIRE_THROWS(points.Z(Zn));

        // add the last point
        Xn.push_back(1);
        Yn.push_back(2);
        Zn.push_back(3);

        // test setters
        REQUIRE_NOTHROW(points.X(Xn));
        REQUIRE_NOTHROW(points.Y(Yn));
        REQUIRE_NOTHROW(points.Z(Zn));

        for (int i = 0; i < num_points - 1; i++)
        {
            auto p = points.Get(i);
            REQUIRE(p->X() == i * 50 + 8);
            REQUIRE(p->Y() == i + 800);
            REQUIRE(p->Z() == i * 4);
        }

        // test last point
        auto last_point = points.Get(points.Size() - 1);
        REQUIRE(last_point->X() == 1);
        REQUIRE(last_point->Y() == 2);
        REQUIRE(last_point->Z() == 3);
    }

    SECTION("Within")
    {
        auto points = Points(3, {1, 1, 1}, copc::Vector3::DefaultOffset());

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(0.0, 5.0);

        // generate points
        int num_points = 2000;
        for (int i = 0; i < num_points; i++)
        {
            auto p = points.CreatePoint();
            p.X(dist(gen));
            p.Y(dist(gen));
            p.Z(dist(gen));
            points.AddPoint(p);
        }

        REQUIRE(points.Within(copc::Box(0, 0, 0, 5, 5, 5)));

        auto p = points.CreatePoint();
        p.X(dist(gen));
        p.Y(dist(gen));
        p.Z(6);
        points.AddPoint(p);

        REQUIRE(!points.Within(copc::Box(0, 0, 0, 5, 5, 5)));
    }
}
