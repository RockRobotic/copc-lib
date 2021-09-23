#include <catch2/catch.hpp>
#include <cmath>
#include <copc-lib/io/reader.hpp>
#include <fstream>
#include <limits>

using namespace copc;
using namespace std;

TEST_CASE("Reader tests", "[Reader]")
{
    GIVEN("A valid file path")
    {

        FileReader reader("test/data/autzen-classified.copc.laz");

        SECTION("GetCopc Test")
        {
            auto copc = reader.GetCopcHeader();
            REQUIRE(copc.span == 0);
            REQUIRE(copc.root_hier_offset == 93169718);
            REQUIRE(copc.root_hier_size == 8896);
            REQUIRE(copc.laz_vlr_offset == 643);
            REQUIRE(copc.laz_vlr_size == 58);
            REQUIRE(copc.wkt_vlr_offset == 755);
            REQUIRE(copc.wkt_vlr_size == 993);
            REQUIRE(copc.eb_vlr_offset == 1802);
            REQUIRE(copc.eb_vlr_size == 384);
        }

        SECTION("GetHeader Test")
        {
            auto header = reader.GetLasHeader();
            REQUIRE(header.header_size == 375);
            REQUIRE(header.point_format_id == 3);
            REQUIRE(header.point_count == 10653336);
            REQUIRE(header.point_record_length == 36);
            REQUIRE(header.NumExtraBytes() == 2);
        }

        SECTION("WKT")
        {
            auto wkt = reader.GetWkt();
            REQUIRE(!wkt.empty());
            REQUIRE(wkt.rfind("COMPD_CS[", 0) == 0);
        }
    }

    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);

        Reader reader(&in_stream);

        SECTION("GetCopc Test")
        {
            auto copc = reader.GetCopcHeader();
            REQUIRE(copc.span == 0);
            REQUIRE(copc.root_hier_offset == 93169718);
            REQUIRE(copc.root_hier_size == 8896);
            REQUIRE(copc.laz_vlr_offset == 643);
            REQUIRE(copc.laz_vlr_size == 58);
            REQUIRE(copc.wkt_vlr_offset == 755);
            REQUIRE(copc.wkt_vlr_size == 993);
            REQUIRE(copc.eb_vlr_offset == 1802);
            REQUIRE(copc.eb_vlr_size == 384);
        }

        SECTION("GetHeader Test")
        {
            auto header = reader.GetLasHeader();
            REQUIRE(header.header_size == 375);
            REQUIRE(header.point_format_id == 3);
            REQUIRE(header.point_count == 10653336);
        }

        SECTION("WKT")
        {
            auto wkt = reader.GetWkt();
            REQUIRE(!wkt.empty());
            REQUIRE(wkt.rfind("COMPD_CS[", 0) == 0);
        }
    }
}

TEST_CASE("FindKey Check", "[Reader]")
{
    GIVEN("A valid file path")
    {
        FileReader reader("test/data/autzen-classified.copc.laz");

        auto key = VoxelKey::BaseKey();
        auto hier_entry = reader.FindNode(key);

        REQUIRE(hier_entry.IsValid() == true);
        REQUIRE(hier_entry.key == key);
        REQUIRE(hier_entry.point_count == 61022);

        key = VoxelKey(5, 9, 7, 0);
        hier_entry = reader.FindNode(key);

        REQUIRE(hier_entry.IsValid() == true);
        REQUIRE(hier_entry.key == key);
        REQUIRE(hier_entry.point_count == 12019);
    }
}

TEST_CASE("GetExtraByteVlrs Test", "[Reader]")
{
    GIVEN("A valid file path")
    {
        FileReader reader("test/data/autzen-classified.copc.laz");

        auto eb_vlr = reader.GetExtraByteVlr();
        REQUIRE(eb_vlr.items.size() == 2);

        REQUIRE(eb_vlr.items[0].data_type == 1);
        REQUIRE(eb_vlr.items[0].name == "FIELD_0");

        REQUIRE(eb_vlr.items[1].data_type == 1);
        REQUIRE(eb_vlr.items[1].name == "FIELD_1");
    }
}

TEST_CASE("GetAllChildren Test", "[Reader]")
{
    GIVEN("A valid file path")
    {
        FileReader reader("test/data/autzen-classified.copc.laz");

        {
            // Get root key
            auto nodes = reader.GetAllChildren();
            REQUIRE(nodes.size() == 278);
        }

        {
            // Get an invalid key
            auto nodes = reader.GetAllChildren(VoxelKey::InvalidKey());
            REQUIRE(nodes.empty());
        }

        {
            // Get an existing key
            auto nodes = reader.GetAllChildren(VoxelKey(5, 9, 7, 0));
            REQUIRE(nodes.size() == 1);
            REQUIRE(nodes[0].IsValid());
            REQUIRE(nodes[0].key == VoxelKey(5, 9, 7, 0));
        }

        {
            // Get a non-existing key
            auto nodes = reader.GetAllChildren(VoxelKey(20, 20, 20, 20));
            REQUIRE(nodes.empty());
        }
    }
}

// TODO[Leo]: Make this test optional
TEST_CASE("GetAllPoints Test", "[Reader]")
{
    //    FileReader reader("test/data/autzen-classified.copc.laz");
    //    REQUIRE(reader.GetAllPoints().Get().size() == reader.GetLasHeader().point_count);
}

TEST_CASE("Point Error Handling Test", "[Reader]")
{
    GIVEN("A valid file path")
    {
        FileReader reader("test/data/autzen-classified.copc.laz");

        Node invalid_node;
        REQUIRE_THROWS(reader.GetPointData(invalid_node));
        Node valid_node = reader.FindNode(VoxelKey(5, 9, 7, 0));
        REQUIRE_NOTHROW(reader.GetPointData(valid_node));

        REQUIRE(reader.GetPointData(invalid_node.key).empty());
        REQUIRE(!reader.GetPointData(valid_node.key).empty());

        REQUIRE_THROWS(reader.GetPoints(invalid_node));
        REQUIRE_NOTHROW(reader.GetPoints(valid_node));

        REQUIRE(reader.GetPoints(invalid_node.key).Get().empty());
        REQUIRE(!reader.GetPoints(valid_node.key).Get().empty());

        REQUIRE_THROWS(reader.GetPointDataCompressed(invalid_node));
        REQUIRE_NOTHROW(reader.GetPointDataCompressed(valid_node));
    }
}

TEST_CASE("Spatial Query Functions", "[Reader]")
{
    FileReader reader("test/data/autzen-classified.copc.laz");

    SECTION("GetNodesWithinBox")
    {
        // Check that no nodes fit in a zero-sized box
        auto subset_nodes = reader.GetNodesWithinBox(Box::ZeroBox());
        REQUIRE(subset_nodes.empty());

        // Check that all nodes fit in a max-sized box
        subset_nodes = reader.GetNodesWithinBox(Box::MaxBox());
        auto all_nodes = reader.GetAllChildren();
        REQUIRE(subset_nodes.size() == all_nodes.size());
    }

    SECTION("GetNodesIntersectBox")
    {
        // Take horizontal 2D box of [200,200] roughly in the middle of the point cloud.
        auto subset_nodes = reader.GetNodesIntersectBox(Box(637190, 851109, 637390, 851309));
        REQUIRE(subset_nodes.size() == 13);
    }

    SECTION("GetPointsWithinBox")
    {
        {
            // Check that no points fit in a zero-sized box
            auto subset_points = reader.GetPointsWithinBox(Box::ZeroBox());
            REQUIRE(subset_points.Get().empty());
        }
        {
            // TODO[Leo]: Make this test optional
            // Check that all points fit in a box sized from header
            //            auto header = reader.GetLasHeader();
            //            auto subset_points = reader.GetPointsWithinBox(Box(std::floor(header.min.x),
            //            std::floor(header.min.y),
            //                                                               std::floor(header.min.z),
            //                                                               std::ceil(header.max.x),
            //                                                               std::ceil(header.max.y),
            //                                                               std::ceil(header.max.z)));
            //            REQUIRE(subset_points.Get().size() == header.point_count);
        } {
            // Take horizontal 2D box of [200,200] roughly in the middle of the point cloud.
            auto subset_points = reader.GetPointsWithinBox(Box(637190, 851109, 637390, 851309));
            REQUIRE(subset_points.Get().size() == 22902);
        }
    }
}
