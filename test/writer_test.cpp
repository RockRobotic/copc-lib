#include <catch2/catch.hpp>
#include <copc-lib/io/reader.hpp>
#include <copc-lib/io/writer.hpp>
#include <cstring>
#include <lazperf/readers.hpp>
#include <sstream>

using namespace copc;
using namespace std;

TEST_CASE("Writer Config Tests", "[Writer]")
{
    GIVEN("A valid output stream")
    {
        SECTION("Default Config")
        {
            stringstream out_stream;

            Writer::LasConfig cfg;
            Writer writer(out_stream, cfg);

            auto las_header = writer.file->GetLasHeader();
            REQUIRE(las_header.scale.z == 0.01);
            REQUIRE(las_header.offset.z == 0);
            REQUIRE(las_header.point_format_id == 0);

            writer.Close();

            lazperf::reader::generic_file f(out_stream);
            REQUIRE(f.pointCount() == 0);
            REQUIRE(f.header().scale.z == 0.01);
            REQUIRE(f.header().offset.z == 0);
            REQUIRE(f.header().point_format_id == 0);
        }

        SECTION("Custom Config")
        {
            stringstream out_stream;

            Writer::LasConfig cfg;
            cfg.file_source_id = 200;
            cfg.point_format_id = 8;
            cfg.scale = vector3{2, 3, 4};
            cfg.offset = vector3{-0.02, -0.03, -40.8};
            Writer writer(out_stream, cfg);

            auto las_header = writer.file->GetLasHeader();
            REQUIRE(las_header.file_source_id == 200);
            REQUIRE(las_header.point_format_id == 8);
            REQUIRE(las_header.scale.x == 2);
            REQUIRE(las_header.offset.x == -0.02);
            REQUIRE(las_header.scale.y == 3);
            REQUIRE(las_header.offset.y == -0.03);
            REQUIRE(las_header.scale.z == 4);
            REQUIRE(las_header.offset.z == -40.8);

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

        SECTION("COPC Span")
        {
            stringstream out_stream;

            Writer::LasConfig cfg;
            Writer writer(out_stream, cfg, 256);

            // todo: use Reader to check all of these
            auto span = writer.file->GetCopc().span;
            REQUIRE(span == 256);

            writer.Close();

            Reader reader(out_stream);
            REQUIRE(reader.file->GetCopc().span == 256);
        }

        SECTION("WKT")
        {
            stringstream out_stream;

            Writer::LasConfig cfg;
            Writer writer(out_stream, cfg, 256, "TEST_WKT");

            // todo: use Reader to check all of these
            REQUIRE(writer.file->GetWkt() == "TEST_WKT");

            writer.Close();

            Reader reader(out_stream);
            REQUIRE(reader.file->GetWkt() == "TEST_WKT");
        }
    }
}

TEST_CASE("Writer Pages", "[Writer]")
{
    SECTION("Root Page")
    {
        stringstream out_stream;

        Writer writer(out_stream, Writer::LasConfig());

        REQUIRE(!writer.FindNode(VoxelKey::BaseKey()).IsValid());
        REQUIRE(!writer.FindNode(VoxelKey::InvalidKey()).IsValid());
        REQUIRE(!writer.FindNode(VoxelKey(5, 4, 3, 2)).IsValid());

        REQUIRE_NOTHROW(writer.GetRootPage());
        Page root_page = writer.GetRootPage();
        REQUIRE(root_page.IsValid());
        REQUIRE(root_page.IsPage());
        REQUIRE(root_page.loaded == true);

        REQUIRE_THROWS(writer.AddSubPage(root_page, VoxelKey::InvalidKey()));

        writer.Close();

        Reader reader(out_stream);
        REQUIRE(reader.file->GetCopc().root_hier_offset > 0);
        REQUIRE(reader.file->GetCopc().root_hier_size == 0);
        REQUIRE(!reader.FindNode(VoxelKey::InvalidKey()).IsValid());
    }

    SECTION("Nested Page")
    {
        stringstream out_stream;

        Writer writer(out_stream, Writer::LasConfig());

        Page root_page = writer.GetRootPage();

        auto sub_page = writer.AddSubPage(root_page, VoxelKey(1,1,1,1));
        REQUIRE(sub_page.IsPage());
        REQUIRE(sub_page.IsValid());
        REQUIRE(sub_page.loaded == true);

        REQUIRE_THROWS(writer.AddSubPage(sub_page, VoxelKey(1, 1, 1, 0)));
        REQUIRE_THROWS(writer.AddSubPage(sub_page, VoxelKey(2, 4, 5, 0)));

        writer.Close();

        Reader reader(out_stream);
        REQUIRE(reader.file->GetCopc().root_hier_offset > 0);
        REQUIRE(reader.file->GetCopc().root_hier_size == 32);
        REQUIRE(!reader.FindNode(VoxelKey::InvalidKey()).IsValid());
    }
}

// TEST_CASE("Writer Node Uncompressed", "[Writer]")
//{
//    SECTION("Root Page")
//    {
//        stringstream out_stream;
//
//        Writer writer(out_stream, Writer::LasConfig());
//
//        REQUIRE(!writer.FindNode(VoxelKey::BaseKey()).IsValid());
//        REQUIRE(!writer.FindNode(VoxelKey::InvalidKey()).IsValid());
//        REQUIRE(!writer.FindNode(VoxelKey(5, 4, 3, 2)).IsValid());
//
//        REQUIRE_NOTHROW(writer.GetRootPage());
//        Page root_page = writer.GetRootPage();
//        REQUIRE(root_page.IsValid());
//        REQUIRE(root_page.loaded == true);
//
//        REQUIRE(writer.AddSubPage(VoxelKey::BaseKey()) == root_page);
//
//        REQUIRE_THROWS(writer.AddSubPage(VoxelKey::InvalidKey()));
//
//        writer.Close();
//
//        Reader reader(out_stream);
//        REQUIRE(reader.file->GetCopc().root_hier_offset > 0);
//        REQUIRE(reader.file->GetCopc().root_hier_size == 0);
//        REQUIRE(!reader.FindNode(VoxelKey::InvalidKey()).IsValid());
//    }
//}