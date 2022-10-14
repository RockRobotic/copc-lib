#include <cstring>
#include <string>

#include <catch2/catch.hpp>
#include <copc-lib/geometry/vector3.hpp>
#include <copc-lib/io/laz_reader.hpp>
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

            // Test updating the values from the LasHeader getter
            writer.LazConfig()->LasHeader()->max = Vector3(10, 11, 12);

            auto las_header = writer.LazConfig()->LasHeader();
            REQUIRE(las_header->Scale().z == 0.01);
            REQUIRE(las_header->Offset().z == 0);
            REQUIRE(las_header->PointFormatId() == 6);
            REQUIRE(las_header->max == Vector3(10, 11, 12));
            REQUIRE(las_header->min == Vector3(0, 0, 0));

            writer.Close();

            REQUIRE_THROWS(las::LazConfigWriter(5));
            REQUIRE_THROWS(las::LazConfigWriter(9));
        }

        SECTION("Custom Config")
        {
            string file_path = "writer_test.laz";

            las::LazConfigWriter cfg(8, {2, 3, 4}, {-0.02, -0.03, -40.8});
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

            las::LazConfigWriter cfg(6, Vector3::DefaultScale(), Vector3::DefaultOffset(), "TEST_WKT");
            laz::LazFileWriter writer(file_path, cfg);

            REQUIRE(writer.LazConfig()->Wkt() == "TEST_WKT");

            writer.Close();

            laz::LazFileReader reader(file_path);
            REQUIRE(reader.LazConfig().Wkt() == "TEST_WKT");
        }
    }

    GIVEN("An invalid filepath")
    {
        REQUIRE_THROWS(laz::LazFileWriter("invalid_path/writer_test.laz", las::LazConfigWriter(6)));
    }

    GIVEN("A valid output stream")
    {
        SECTION("Default Config")
        {
            stringstream out_stream;

            las::LazConfigWriter cfg(6);
            laz::LazWriter writer(out_stream, cfg);

            auto las_header = writer.LazConfig()->LasHeader();
            REQUIRE(las_header->Scale().z == 0.01);
            REQUIRE(las_header->Offset().z == 0);
            REQUIRE(las_header->PointFormatId() == 6);

            writer.Close();

            lazperf::reader::generic_file f(out_stream);
            REQUIRE(f.pointCount() == 0);
            REQUIRE(f.header().scale.z == 0.01);
            REQUIRE(f.header().offset.z == 0);
            REQUIRE(f.header().point_format_id == 6);
        }

        SECTION("Custom Config")
        {
            stringstream out_stream;

            las::LazConfigWriter cfg(8, {2, 3, 4}, {-0.02, -0.03, -40.8});
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

            lazperf::reader::generic_file f(out_stream);
            REQUIRE(f.pointCount() == 0);
            REQUIRE(f.header().file_source_id == 200);
            REQUIRE(f.header().point_format_id == 8);
            REQUIRE(f.header().scale.x == 2);
            REQUIRE(f.header().offset.x == -0.02);
            REQUIRE(f.header().scale.y == 3);
            REQUIRE(f.header().offset.y == -0.03);
            REQUIRE(f.header().scale.z == 4);
            REQUIRE(f.header().offset.z == -40.8);
        }

        SECTION("WKT")
        {
            stringstream out_stream;

            las::LazConfigWriter cfg(6, {}, {}, "TEST_WKT");
            laz::LazWriter writer(out_stream, cfg);

            REQUIRE(writer.LazConfig()->Wkt() == "TEST_WKT");

            writer.Close();

            laz::LazReader reader(&out_stream);
            REQUIRE(reader.LazConfig().Wkt() == "TEST_WKT");
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
        las::LazConfigWriter cfg(7, {}, {}, {}, eb_vlr);
        laz::LazWriter writer(out_stream, cfg);

        REQUIRE(writer.LazConfig()->LasHeader()->PointRecordLength() == 40); // 36 + 4

        writer.Close();

        laz::LazReader reader(&out_stream);
        REQUIRE(reader.LazConfig().ExtraBytesVlr().items.size() == 1);
        REQUIRE(reader.LazConfig().ExtraBytesVlr().items[0].data_type == 0);
        REQUIRE(reader.LazConfig().ExtraBytesVlr().items[0].options == 4);
        REQUIRE(reader.LazConfig().ExtraBytesVlr().items[0].name == "FIELD_0");
        REQUIRE(reader.LazConfig().ExtraBytesVlr().items[0].maxval[2] == 0);
        REQUIRE(reader.LazConfig().ExtraBytesVlr().items[0].minval[2] == 0);
        REQUIRE(reader.LazConfig().ExtraBytesVlr().items[0].offset[2] == 0);
        REQUIRE(reader.LazConfig().ExtraBytesVlr().items[0].scale[2] == 0);
        REQUIRE(reader.LazConfig().LasHeader().PointRecordLength() == 40);
    }

    SECTION("Data type 29")
    {
        stringstream out_stream;
        las::EbVlr eb_vlr(1);
        eb_vlr.items[0].data_type = 29;
        las::LazConfigWriter cfg(7, {}, {}, {}, eb_vlr);
        laz::LazWriter writer(out_stream, cfg);

        REQUIRE(writer.LazConfig()->LasHeader()->PointRecordLength() == 48); // 36 + 12

        writer.Close();

        laz::LazReader reader(&out_stream);
        REQUIRE(reader.LazConfig().ExtraBytesVlr().items.size() == 1);
        REQUIRE(reader.LazConfig().LasHeader().PointRecordLength() == 48);
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
    point->X(11);
    point->Y(12);
    point->Z(13);
    points.AddPoint(point);

    writer.WritePoints(points);
    writer.WritePoints(points);
    writer.Close();

    // Validate
    laz::LazFileReader reader(file_path);
    auto read_points = reader.GetPoints();
    REQUIRE(read_points.Size() == 4);
    REQUIRE(read_points.Get(0)->X() == 0);
    REQUIRE(read_points.Get(0)->Y() == 0);
    REQUIRE(read_points.Get(0)->Z() == 0);
    REQUIRE(read_points.Get(1)->X() == 11);
    REQUIRE(read_points.Get(1)->Y() == 12);
    REQUIRE(read_points.Get(1)->Z() == 13);
    REQUIRE(read_points.Get(2)->X() == 0);
    REQUIRE(read_points.Get(2)->Y() == 0);
    REQUIRE(read_points.Get(2)->Z() == 0);
    REQUIRE(read_points.Get(3)->X() == 11);
    REQUIRE(read_points.Get(3)->Y() == 12);
    REQUIRE(read_points.Get(3)->Z() == 13);
}
