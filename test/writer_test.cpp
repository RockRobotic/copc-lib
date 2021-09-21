#include <catch2/catch.hpp>
#include <copc-lib/io/reader.hpp>
#include <copc-lib/io/writer.hpp>
#include <cstring>
#include <lazperf/readers.hpp>
#include <sstream>
#include <string>

using namespace copc;
using namespace std;

TEST_CASE("Writer Config Tests", "[Writer]")
{
    GIVEN("A valid file_path")
    {
        SECTION("Default Config")
        {
            string file_path = "test/data/writer_test.copc.laz";

            Writer::LasHeaderConfig cfg(0);
            FileWriter writer(file_path, cfg);

            auto las_header = writer.GetLasHeader();
            REQUIRE(las_header.scale.z == 0.01);
            REQUIRE(las_header.offset.z == 0);
            REQUIRE(las_header.point_format_id == 0);

            cfg.ToString();

            writer.Close();
        }

        SECTION("Custom Config")
        {
            string file_path = "test/data/writer_test.copc.laz";

            Writer::LasHeaderConfig cfg(8, {2, 3, 4}, {-0.02, -0.03, -40.8});
            cfg.file_source_id = 200;

            // Test checks on string attributes
            cfg.SystemIdentifier("test_string");
            REQUIRE(cfg.SystemIdentifier() == "test_string");
            REQUIRE_THROWS(cfg.SystemIdentifier(std::string(33, 'a')));
            cfg.GeneratingSoftware("test_string");
            REQUIRE(cfg.GeneratingSoftware() == "test_string");
            REQUIRE_THROWS(cfg.GeneratingSoftware(std::string(33, 'a')));

            FileWriter writer(file_path, cfg);

            auto las_header = writer.GetLasHeader();
            REQUIRE(las_header.file_source_id == 200);
            REQUIRE(las_header.point_format_id == 8);
            REQUIRE(las_header.scale.x == 2);
            REQUIRE(las_header.offset.x == -0.02);
            REQUIRE(las_header.scale.y == 3);
            REQUIRE(las_header.offset.y == -0.03);
            REQUIRE(las_header.scale.z == 4);
            REQUIRE(las_header.offset.z == -40.8);

            writer.Close();
        }

        SECTION("COPC Span")
        {
            string file_path = "test/data/writer_test.copc.laz";

            Writer::LasHeaderConfig cfg(0);
            FileWriter writer(file_path, cfg, 256);

            // todo: use Reader to check all of these
            auto span = writer.GetCopcHeader().span;
            REQUIRE(span == 256);

            writer.Close();

            FileReader reader(file_path);
            REQUIRE(reader.GetCopcHeader().span == 256);
        }

        SECTION("WKT")
        {
            string file_path = "test/data/writer_test.copc.laz";

            Writer::LasHeaderConfig cfg(0);
            FileWriter writer(file_path, cfg, 256, "TEST_WKT");

            // todo: use Reader to check all of these
            REQUIRE(writer.GetWkt() == "TEST_WKT");

            writer.Close();

            FileReader reader(file_path);
            REQUIRE(reader.GetWkt() == "TEST_WKT");
        }

        SECTION("Copy")
        {
            FileReader orig("test/data/autzen-classified.copc.laz");

            string file_path = "test/data/writer_test.copc.laz";
            Writer::LasHeaderConfig cfg(orig.GetLasHeader(), orig.GetExtraByteVlr());
            FileWriter writer(file_path, cfg);
            writer.Close();

            FileReader reader(file_path);
            REQUIRE(reader.GetLasHeader().file_source_id == orig.GetLasHeader().file_source_id);
            REQUIRE(reader.GetLasHeader().global_encoding == orig.GetLasHeader().global_encoding);
            REQUIRE(reader.GetLasHeader().creation_day == orig.GetLasHeader().creation_day);
            REQUIRE(reader.GetLasHeader().creation_year == orig.GetLasHeader().creation_year);
            REQUIRE(reader.GetLasHeader().file_source_id == orig.GetLasHeader().file_source_id);
            REQUIRE(reader.GetLasHeader().point_format_id == orig.GetLasHeader().point_format_id);
            REQUIRE(reader.GetLasHeader().point_record_length == orig.GetLasHeader().point_record_length);
            REQUIRE(reader.GetLasHeader().point_count == 0);
            REQUIRE(reader.GetLasHeader().scale == reader.GetLasHeader().scale);
            REQUIRE(reader.GetLasHeader().offset == reader.GetLasHeader().offset);
        }
    }
    GIVEN("A valid output stream")
    {
        SECTION("Default Config")
        {
            stringstream out_stream;

            Writer::LasHeaderConfig cfg(0);
            Writer writer(out_stream, cfg);

            auto las_header = writer.GetLasHeader();
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

            Writer::LasHeaderConfig cfg(8, {2, 3, 4}, {-0.02, -0.03, -40.8});
            cfg.file_source_id = 200;
            Writer writer(out_stream, cfg);

            auto las_header = writer.GetLasHeader();
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

            Writer::LasHeaderConfig cfg(0);
            Writer writer(out_stream, cfg, 256);

            // todo: use Reader to check all of these
            auto span = writer.GetCopcHeader().span;
            REQUIRE(span == 256);

            writer.Close();

            Reader reader(&out_stream);
            REQUIRE(reader.GetCopcHeader().span == 256);
        }

        SECTION("WKT")
        {
            stringstream out_stream;

            Writer::LasHeaderConfig cfg(0);
            Writer writer(out_stream, cfg, 256, "TEST_WKT");

            // todo: use Reader to check all of these
            REQUIRE(writer.GetWkt() == "TEST_WKT");

            writer.Close();

            Reader reader(&out_stream);
            REQUIRE(reader.GetWkt() == "TEST_WKT");
        }

        SECTION("Copy")
        {
            fstream in_stream;
            in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
            Reader orig(&in_stream);

            stringstream out_stream;
            Writer::LasHeaderConfig cfg(orig.GetLasHeader(), orig.GetExtraByteVlr());
            Writer writer(out_stream, cfg);
            writer.Close();

            Reader reader(&out_stream);
            REQUIRE(reader.GetLasHeader().file_source_id == orig.GetLasHeader().file_source_id);
            REQUIRE(reader.GetLasHeader().global_encoding == orig.GetLasHeader().global_encoding);
            REQUIRE(reader.GetLasHeader().creation_day == orig.GetLasHeader().creation_day);
            REQUIRE(reader.GetLasHeader().creation_year == orig.GetLasHeader().creation_year);
            REQUIRE(reader.GetLasHeader().file_source_id == orig.GetLasHeader().file_source_id);
            REQUIRE(reader.GetLasHeader().point_format_id == orig.GetLasHeader().point_format_id);
            REQUIRE(reader.GetLasHeader().point_record_length == orig.GetLasHeader().point_record_length);
            REQUIRE(reader.GetLasHeader().point_count == 0);
            REQUIRE(reader.GetLasHeader().scale == reader.GetLasHeader().scale);
            REQUIRE(reader.GetLasHeader().offset == reader.GetLasHeader().offset);
        }
    }
}

TEST_CASE("Writer Pages", "[Writer]")
{
    SECTION("Root Page")
    {
        stringstream out_stream;

        Writer writer(out_stream, Writer::LasHeaderConfig(0));

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

        Reader reader(&out_stream);
        REQUIRE(reader.GetCopcHeader().root_hier_offset > 0);
        REQUIRE(reader.GetCopcHeader().root_hier_size == 0);
        REQUIRE(!reader.FindNode(VoxelKey::InvalidKey()).IsValid());
    }

    SECTION("Nested Page")
    {
        stringstream out_stream;

        Writer writer(out_stream, Writer::LasHeaderConfig(0));

        Page root_page = writer.GetRootPage();

        auto sub_page = writer.AddSubPage(root_page, VoxelKey(1, 1, 1, 1));
        REQUIRE(sub_page.IsPage());
        REQUIRE(sub_page.IsValid());
        REQUIRE(sub_page.loaded == true);

        REQUIRE_THROWS(writer.AddSubPage(sub_page, VoxelKey(1, 1, 1, 0)));
        REQUIRE_THROWS(writer.AddSubPage(sub_page, VoxelKey(2, 4, 5, 0)));

        writer.Close();

        Reader reader(&out_stream);
        REQUIRE(reader.GetCopcHeader().root_hier_offset > 0);
        REQUIRE(reader.GetCopcHeader().root_hier_size == 32);
        REQUIRE(!reader.FindNode(VoxelKey::InvalidKey()).IsValid());
    }
}

TEST_CASE("Writer EBs", "[Writer]")
{
    SECTION("Data type 0")
    {
        stringstream out_stream;
        Writer::LasHeaderConfig config(7);
        las::EbVlr eb_vlr(1); // Always initialize with the ebCount constructor
        // don't make ebfields yourself unless you set their names correctly
        eb_vlr.items[0].data_type = 0;
        eb_vlr.items[0].options = 4;
        config.extra_bytes = eb_vlr;
        Writer writer(out_stream, config);

        REQUIRE(writer.GetLasHeader().point_record_length == 40); // 36 + 4

        writer.Close();

        Reader reader(&out_stream);
        REQUIRE(reader.GetExtraByteVlr().items.size() == 1);
        REQUIRE(reader.GetExtraByteVlr().items[0].data_type == 0);
        REQUIRE(reader.GetExtraByteVlr().items[0].options == 4);
        REQUIRE(reader.GetExtraByteVlr().items[0].name == "FIELD_0");
        REQUIRE(reader.GetExtraByteVlr().items[0].maxval[2] == 0);
        REQUIRE(reader.GetExtraByteVlr().items[0].minval[2] == 0);
        REQUIRE(reader.GetExtraByteVlr().items[0].offset[2] == 0);
        REQUIRE(reader.GetExtraByteVlr().items[0].scale[2] == 0);
        REQUIRE(reader.GetLasHeader().point_record_length == 40);
    }

    SECTION("Data type 29")
    {
        stringstream out_stream;
        Writer::LasHeaderConfig config(7);
        las::EbVlr eb_vlr(1);
        eb_vlr.items[0].data_type = 29;
        config.extra_bytes = eb_vlr;
        Writer writer(out_stream, config);

        REQUIRE(writer.GetLasHeader().point_record_length == 48); // 36 + 12

        writer.Close();

        Reader reader(&out_stream);
        REQUIRE(reader.GetExtraByteVlr().items.size() == 1);
        REQUIRE(reader.GetLasHeader().point_record_length == 48);
    }

    SECTION("Copy Vlr")
    {
        FileReader reader("test/data/autzen-classified.copc.laz");

        auto in_eb_vlr = reader.GetExtraByteVlr();

        stringstream out_stream;
        Writer::LasHeaderConfig config(3);
        config.extra_bytes = in_eb_vlr;
        Writer writer(out_stream, config);

        REQUIRE(writer.GetLasHeader().point_record_length == 36); // 34 + 1 + 1

        writer.Close();

        Reader reader2(&out_stream);
        REQUIRE(reader2.GetExtraByteVlr().items.size() == 2);
        REQUIRE(reader2.GetLasHeader().point_record_length == reader.GetLasHeader().point_record_length);
        REQUIRE(reader2.GetExtraByteVlr().items == reader.GetExtraByteVlr().items);
    }
}

TEST_CASE("Writer Copy", "[Writer]")
{
    SECTION("Autzen")
    {
        FileReader reader("test/data/autzen-classified.copc.laz");

        stringstream out_stream;
        Writer::LasHeaderConfig cfg(reader.GetLasHeader(), reader.GetExtraByteVlr());
        Writer writer(out_stream, cfg);

        Page root_page = writer.GetRootPage();

        for (auto node : reader.GetAllChildren())
        {
            // only write/compare compressed data or otherwise tests take too long
            writer.AddNodeCompressed(root_page, node.key, reader.GetPointDataCompressed(node), node.point_count);
        }

        writer.Close();

        Reader new_reader(&out_stream);

        for (auto node : reader.GetAllChildren())
        {
            REQUIRE(node.IsValid());
            auto new_node = new_reader.FindNode(node.key);
            REQUIRE(new_node.IsValid());
            REQUIRE(new_node.key == node.key);
            REQUIRE(new_node.point_count == node.point_count);
            REQUIRE(new_node.byte_size == node.byte_size);
            REQUIRE(new_reader.GetPointDataCompressed(new_node) == reader.GetPointDataCompressed(node));
        }

        // we can do one uncompressed comparison here
        REQUIRE(new_reader.GetPointData(new_reader.FindNode(VoxelKey(5, 9, 7, 0))) ==
                reader.GetPointData(reader.FindNode(VoxelKey(5, 9, 7, 0))));
    }
}
