#include <limits>
#include <random>

#include <catch2/catch.hpp>
#include <copc-lib/geometry/box.hpp>
#include <copc-lib/geometry/vector3.hpp>
#include <copc-lib/las/point.hpp>
#include <copc-lib/las/points.hpp>

using namespace copc::las;
using namespace std;

TEST_CASE("Points tests", "[Point]")
{
    SECTION("Points constructors")
    {
        auto points = Points(6, 4);
        REQUIRE(points.PointFormatId() == 6);
        REQUIRE(points.PointRecordLength() == 34);
        REQUIRE(points.Get().empty());

        auto point_vec = std::vector<std::shared_ptr<Point>>();
        auto point1 = points.CreatePoint();
        point1->X(11.1);
        point1->Y(11.2);
        point1->Z(11.3);
        point_vec.push_back(point1);

        auto point2 = points.CreatePoint();
        point_vec.push_back(point2);

        auto point3 = points.CreatePoint();
        point_vec.push_back(point3);

        points = Points(point_vec);
        REQUIRE(points.PointFormatId() == 6);
        REQUIRE(points.PointRecordLength() == 34);
        for (const auto &point : points.Get())
            REQUIRE(point->PointFormatId() == 6);
        REQUIRE(points.Get(0)->X() == 11.1);
        REQUIRE(points.Get(0)->Y() == 11.2);
        REQUIRE(points.Get(0)->Z() == 11.3);

        SECTION("Points ToString") 
        { 
            points.ToString();
            std::stringstream ss;
            ss << points;
        }    
    }

    SECTION("Adding Point to Points")
    {
        auto points = Points(6, 0);
        auto point = points.CreatePoint();
        point->X(11.1);
        point->Y(11.2);
        point->Z(11.3);

        points.AddPoint(point);

        REQUIRE(points.Get().size() == 1);
        REQUIRE(points.Get(0)->X() == 11.1);
        REQUIRE(points.Get(0)->Y() == 11.2);
        REQUIRE(points.Get(0)->Z() == 11.3);

        point = points.CreatePoint();
        point->X(22.5);
        point->Y(22.6);
        point->Z(22.7);

        points.AddPoint(point);
        REQUIRE(points.Get().size() == 2);
        REQUIRE(points.Get(1)->X() == 22.5);
        REQUIRE(points.Get(1)->Y() == 22.6);
        REQUIRE(points.Get(1)->Z() == 22.7);

        // Test check on point format
        point = std::make_shared<Point>(7, 0);
        REQUIRE_THROWS(points.AddPoint(point));

        // Test check on extra bytes
        point = std::make_shared<Point>(6, 1);
        REQUIRE_THROWS(points.AddPoint(point));
    }

    SECTION("Adding Points to Points")
    {
        auto points = Points(std::vector<std::shared_ptr<Point>>(10, std::make_shared<Point>(6, 4)));
        auto points_other = Points(std::vector<std::shared_ptr<Point>>(10, std::make_shared<Point>(6, 4)));

        points.AddPoints(points_other);

        REQUIRE(points.Get().size() == 20);

        // Test check on point format
        points_other = Points(std::vector<std::shared_ptr<Point>>(10, std::make_shared<Point>(7, 4)));
        REQUIRE_THROWS(points.AddPoints(points_other));

        // Test check on extra bytes
        points_other = Points(std::vector<std::shared_ptr<Point>>(10, std::make_shared<Point>(6, 1)));
        REQUIRE_THROWS(points.AddPoints(points_other));
    }

    SECTION("Points format conversion")
    {
        auto points = Points(std::vector<std::shared_ptr<Point>>(10, std::make_shared<Point>(6, 4)));
        points.ToPointFormat(7);

        REQUIRE(points.PointFormatId() == 7);
        REQUIRE(points.PointRecordLength() == 40);
        REQUIRE(points.Get(1)->PointFormatId() == 7);
        REQUIRE(points.Get(1)->PointRecordLength() == 40);

        REQUIRE_THROWS(points.ToPointFormat(5));
        REQUIRE_THROWS(points.ToPointFormat(9));
    }

    SECTION("Points Group Accessors")
    {
        auto points = Points(7, 4);

        // generate points
        int num_points = 2000;
        for (int i = 0; i < num_points; i++)
        {
            auto p = points.CreatePoint();
            p->X(i);
            p->Y(i * 3);
            p->Z(i - 80);
            p->Classification(i * 255 / num_points);
            p->PointSourceId(i * 255 / num_points);
            p->Red(i * 4);
            p->Green(i * 5);
            p->Blue(i * 6);
            points.AddPoint(p);
        }

        REQUIRE(points.Size() == num_points);

        // test that the getters work
        auto X = points.X();
        auto Y = points.Y();
        auto Z = points.Z();
        auto classification = points.Classification();
        auto point_source_id = points.PointSourceId();
        auto red = points.Red();
        auto green = points.Green();
        auto blue = points.Blue();
        for (int i = 0; i < num_points; i++)
        {
            REQUIRE(X[i] == i);
            REQUIRE(Y[i] == i * 3);
            REQUIRE(Z[i] == i - 80);
            REQUIRE(classification[i] == i * 255 / num_points);
            REQUIRE(point_source_id[i] == i * 255 / num_points);
            REQUIRE(red[i] == i * 4);
            REQUIRE(green[i] == i * 5);
            REQUIRE(blue[i] == i * 6);
        }

        // generate vector of coordinates
        std::vector<double> Xn;
        std::vector<double> Yn;
        std::vector<double> Zn;
        std::vector<int32_t> UnscaledXn;
        std::vector<int32_t> UnscaledYn;
        std::vector<int32_t> UnscaledZn;
        std::vector<uint8_t> classification_n;
        std::vector<uint8_t> point_source_id_n;
        std::vector<uint16_t> red_n;
        std::vector<uint16_t> green_n;
        std::vector<uint16_t> blue_n;

        REQUIRE_THROWS(points.X(Xn));
        REQUIRE_THROWS(points.Y(Yn));
        REQUIRE_THROWS(points.Z(Zn));
        REQUIRE_THROWS(points.Classification(classification_n));
        REQUIRE_THROWS(points.PointSourceId(point_source_id_n));
        REQUIRE_THROWS(points.Red(red_n));
        REQUIRE_THROWS(points.Green(green_n));
        REQUIRE_THROWS(points.Blue(blue_n));

        for (int i = 0; i < num_points - 1; i++)
        {
            Xn.push_back(i * 50 + 8);
            Yn.push_back(i + 800);
            Zn.push_back(i * 4);
            classification_n.push_back(i * 255 / 2000);
            point_source_id_n.push_back(i * 255 / 2000);
            red_n.push_back(i * 4);
            green_n.push_back(i * 5);
            blue_n.push_back(i * 6);
        }

        REQUIRE_THROWS(points.X(Xn));
        REQUIRE_THROWS(points.Y(Yn));
        REQUIRE_THROWS(points.Z(Zn));
        REQUIRE_THROWS(points.Classification(classification_n));
        REQUIRE_THROWS(points.PointSourceId(point_source_id_n));
        REQUIRE_THROWS(points.Red(red_n));
        REQUIRE_THROWS(points.Green(green_n));
        REQUIRE_THROWS(points.Blue(blue_n));

        // add the last point
        Xn.push_back(1);
        Yn.push_back(2);
        Zn.push_back(3);
        classification_n.push_back(255);
        point_source_id_n.push_back(255);
        red_n.push_back(num_points * 4);
        green_n.push_back(num_points * 5);
        blue_n.push_back(num_points * 6);

        // test setters
        REQUIRE_NOTHROW(points.X(Xn));
        REQUIRE_NOTHROW(points.Y(Yn));
        REQUIRE_NOTHROW(points.Z(Zn));
        REQUIRE_NOTHROW(points.Classification(classification_n));
        REQUIRE_NOTHROW(points.PointSourceId(point_source_id_n));
        REQUIRE_NOTHROW(points.Red(red_n));
        REQUIRE_NOTHROW(points.Green(green_n));
        REQUIRE_NOTHROW(points.Blue(blue_n));

        for (int i = 0; i < num_points - 1; i++)
        {
            auto p = points.Get(i);
            REQUIRE(p->X() == i * 50 + 8);
            REQUIRE(p->Y() == i + 800);
            REQUIRE(p->Z() == i * 4);
            REQUIRE(p->Classification() == i * 255 / 2000);
            REQUIRE(p->PointSourceId() == i * 255 / 2000);
            REQUIRE(p->Red() == i * 4);
            REQUIRE(p->Green() == i * 5);
            REQUIRE(p->Blue() == i * 6);
        }

        // test last point
        auto last_point = points.Get(points.Size() - 1);
        REQUIRE(last_point->X() == 1);
        REQUIRE(last_point->Y() == 2);
        REQUIRE(last_point->Z() == 3);
        REQUIRE(last_point->Classification() == 255);
        REQUIRE(last_point->PointSourceId() == 255);
        REQUIRE(last_point->Red() == num_points * 4);
        REQUIRE(last_point->Green() == num_points * 5);
        REQUIRE(last_point->Blue() == num_points * 6);
    }

    SECTION("Points Indexers")
    {
        auto points = Points(6, 4);

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
        auto points = Points(6);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(0.0, 5.0);

        // generate points
        int num_points = 2000;
        for (int i = 0; i < num_points; i++)
        {
            auto p = points.CreatePoint();
            p->X(dist(gen));
            p->Y(dist(gen));
            p->Z(dist(gen));
            points.AddPoint(p);
        }

        REQUIRE(points.Within(copc::Box(0, 0, 0, 5, 5, 5)));

        auto p = points.CreatePoint();
        p->X(dist(gen));
        p->Y(dist(gen));
        p->Z(6);
        points.AddPoint(p);

        REQUIRE(!points.Within(copc::Box(0, 0, 0, 5, 5, 5)));
    }

    SECTION("GetWithin")
    {
        auto points = Points(6);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(0.0, 5.0);

        // generate points
        int num_points = 2000;
        for (int i = 0; i < num_points; i++)
        {
            auto p = points.CreatePoint();
            p->X(dist(gen));
            p->Y(dist(gen));
            p->Z(dist(gen));
            points.AddPoint(p);
        }
        auto box = copc::Box(0, 0, 0, 2.5, 2.5, 2.5);
        points = points.GetWithin(box);

        REQUIRE(points.Within(box));
    }
}
