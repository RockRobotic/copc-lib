#include <cstring>
#include <string>

#include <catch2/catch.hpp>
#include <copc-lib/geometry/vector3.hpp>
#include <copc-lib/io/laz_writer.hpp>

using namespace copc;
using namespace std;

TEST_CASE("Laz Writer Config Tests", "[LAZ Writer]")
{
    GIVEN("A valid file_path")
    {
        SECTION("Default Config")
        {
            string file_path = "writer_test.laz";

            las::LazConfigWriter cfg(6);
            laz::LazFileWriter writer(file_path, cfg);

            auto las_header = writer.LazConfig()->LasHeader();
            REQUIRE(las_header->Scale().z == 0.01);
            REQUIRE(las_header->Offset().z == 0);
            REQUIRE(las_header->PointFormatId() == 6);

            writer.Close();

            REQUIRE_THROWS(CopcConfigWriter(5));
            REQUIRE_THROWS(CopcConfigWriter(9));
        }

        SECTION("Custom Config")
        {
            string file_path = "writer_test.laz";

            CopcConfigWriter cfg(8, {2, 3, 4}, {-0.02, -0.03, -40.8});
            cfg.LasHeader()->file_source_id = 200;

            // Test checks on string attributes
            cfg.LasHeader()->SystemIdentifier("test_string");
            REQUIRE(cfg.LasHeader()->SystemIdentifier() == "test_string");
            REQUIRE_THROWS(cfg.LasHeader()->SystemIdentifier(std::string(33, 'a')));
            cfg.LasHeader()->GeneratingSoftware("test_string");
            REQUIRE(cfg.LasHeader()->GeneratingSoftware() == "test_string");
            REQUIRE_THROWS(cfg.LasHeader()->GeneratingSoftware(std::string(33, 'a')));

            laz::LazFileWriter writer(file_path, cfg);

            auto las_header = writer.LazConfig()->LasHeader();
            REQUIRE(las_header->file_source_id == 200);
            REQUIRE(las_header->PointFormatId() == 8);
            REQUIRE(las_header->Scale().x == 2);
            REQUIRE(las_header->Offset().x == -0.02);
            REQUIRE(las_header->Scale().y == 3);
            REQUIRE(las_header->Offset().y == -0.03);
            REQUIRE(las_header->Scale().z == 4);
            REQUIRE(las_header->Offset().z == -40.8);

            writer.Close();
        }

        SECTION("WKT")
        {
            string file_path = "writer_test.laz";

            CopcConfigWriter cfg(6, Vector3::DefaultScale(), Vector3::DefaultOffset(), "TEST_WKT");
            laz::LazFileWriter writer(file_path, cfg);

            REQUIRE(writer.LazConfig()->Wkt() == "TEST_WKT");

            writer.Close();
        }
    }

    GIVEN("An invalid filepath")
    {
        REQUIRE_THROWS(laz::LazFileWriter("invalid_path/writer_test.laz", CopcConfigWriter(6)));
    }

    GIVEN("A valid output stream")
    {
        SECTION("Default Config")
        {
            stringstream out_stream;

            CopcConfigWriter cfg(6);
            laz::LazWriter writer(out_stream, cfg);

            auto las_header = writer.LazConfig()->LasHeader();
            REQUIRE(las_header->Scale().z == 0.01);
            REQUIRE(las_header->Offset().z == 0);
            REQUIRE(las_header->PointFormatId() == 6);

            writer.Close();
        }

        SECTION("Custom Config")
        {
            stringstream out_stream;

            CopcConfigWriter cfg(8, {2, 3, 4}, {-0.02, -0.03, -40.8});
            cfg.LasHeader()->file_source_id = 200;
            laz::LazWriter writer(out_stream, cfg);

            auto las_header = writer.LazConfig()->LasHeader();
            REQUIRE(las_header->file_source_id == 200);
            REQUIRE(las_header->PointFormatId() == 8);
            REQUIRE(las_header->Scale().x == 2);
            REQUIRE(las_header->Offset().x == -0.02);
            REQUIRE(las_header->Scale().y == 3);
            REQUIRE(las_header->Offset().y == -0.03);
            REQUIRE(las_header->Scale().z == 4);
            REQUIRE(las_header->Offset().z == -40.8);

            writer.Close();
        }

        SECTION("WKT")
        {
            stringstream out_stream;

            CopcConfigWriter cfg(6, {}, {}, "TEST_WKT");
            laz::LazWriter writer(out_stream, cfg);

            REQUIRE(writer.LazConfig()->Wkt() == "TEST_WKT");

            writer.Close();
        }
    }
}

TEST_CASE("LAZ Writer EBs", "[LAZ Writer]")
{
    SECTION("Data type 0")
    {
        stringstream out_stream;
        las::EbVlr eb_vlr(1); // Always initialize with the ebCount constructor
        // don't make ebfields yourself unless you set their names correctly
        eb_vlr.items[0].data_type = 0;
        eb_vlr.items[0].options = 4;
        CopcConfigWriter cfg(7, {}, {}, {}, eb_vlr);
        laz::LazWriter writer(out_stream, cfg);

        REQUIRE(writer.LazConfig()->LasHeader()->PointRecordLength() == 40); // 36 + 4

        writer.Close();
    }

    SECTION("Data type 29")
    {
        stringstream out_stream;
        las::EbVlr eb_vlr(1);
        eb_vlr.items[0].data_type = 29;
        CopcConfigWriter cfg(7, {}, {}, {}, eb_vlr);
        laz::LazWriter writer(out_stream, cfg);

        REQUIRE(writer.LazConfig()->LasHeader()->PointRecordLength() == 48); // 36 + 12

        writer.Close();
    }
}

TEST_CASE("LAZ Write Points", "[LAZ Writer]")
{
    string file_path = "writer_test.laz";

    las::LazConfigWriter cfg(6);
    laz::LazFileWriter writer(file_path, cfg);

    // Create a Points object based on the LAS header
    las::Points points(*cfg.LasHeader());
    // Populate the points
    // Create a point with a given point format
    // The use of las::Point constructor is strongly discouraged, instead use las::Points::CreatePoint
    auto point = points.CreatePoint();
    // point has getters/setters for all attributes
    point->X(0);
    point->Y(0);
    point->Z(0);
    points.AddPoint(point);
    point = points.CreatePoint();
    // point has getters/setters for all attributes
    point->X(10);
    point->Y(10);
    point->Z(10);
    points.AddPoint(point);

    writer.WritePoints(points);
    REQUIRE(writer.PointCount() == 2);
    REQUIRE(writer.ChunkCount() == 1);
    writer.WritePoints(points);
    REQUIRE(writer.PointCount() == 4);
    REQUIRE(writer.ChunkCount() == 2);
    writer.Close();
}
