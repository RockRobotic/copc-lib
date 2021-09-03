#include "data/test_data.h"
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

            Writer::LasConfig cfg;
            cfg.file_source_id = 200;
            cfg.point_format_id = 8;
            cfg.scale = vector3{2, 3, 4};
            cfg.offset = vector3{-0.02, -0.03, -40.8};
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

            Writer::LasConfig cfg;
            Writer writer(out_stream, cfg, 256);

            // todo: use Reader to check all of these
            auto span = writer.GetCopcHeader().span;
            REQUIRE(span == 256);

            writer.Close();

            Reader reader(out_stream);
            REQUIRE(reader.GetCopcHeader().span == 256);
        }

        SECTION("WKT")
        {
            stringstream out_stream;

            Writer::LasConfig cfg;
            Writer writer(out_stream, cfg, 256, "TEST_WKT");

            // todo: use Reader to check all of these
            REQUIRE(writer.GetWkt() == "TEST_WKT");

            writer.Close();

            Reader reader(out_stream);
            REQUIRE(reader.GetWkt() == "TEST_WKT");
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
        REQUIRE(reader.GetCopcHeader().root_hier_offset > 0);
        REQUIRE(reader.GetCopcHeader().root_hier_size == 0);
        REQUIRE(!reader.FindNode(VoxelKey::InvalidKey()).IsValid());
    }

    SECTION("Nested Page")
    {
        stringstream out_stream;

        Writer writer(out_stream, Writer::LasConfig());

        Page root_page = writer.GetRootPage();

        auto sub_page = writer.AddSubPage(root_page, VoxelKey(1, 1, 1, 1));
        REQUIRE(sub_page.IsPage());
        REQUIRE(sub_page.IsValid());
        REQUIRE(sub_page.loaded == true);

        REQUIRE_THROWS(writer.AddSubPage(sub_page, VoxelKey(1, 1, 1, 0)));
        REQUIRE_THROWS(writer.AddSubPage(sub_page, VoxelKey(2, 4, 5, 0)));

        writer.Close();

        Reader reader(out_stream);
        REQUIRE(reader.GetCopcHeader().root_hier_offset > 0);
        REQUIRE(reader.GetCopcHeader().root_hier_size == 32);
        REQUIRE(!reader.FindNode(VoxelKey::InvalidKey()).IsValid());
    }
}

TEST_CASE("Writer Node Uncompressed", "[Writer]")
{
    SECTION("Add one")
    {
        stringstream out_stream;

        Writer::LasConfig cfg;
        cfg.point_format_id = 3;
        Writer writer(out_stream, cfg);

        Page root_page = writer.GetRootPage();

        REQUIRE_THROWS(writer.AddNode(root_page, VoxelKey::InvalidKey(), std::vector<char>()));
        REQUIRE_THROWS(writer.AddNode(root_page, VoxelKey(0, 0, 0, 0), std::vector<char>()));

        std::vector<char> root_node(twenty_pts_prdf3, twenty_pts_prdf3 + sizeof(twenty_pts_prdf3));
        REQUIRE_NOTHROW(writer.AddNode(root_page, VoxelKey(0, 0, 0, 0), root_node));

        writer.Close();

        Reader reader(out_stream);
        REQUIRE(reader.GetCopcHeader().root_hier_offset > 0);
        REQUIRE(reader.GetCopcHeader().root_hier_size == 32);

        auto node = reader.FindNode(VoxelKey::BaseKey());
        REQUIRE(node.IsValid());
        auto root_node_test = reader.GetPointData(node);

        REQUIRE(root_node_test == root_node);
    }

    SECTION("Add multiple")
    {
        stringstream out_stream;

        Writer::LasConfig cfg;
        cfg.point_format_id = 3;
        Writer writer(out_stream, cfg, 256, "test_wkt");

        Page root_page = writer.GetRootPage();

        std::vector<char> twenty(twenty_pts_prdf3, twenty_pts_prdf3 + sizeof(twenty_pts_prdf3));
        REQUIRE_NOTHROW(writer.AddNode(root_page, VoxelKey(0, 0, 0, 0), twenty));

        std::vector<char> twelve(twelve_pts_prdf3, twelve_pts_prdf3 + sizeof(twelve_pts_prdf3));
        REQUIRE_NOTHROW(writer.AddNode(root_page, VoxelKey(1, 1, 1, 1), twelve));

        std::vector<char> sixty(sixty_pts_prdf3, sixty_pts_prdf3 + sizeof(sixty_pts_prdf3));
        REQUIRE_NOTHROW(writer.AddNode(root_page, VoxelKey(1, 1, 1, 0), sixty));

        writer.Close();

        std::string ostr = out_stream.str();
        Reader reader(out_stream);
        REQUIRE(reader.GetCopcHeader().root_hier_offset > 0);
        REQUIRE(reader.GetCopcHeader().root_hier_size == 32 * 3);

        {
            auto node = reader.FindNode(VoxelKey::BaseKey());
            REQUIRE(node.IsValid());
            auto node_data = reader.GetPointData(node);
            REQUIRE(node_data == twenty);
        }

        {
            auto node = reader.FindNode(VoxelKey(1, 1, 1, 1));
            REQUIRE(node.IsValid());
            auto node_data = reader.GetPointData(node);
            REQUIRE(node_data == twelve);
        }

        {
            auto node = reader.FindNode(VoxelKey(1, 1, 1, 0));
            REQUIRE(node.IsValid());
            auto node_data = reader.GetPointData(node);
            REQUIRE(node_data == sixty);
        }
    }

    SECTION("Add hierarchy")
    {
        stringstream out_stream;

        Writer::LasConfig cfg;
        cfg.point_format_id = 3;
        Writer writer(out_stream, cfg);

        Page root_page = writer.GetRootPage();
        Page sub_page1 = writer.AddSubPage(root_page, VoxelKey(1, 0, 0, 0));
        Page sub_page2 = writer.AddSubPage(root_page, VoxelKey(1, 1, 1, 1));

        std::vector<char> twenty(twenty_pts_prdf3, twenty_pts_prdf3 + sizeof(twenty_pts_prdf3));
        REQUIRE_NOTHROW(writer.AddNode(root_page, VoxelKey(0, 0, 0, 0), twenty));

        std::vector<char> twelve(twelve_pts_prdf3, twelve_pts_prdf3 + sizeof(twelve_pts_prdf3));
        REQUIRE_NOTHROW(writer.AddNode(sub_page1, VoxelKey(1, 0, 0, 0), twelve));

        std::vector<char> sixty(sixty_pts_prdf3, sixty_pts_prdf3 + sizeof(sixty_pts_prdf3));
        REQUIRE_NOTHROW(writer.AddNode(sub_page2, VoxelKey(1, 1, 1, 1), sixty));
        REQUIRE_NOTHROW(writer.AddNode(sub_page2, VoxelKey(2, 2, 2, 2), twenty));

        // Can't add a node that's not a child of the page
        REQUIRE_THROWS(writer.AddNode(sub_page2, VoxelKey(1, 2, 2, 2), twenty));

        writer.Close();

        /*
            hierarchy should look like this:
            Page 0-0-0-0
                Node 0-0-0-0
                Page 1-0-0-0
                    Node 1-0-0-0
                Page 1-1-1-1
                    Node 1-1-1-1
                    Node 2-2-2-2
        */

        Reader reader(out_stream);
        REQUIRE(reader.GetCopcHeader().root_hier_offset > 0);
        REQUIRE(reader.GetCopcHeader().root_hier_size == 32 * 3);

        {
            auto sub_node = reader.FindNode(VoxelKey(2, 2, 2, 2));
            REQUIRE(sub_node.IsValid());
            auto sub_node_data = reader.GetPointData(sub_node);
            REQUIRE(sub_node_data == twenty);
        }
        {
            auto sub_node = reader.FindNode(VoxelKey(1, 1, 1, 1));
            REQUIRE(sub_node.IsValid());
            auto sub_node_data = reader.GetPointData(sub_node);
            REQUIRE(sub_node_data == sixty);
        }
        {
            auto sub_node = reader.FindNode(VoxelKey(1, 0, 0, 0));
            REQUIRE(sub_node.IsValid());
            auto sub_node_data = reader.GetPointData(sub_node);
            REQUIRE(sub_node_data == twelve);
        }
        {
            auto sub_node = reader.FindNode(VoxelKey(0, 0, 0, 0));
            REQUIRE(sub_node.IsValid());
            auto sub_node_data = reader.GetPointData(sub_node);
            REQUIRE(sub_node_data == twenty);
        }
    }
}

TEST_CASE("Writer Node Compressed", "[Writer]")
{
    SECTION("Add one")
    {
        stringstream out_stream;

        Writer::LasConfig cfg;
        cfg.point_format_id = 3;
        Writer writer(out_stream, cfg);

        Page root_page = writer.GetRootPage();

        REQUIRE_THROWS(writer.AddNodeCompressed(root_page, VoxelKey::InvalidKey(), std::vector<char>(), 0));

        std::vector<char> root_node(twenty_pts_prdf3_compressed,
                                    twenty_pts_prdf3_compressed + sizeof(twenty_pts_prdf3_compressed));
        REQUIRE_NOTHROW(writer.AddNodeCompressed(root_page, VoxelKey(0, 0, 0, 0), root_node, 20));

        writer.Close();

        Reader reader(out_stream);
        REQUIRE(reader.GetCopcHeader().root_hier_offset > 0);
        REQUIRE(reader.GetCopcHeader().root_hier_size == 32);

        auto node = reader.FindNode(VoxelKey::BaseKey());
        REQUIRE(node.IsValid());
        auto root_node_test = reader.GetPointData(node);

        REQUIRE(root_node_test == std::vector<char>(twenty_pts_prdf3, twenty_pts_prdf3 + sizeof(twenty_pts_prdf3)));
    }

    SECTION("Add multiple")
    {
        stringstream out_stream;

        Writer::LasConfig cfg;
        cfg.point_format_id = 3;
        Writer writer(out_stream, cfg, 256, "test_wkt");

        Page root_page = writer.GetRootPage();

        std::vector<char> twenty(twenty_pts_prdf3_compressed,
                                 twenty_pts_prdf3_compressed + sizeof(twenty_pts_prdf3_compressed));
        REQUIRE_NOTHROW(writer.AddNodeCompressed(root_page, VoxelKey(0, 0, 0, 0), twenty, 20));

        std::vector<char> twelve(twelve_pts_prdf3_compressed,
                                 twelve_pts_prdf3_compressed + sizeof(twelve_pts_prdf3_compressed));
        REQUIRE_NOTHROW(writer.AddNodeCompressed(root_page, VoxelKey(1, 1, 1, 1), twelve, 12));

        std::vector<char> sixty(sixty_pts_prdf3_compressed,
                                sixty_pts_prdf3_compressed + sizeof(sixty_pts_prdf3_compressed));
        REQUIRE_NOTHROW(writer.AddNodeCompressed(root_page, VoxelKey(1, 1, 1, 0), sixty, 60));

        writer.Close();

        std::string ostr = out_stream.str();
        Reader reader(out_stream);
        REQUIRE(reader.GetCopcHeader().root_hier_offset > 0);
        REQUIRE(reader.GetCopcHeader().root_hier_size == 32 * 3);

        {
            auto sub_node = reader.FindNode(VoxelKey::BaseKey());
            REQUIRE(sub_node.IsValid());
            auto sub_node_data = reader.GetPointData(sub_node);
            REQUIRE(sub_node_data == std::vector<char>(twenty_pts_prdf3, twenty_pts_prdf3 + sizeof(twenty_pts_prdf3)));
        }

        {
            auto sub_node = reader.FindNode(VoxelKey(1, 1, 1, 1));
            REQUIRE(sub_node.IsValid());
            auto sub_node_data = reader.GetPointData(sub_node);
            REQUIRE(sub_node_data == std::vector<char>(twelve_pts_prdf3, twelve_pts_prdf3 + sizeof(twelve_pts_prdf3)));
        }

        {
            auto sub_node = reader.FindNode(VoxelKey(1, 1, 1, 0));
            REQUIRE(sub_node.IsValid());
            auto sub_node_data = reader.GetPointData(sub_node);
            REQUIRE(sub_node_data == std::vector<char>(sixty_pts_prdf3, sixty_pts_prdf3 + sizeof(sixty_pts_prdf3)));
        }
    }

    SECTION("Add hierarchy")
    {
        stringstream out_stream;

        Writer::LasConfig cfg;
        cfg.point_format_id = 3;
        Writer writer(out_stream, cfg);

        Page root_page = writer.GetRootPage();
        Page sub_page1 = writer.AddSubPage(root_page, VoxelKey(1, 0, 0, 0));
        Page sub_page2 = writer.AddSubPage(root_page, VoxelKey(1, 1, 1, 1));

        std::vector<char> twenty(twenty_pts_prdf3_compressed,
                                 twenty_pts_prdf3_compressed + sizeof(twenty_pts_prdf3_compressed));
        REQUIRE_NOTHROW(writer.AddNodeCompressed(root_page, VoxelKey(0, 0, 0, 0), twenty, 20));

        std::vector<char> twelve(twelve_pts_prdf3_compressed,
                                 twelve_pts_prdf3_compressed + sizeof(twelve_pts_prdf3_compressed));
        REQUIRE_NOTHROW(writer.AddNodeCompressed(sub_page1, VoxelKey(1, 0, 0, 0), twelve, 12));

        std::vector<char> sixty(sixty_pts_prdf3_compressed,
                                sixty_pts_prdf3_compressed + sizeof(sixty_pts_prdf3_compressed));
        REQUIRE_NOTHROW(writer.AddNodeCompressed(sub_page2, VoxelKey(1, 1, 1, 1), sixty, 60));
        REQUIRE_NOTHROW(writer.AddNodeCompressed(sub_page2, VoxelKey(2, 2, 2, 2), twenty, 20));

        // Can't add a node that's not a child of the page
        REQUIRE_THROWS(writer.AddNodeCompressed(sub_page2, VoxelKey(1, 2, 2, 2), twenty, 20));

        writer.Close();

        /*
            hierarchy should look like this:
            Page 0-0-0-0
                Node 0-0-0-0
                Page 1-0-0-0
                    Node 1-0-0-0
                Page 1-1-1-1
                    Node 1-1-1-1
                    Node 2-2-2-2
        */

        Reader reader(out_stream);
        REQUIRE(reader.GetCopcHeader().root_hier_offset > 0);
        REQUIRE(reader.GetCopcHeader().root_hier_size == 32 * 3);

        {
            auto sub_node = reader.FindNode(VoxelKey(2, 2, 2, 2));
            REQUIRE(sub_node.IsValid());
            auto sub_node_data = reader.GetPointData(sub_node);
            REQUIRE(sub_node_data == std::vector<char>(twenty_pts_prdf3, twenty_pts_prdf3 + sizeof(twenty_pts_prdf3)));
        }
        {
            auto sub_node = reader.FindNode(VoxelKey(1, 1, 1, 1));
            REQUIRE(sub_node.IsValid());
            auto sub_node_data = reader.GetPointData(sub_node);
            REQUIRE(sub_node_data == std::vector<char>(sixty_pts_prdf3, sixty_pts_prdf3 + sizeof(sixty_pts_prdf3)));
        }
        {
            auto sub_node = reader.FindNode(VoxelKey(1, 0, 0, 0));
            REQUIRE(sub_node.IsValid());
            auto sub_node_data = reader.GetPointData(sub_node);
            REQUIRE(sub_node_data == std::vector<char>(twelve_pts_prdf3, twelve_pts_prdf3 + sizeof(twelve_pts_prdf3)));
        }
        {
            auto sub_node = reader.FindNode(VoxelKey(0, 0, 0, 0));
            REQUIRE(sub_node.IsValid());
            auto sub_node_data = reader.GetPointData(sub_node);
            REQUIRE(sub_node_data == std::vector<char>(twenty_pts_prdf3, twenty_pts_prdf3 + sizeof(twenty_pts_prdf3)));
        }
    }
}
