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

        FileReader reader("autzen-classified.copc.laz");

        SECTION("GetHeader Test")
        {
            auto header = reader.GetLasHeader();
            REQUIRE(header.header_size == 375);
            REQUIRE(header.point_format_id == 7);
            REQUIRE(header.point_count == 10653336);
            REQUIRE(header.point_record_length == 36);
            REQUIRE(header.EbByteSize() == 0);
        }

        SECTION("GetCopcInfo Test")
        {
            auto copc_info = reader.GetCopcInfo();
            REQUIRE_THAT(copc_info.center_x, Catch::Matchers::WithinAbs(637905.5448, 0.0001));
            REQUIRE_THAT(copc_info.center_y, Catch::Matchers::WithinAbs(851209.9048, 0.0001));
            REQUIRE_THAT(copc_info.center_z, Catch::Matchers::WithinAbs(2733.8948, 0.0001));
            REQUIRE_THAT(copc_info.halfsize, Catch::Matchers::WithinAbs(2327.7548, 0.0001));
            REQUIRE_THAT(copc_info.spacing, Catch::Matchers::WithinAbs(36.3711, 0.0001));
            REQUIRE(copc_info.root_hier_offset == 73017045);
            REQUIRE(copc_info.root_hier_size == 8896);
        }

        SECTION("GetCopcExtents Test")
        {
            auto copc_extents = reader.GetCopcExtents();
            REQUIRE_THAT(copc_extents.Intensity()->minimum, Catch::Matchers::WithinAbs(0, 0.0001));
            REQUIRE_THAT(copc_extents.Intensity()->maximum, Catch::Matchers::WithinAbs(254, 0.0001));
            REQUIRE_THAT(copc_extents.ReturnNumber()->minimum, Catch::Matchers::WithinAbs(1, 0.0001));
            REQUIRE_THAT(copc_extents.ReturnNumber()->maximum, Catch::Matchers::WithinAbs(4, 0.0001));
            REQUIRE_THAT(copc_extents.NumberOfReturns()->minimum, Catch::Matchers::WithinAbs(1, 0.0001));
            REQUIRE_THAT(copc_extents.NumberOfReturns()->maximum, Catch::Matchers::WithinAbs(4, 0.0001));
            REQUIRE_THAT(copc_extents.ScannerChannel()->minimum, Catch::Matchers::WithinAbs(0, 0.0001));
            REQUIRE_THAT(copc_extents.ScannerChannel()->maximum, Catch::Matchers::WithinAbs(0, 0.0001));
            REQUIRE_THAT(copc_extents.ScanDirectionFlag()->minimum, Catch::Matchers::WithinAbs(0, 0.0001));
            REQUIRE_THAT(copc_extents.ScanDirectionFlag()->maximum, Catch::Matchers::WithinAbs(1, 0.0001));
            REQUIRE_THAT(copc_extents.EdgeOfFlightLine()->minimum, Catch::Matchers::WithinAbs(0, 0.0001));
            REQUIRE_THAT(copc_extents.EdgeOfFlightLine()->maximum, Catch::Matchers::WithinAbs(0, 0.0001));
            REQUIRE_THAT(copc_extents.Classification()->minimum, Catch::Matchers::WithinAbs(0, 0.0001));
            REQUIRE_THAT(copc_extents.Classification()->maximum, Catch::Matchers::WithinAbs(77, 0.0001));
            REQUIRE_THAT(copc_extents.UserData()->minimum, Catch::Matchers::WithinAbs(115, 0.0001));
            REQUIRE_THAT(copc_extents.UserData()->maximum, Catch::Matchers::WithinAbs(156, 0.0001));
            REQUIRE_THAT(copc_extents.ScanAngle()->minimum, Catch::Matchers::WithinAbs(-21, 0.0001));
            REQUIRE_THAT(copc_extents.ScanAngle()->maximum, Catch::Matchers::WithinAbs(20, 0.0001));
            REQUIRE_THAT(copc_extents.PointSourceID()->minimum, Catch::Matchers::WithinAbs(7326, 0.0001));
            REQUIRE_THAT(copc_extents.PointSourceID()->maximum, Catch::Matchers::WithinAbs(7334, 0.0001));
            REQUIRE_THAT(copc_extents.GpsTime()->minimum, Catch::Matchers::WithinAbs(245369.89656857715, 0.0001));
            REQUIRE_THAT(copc_extents.GpsTime()->maximum, Catch::Matchers::WithinAbs(249783.70312432514, 0.0001));
            REQUIRE_THAT(copc_extents.Red()->minimum, Catch::Matchers::WithinAbs(4352, 0.0001));
            REQUIRE_THAT(copc_extents.Red()->maximum, Catch::Matchers::WithinAbs(65280, 0.0001));
            REQUIRE_THAT(copc_extents.Green()->minimum, Catch::Matchers::WithinAbs(0, 0.0001));
            REQUIRE_THAT(copc_extents.Green()->maximum, Catch::Matchers::WithinAbs(65280, 0.0001));
            REQUIRE_THAT(copc_extents.Blue()->minimum, Catch::Matchers::WithinAbs(1536, 0.0001));
            REQUIRE_THAT(copc_extents.Blue()->maximum, Catch::Matchers::WithinAbs(65280, 0.0001));
            REQUIRE_THROWS(copc_extents.NIR());
            REQUIRE(copc_extents.ExtraBytes().empty());
        }

        SECTION("WKT")
        {
            auto wkt = reader.GetWkt();
            REQUIRE(wkt.empty());
        }
    }

    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("autzen-classified.copc.laz", ios::in | ios::binary);

        Reader reader(&in_stream);

        SECTION("GetCopcInfo Test")
        {
            auto copc_info = reader.GetCopcInfo();
            REQUIRE_THAT(copc_info.center_x, Catch::Matchers::WithinAbs(637905.5448828125, 0.0001));
            REQUIRE_THAT(copc_info.center_y, Catch::Matchers::WithinAbs(851209.9048828125, 0.0001));
            REQUIRE_THAT(copc_info.center_z, Catch::Matchers::WithinAbs(2733.8948828125, 0.0001));
            REQUIRE_THAT(copc_info.halfsize, Catch::Matchers::WithinAbs(2327.7548828125, 0.0001));
            REQUIRE_THAT(copc_info.spacing, Catch::Matchers::WithinAbs(36.3711700439, 0.0001));
            REQUIRE(copc_info.root_hier_offset == 73017045);
            REQUIRE(copc_info.root_hier_size == 8896);
        }

        SECTION("GetHeader Test")
        {
            auto header = reader.GetLasHeader();
            REQUIRE(header.header_size == 375);
            REQUIRE(header.point_format_id == 7);
            REQUIRE(header.point_count == 10653336);
        }

        SECTION("WKT")
        {
            auto wkt = reader.GetWkt();
            REQUIRE(wkt.empty());
        }
    }
}

TEST_CASE("FindKey Check", "[Reader]")
{
    GIVEN("A valid file path")
    {
        FileReader reader("autzen-classified.copc.laz");

        auto key = VoxelKey::BaseKey();
        auto hier_entry = reader.FindNode(key);

        REQUIRE(hier_entry.IsValid() == true);
        REQUIRE(hier_entry.key == key);
        REQUIRE(hier_entry.point_count == 60978);

        key = VoxelKey(5, 9, 7, 0);
        hier_entry = reader.FindNode(key);

        REQUIRE(hier_entry.IsValid() == true);
        REQUIRE(hier_entry.key == key);
        REQUIRE(hier_entry.point_count == 12021);
    }
}

TEST_CASE("GetExtraByteVlrs Test", "[Reader]")
{
    GIVEN("A valid file path")
    {
        FileReader reader("autzen-classified.copc.laz");

        auto eb_vlr = reader.GetExtraByteVlr();
        REQUIRE(eb_vlr.items.size() == 0);

        //        REQUIRE(eb_vlr.items[0].data_type == 1);
        //        REQUIRE(eb_vlr.items[0].name == "FIELD_0");
        //
        //        REQUIRE(eb_vlr.items[1].data_type == 1);
        //        REQUIRE(eb_vlr.items[1].name == "FIELD_1");
    }
}

TEST_CASE("GetAllChildren Test", "[Reader]")
{
    GIVEN("A valid file path")
    {
        FileReader reader("autzen-classified.copc.laz");

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
    //    FileReader reader("autzen-classified.copc.laz");
    //    REQUIRE(reader.GetAllPoints().Get().size() == reader.GetLasHeader().point_count);
}

TEST_CASE("Point Error Handling Test", "[Reader]")
{
    GIVEN("A valid file path")
    {
        FileReader reader("autzen-classified.copc.laz");

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
    FileReader reader("autzen-classified.copc.laz");

    // Make horizontal 2D box of [200,200] roughly in the middle of the point cloud.
    auto middle = (reader.GetLasHeader().max + reader.GetLasHeader().min) / 2;
    Box middle_box(middle.x - 200, middle.y - 200, middle.x + 200, middle.y + 200);

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
        auto subset_nodes = reader.GetNodesIntersectBox(middle_box);
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
        }
        {
            // Take horizontal 2D box of [200,200] roughly in the middle of the point cloud.
            auto subset_points = reader.GetPointsWithinBox(middle_box);
            REQUIRE(subset_points.Get().size() == 91178);
        }
    }

    SECTION("GetDepthAtResolution")
    {
        REQUIRE(reader.GetDepthAtResolution(3) == 4);
        REQUIRE(reader.GetDepthAtResolution(0) == 5);
        REQUIRE(reader.GetDepthAtResolution(std::numeric_limits<double>::min()) == 5);
        REQUIRE(reader.GetDepthAtResolution(std::numeric_limits<double>::max()) == 0);
    }

    SECTION("GetNodesAtResolution")
    {
        auto subset_nodes = reader.GetNodesAtResolution(3);
        REQUIRE(subset_nodes.size() == 192);
        for (const auto &node : reader.GetNodesAtResolution(0))
            REQUIRE(node.key.d == 5);
    }

    SECTION("GetNodesWithinResolution")
    {
        auto subset_nodes = reader.GetNodesWithinResolution(3);
        REQUIRE(subset_nodes.size() == 257);
        REQUIRE(reader.GetNodesWithinResolution(0).size() == reader.GetAllChildren().size());
    }
}
