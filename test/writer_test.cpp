#include <cstring>
#include <sstream>
#include <string>

#include <catch2/catch.hpp>
#include <copc-lib/copc/extents.hpp>
#include <copc-lib/geometry/vector3.hpp>
#include <copc-lib/io/reader.hpp>
#include <copc-lib/io/writer.hpp>
#include <lazperf/readers.hpp>

using namespace copc;
using namespace std;

TEST_CASE("Writer Config Tests", "[Writer]")
{
    GIVEN("A valid file_path")
    {
        SECTION("Default Config")
        {
            string file_path = "writer_test.copc.laz";

            CopcConfigWriter cfg(6);
            FileWriter writer(file_path, cfg);

            auto las_header = writer.CopcConfig()->LasHeader();
            REQUIRE(las_header->Scale().z == 0.01);
            REQUIRE(las_header->Offset().z == 0);
            REQUIRE(las_header->PointFormatId() == 6);

            writer.Close();

            REQUIRE_THROWS(CopcConfigWriter(5));
            REQUIRE_THROWS(CopcConfigWriter(9));
        }

        SECTION("Custom Config")
        {
            string file_path = "writer_test.copc.laz";

            CopcConfigWriter cfg(8, {2, 3, 4}, {-0.02, -0.03, -40.8});
            cfg.LasHeader()->file_source_id = 200;

            // Test checks on string attributes
            cfg.LasHeader()->SystemIdentifier("test_string");
            REQUIRE(cfg.LasHeader()->SystemIdentifier() == "test_string");
            REQUIRE_THROWS(cfg.LasHeader()->SystemIdentifier(std::string(33, 'a')));
            cfg.LasHeader()->GeneratingSoftware("test_string");
            REQUIRE(cfg.LasHeader()->GeneratingSoftware() == "test_string");
            REQUIRE_THROWS(cfg.LasHeader()->GeneratingSoftware(std::string(33, 'a')));

            FileWriter writer(file_path, cfg);

            auto las_header = writer.CopcConfig()->LasHeader();
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

        SECTION("COPC Spacing")
        {
            string file_path = "writer_test.copc.laz";

            {
                CopcConfigWriter cfg(6);
                cfg.CopcInfo()->spacing = 10;
                FileWriter writer(file_path, cfg);

                REQUIRE(writer.CopcConfig()->CopcInfo()->spacing == 10);

                writer.Close();
            }
            FileReader reader(file_path);
            REQUIRE(reader.CopcConfig().CopcInfo().spacing == 10);
        }

        SECTION("Extents")
        {
            string file_path = "writer_test.copc.laz";

            CopcConfigWriter cfg(6);
            FileWriter writer(file_path, cfg);

            auto extents = writer.CopcConfig()->CopcExtents();
            extents->Intensity()->minimum = -1.0;
            extents->Intensity()->maximum = 1;

            extents->Classification()->minimum = -std::numeric_limits<double>::max();
            extents->Classification()->maximum = std::numeric_limits<double>::max();

            REQUIRE(writer.CopcConfig()->CopcExtents()->Intensity()->minimum == extents->Intensity()->minimum);
            REQUIRE(writer.CopcConfig()->CopcExtents()->Intensity()->maximum == extents->Intensity()->maximum);
            REQUIRE(writer.CopcConfig()->CopcExtents()->Classification()->minimum ==
                    extents->Classification()->minimum);
            REQUIRE(writer.CopcConfig()->CopcExtents()->Classification()->maximum ==
                    extents->Classification()->maximum);

            writer.Close();

            // Test reading of extents
            FileReader reader(file_path);
            REQUIRE(reader.CopcConfig().CopcExtents().Intensity()->minimum == extents->Intensity()->minimum);
            REQUIRE(reader.CopcConfig().CopcExtents().Intensity()->maximum == extents->Intensity()->maximum);
            REQUIRE(reader.CopcConfig().CopcExtents().Classification()->minimum == extents->Classification()->minimum);
            REQUIRE(reader.CopcConfig().CopcExtents().Classification()->maximum == extents->Classification()->maximum);
        }

        SECTION("WKT")
        {
            string file_path = "writer_test.copc.laz";

            CopcConfigWriter cfg(6, Vector3::DefaultScale(), Vector3::DefaultOffset(), "TEST_WKT");
            FileWriter writer(file_path, cfg);

            REQUIRE(writer.CopcConfig()->Wkt() == "TEST_WKT");

            writer.Close();

            FileReader reader(file_path);
            REQUIRE(reader.CopcConfig().Wkt() == "TEST_WKT");
        }
        SECTION("Copy")
        {
            FileReader orig("autzen-classified.copc.laz");

            string file_path = "writer_test.copc.laz";
            auto cfg = orig.CopcConfig();
            FileWriter writer(file_path, cfg);
            writer.Close();

            FileReader reader(file_path);
            REQUIRE(reader.CopcConfig().LasHeader().file_source_id == orig.CopcConfig().LasHeader().file_source_id);
            REQUIRE(reader.CopcConfig().LasHeader().global_encoding == orig.CopcConfig().LasHeader().global_encoding);
            REQUIRE(reader.CopcConfig().LasHeader().creation_day == orig.CopcConfig().LasHeader().creation_day);
            REQUIRE(reader.CopcConfig().LasHeader().creation_year == orig.CopcConfig().LasHeader().creation_year);
            REQUIRE(reader.CopcConfig().LasHeader().file_source_id == orig.CopcConfig().LasHeader().file_source_id);
            REQUIRE(reader.CopcConfig().LasHeader().PointFormatId() == orig.CopcConfig().LasHeader().PointFormatId());
            REQUIRE(reader.CopcConfig().LasHeader().PointRecordLength() ==
                    orig.CopcConfig().LasHeader().PointRecordLength());
            REQUIRE(reader.CopcConfig().LasHeader().PointCount() == 0);
            REQUIRE(reader.CopcConfig().LasHeader().Scale() == reader.CopcConfig().LasHeader().Scale());
            REQUIRE(reader.CopcConfig().LasHeader().Offset() == reader.CopcConfig().LasHeader().Offset());
        }
    }
    GIVEN("A valid output stream")
    {
        SECTION("Default Config")
        {
            stringstream out_stream;

            CopcConfigWriter cfg(6);
            Writer writer(out_stream, cfg);

            auto las_header = writer.CopcConfig()->LasHeader();
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

            CopcConfigWriter cfg(8, {2, 3, 4}, {-0.02, -0.03, -40.8});
            cfg.LasHeader()->file_source_id = 200;
            Writer writer(out_stream, cfg);

            auto las_header = writer.CopcConfig()->LasHeader();
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

        SECTION("COPC Spacing")
        {
            stringstream out_stream;

            CopcConfigWriter cfg(6);
            cfg.CopcInfo()->spacing = 10;
            Writer writer(out_stream, cfg);

            // todo: use Reader to check all of these
            auto spacing = writer.CopcConfig()->CopcInfo()->spacing;
            REQUIRE(spacing == 10);

            writer.Close();

            Reader reader(&out_stream);
            REQUIRE(reader.CopcConfig().CopcInfo().spacing == 10);
        }

        SECTION("WKT")
        {
            stringstream out_stream;

            CopcConfigWriter cfg(6, {}, {}, "TEST_WKT");
            Writer writer(out_stream, cfg);

            REQUIRE(writer.CopcConfig()->Wkt() == "TEST_WKT");

            writer.Close();

            Reader reader(&out_stream);
            REQUIRE(reader.CopcConfig().Wkt() == "TEST_WKT");
        }

        SECTION("Copy")
        {
            fstream in_stream;
            in_stream.open("autzen-classified.copc.laz", ios::in | ios::binary);
            Reader orig(&in_stream);

            stringstream out_stream;
            auto cfg = orig.CopcConfig();

            Writer writer(out_stream, cfg);
            writer.Close();

            Reader reader(&out_stream);
            REQUIRE(reader.CopcConfig().LasHeader().file_source_id == orig.CopcConfig().LasHeader().file_source_id);
            REQUIRE(reader.CopcConfig().LasHeader().global_encoding == orig.CopcConfig().LasHeader().global_encoding);
            REQUIRE(reader.CopcConfig().LasHeader().creation_day == orig.CopcConfig().LasHeader().creation_day);
            REQUIRE(reader.CopcConfig().LasHeader().creation_year == orig.CopcConfig().LasHeader().creation_year);
            REQUIRE(reader.CopcConfig().LasHeader().file_source_id == orig.CopcConfig().LasHeader().file_source_id);
            REQUIRE(reader.CopcConfig().LasHeader().PointFormatId() == orig.CopcConfig().LasHeader().PointFormatId());
            REQUIRE(reader.CopcConfig().LasHeader().PointRecordLength() ==
                    orig.CopcConfig().LasHeader().PointRecordLength());
            REQUIRE(reader.CopcConfig().LasHeader().PointCount() == 0);
            REQUIRE(reader.CopcConfig().LasHeader().Scale() == reader.CopcConfig().LasHeader().Scale());
            REQUIRE(reader.CopcConfig().LasHeader().Offset() == reader.CopcConfig().LasHeader().Offset());
        }

        SECTION("Update")
        {
            stringstream out_stream;
            const Vector3 min1 = {-800, 300, 800};
            const Vector3 max1 = {5000, 8444, 3333};
            const Vector3 min2 = {-20, -30, -40};
            const Vector3 max2 = {20, 30, 40};
            std::array<uint64_t, 15> points_by_return = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

            CopcConfigWriter cfg(6, {}, {}, "TEST_WKT");
            cfg.LasHeader()->min = min1;
            cfg.LasHeader()->max = max1;
            cfg.CopcInfo()->spacing = 10;
            Writer writer(out_stream, cfg);

            REQUIRE(writer.CopcConfig()->LasHeader()->min == min1);
            REQUIRE(writer.CopcConfig()->LasHeader()->max == max1);
            REQUIRE(writer.CopcConfig()->LasHeader()->points_by_return == std::array<uint64_t, 15>{0});

            writer.CopcConfig()->LasHeader()->min = min2;
            writer.CopcConfig()->LasHeader()->max = max2;
            writer.CopcConfig()->LasHeader()->points_by_return = points_by_return;

            REQUIRE(writer.CopcConfig()->LasHeader()->min == min2);
            REQUIRE(writer.CopcConfig()->LasHeader()->max == max2);
            REQUIRE(writer.CopcConfig()->LasHeader()->points_by_return == points_by_return);

            writer.Close();

            Reader reader(&out_stream);
            REQUIRE(reader.CopcConfig().LasHeader().min == min2);
            REQUIRE(reader.CopcConfig().LasHeader().max == max2);
            REQUIRE(reader.CopcConfig().LasHeader().points_by_return == points_by_return);
        }
    }
}

TEST_CASE("Writer Pages", "[Writer]")
{
    SECTION("Root Page")
    {
        stringstream out_stream;

        Writer writer(out_stream, {6});

        REQUIRE(!writer.FindNode(VoxelKey::RootKey()).IsValid());
        REQUIRE(!writer.FindNode(VoxelKey::InvalidKey()).IsValid());
        REQUIRE(!writer.FindNode(VoxelKey(5, 4, 3, 2)).IsValid());
        auto header = *writer.CopcConfig()->LasHeader();
        las::Points points(header.PointFormatId(), header.Scale(), header.Offset());
        points.AddPoint(points.CreatePoint());
        // Add a node with root key as page
        writer.AddNode(VoxelKey(1, 1, 1, 1), points, VoxelKey::RootKey());

        writer.Close();

        Reader reader(&out_stream);
        REQUIRE(reader.CopcConfig().CopcInfo().root_hier_offset > 0);
        REQUIRE(reader.CopcConfig().CopcInfo().root_hier_size == 32);
        REQUIRE(!reader.FindNode(VoxelKey::InvalidKey()).IsValid());
    }

    SECTION("Nested Page")
    {
        stringstream out_stream;

        Writer writer(out_stream, {6});

        auto header = *writer.CopcConfig()->LasHeader();
        las::Points points(header.PointFormatId(), header.Scale(), header.Offset());
        points.AddPoint(points.CreatePoint());

        writer.AddNode(VoxelKey(1, 1, 1, 1), points, VoxelKey(1, 1, 1, 1));
        writer.AddNode(VoxelKey(3, 4, 4, 4), points, VoxelKey(2, 2, 2, 2));
        writer.AddNode(VoxelKey(2, 0, 2, 2), points, VoxelKey(1, 0, 1, 1));

        writer.Close();

        Reader reader(&out_stream);
        REQUIRE(reader.CopcConfig().CopcInfo().root_hier_offset > 0);
        REQUIRE(reader.CopcConfig().CopcInfo().root_hier_size == 64); // size of two sub pages of the root page
        REQUIRE(!reader.FindNode(VoxelKey::InvalidKey()).IsValid());

        auto page_keys = reader.GetAllPageKeys();
        REQUIRE(page_keys.size() == 4);
        REQUIRE(std::find(page_keys.begin(), page_keys.end(), VoxelKey(1, 1, 1, 1)) != page_keys.end());
        REQUIRE(std::find(page_keys.begin(), page_keys.end(), VoxelKey(2, 2, 2, 2)) != page_keys.end());
        REQUIRE(std::find(page_keys.begin(), page_keys.end(), VoxelKey(1, 0, 1, 1)) != page_keys.end());
    }
}

TEST_CASE("Writer EBs", "[Writer]")
{
    SECTION("Data type 0")
    {
        stringstream out_stream;
        las::EbVlr eb_vlr(1); // Always initialize with the ebCount constructor
        // don't make ebfields yourself unless you set their names correctly
        eb_vlr.items[0].data_type = 0;
        eb_vlr.items[0].options = 4;
        CopcConfigWriter cfg(7, {}, {}, {}, eb_vlr);
        Writer writer(out_stream, cfg);

        REQUIRE(writer.CopcConfig()->LasHeader()->PointRecordLength() == 40); // 36 + 4

        writer.Close();

        Reader reader(&out_stream);
        REQUIRE(reader.CopcConfig().ExtraBytesVlr().items.size() == 1);
        REQUIRE(reader.CopcConfig().ExtraBytesVlr().items[0].data_type == 0);
        REQUIRE(reader.CopcConfig().ExtraBytesVlr().items[0].options == 4);
        REQUIRE(reader.CopcConfig().ExtraBytesVlr().items[0].name == "FIELD_0");
        REQUIRE(reader.CopcConfig().ExtraBytesVlr().items[0].maxval[2] == 0);
        REQUIRE(reader.CopcConfig().ExtraBytesVlr().items[0].minval[2] == 0);
        REQUIRE(reader.CopcConfig().ExtraBytesVlr().items[0].offset[2] == 0);
        REQUIRE(reader.CopcConfig().ExtraBytesVlr().items[0].scale[2] == 0);
        REQUIRE(reader.CopcConfig().LasHeader().PointRecordLength() == 40);
    }

    SECTION("Data type 29")
    {
        stringstream out_stream;
        las::EbVlr eb_vlr(1);
        eb_vlr.items[0].data_type = 29;
        CopcConfigWriter cfg(7, {}, {}, {}, eb_vlr);
        Writer writer(out_stream, cfg);

        REQUIRE(writer.CopcConfig()->LasHeader()->PointRecordLength() == 48); // 36 + 12

        writer.Close();

        Reader reader(&out_stream);
        REQUIRE(reader.CopcConfig().ExtraBytesVlr().items.size() == 1);
        REQUIRE(reader.CopcConfig().LasHeader().PointRecordLength() == 48);
    }
}
TEST_CASE("Writer Copy", "[Writer]")
{

    SECTION("Copy Copc Config")
    {
        FileReader reader("autzen-classified.copc.laz");

        stringstream out_stream;
        auto cfg = reader.CopcConfig();

        Writer writer(out_stream, cfg);

        writer.CopcConfig()->LasHeader()->min = {1, 1, 1}; // Update some values in the process
        writer.CopcConfig()->LasHeader()->max = {50, 50, 50};

        REQUIRE(writer.CopcConfig()->LasHeader()->PointRecordLength() ==
                reader.CopcConfig().LasHeader().PointRecordLength());
        REQUIRE(writer.CopcConfig()->LasHeader()->Scale() == reader.CopcConfig().LasHeader().Scale());
        REQUIRE(writer.CopcConfig()->LasHeader()->Offset() == reader.CopcConfig().LasHeader().Offset());
        REQUIRE(writer.CopcConfig()->LasHeader()->min == Vector3(1, 1, 1));
        REQUIRE(writer.CopcConfig()->LasHeader()->max == Vector3(50, 50, 50));

        writer.Close();

        Reader new_reader(&out_stream);
        REQUIRE(new_reader.CopcConfig().LasHeader().PointRecordLength() ==
                reader.CopcConfig().LasHeader().PointRecordLength());
        REQUIRE(new_reader.CopcConfig().LasHeader().Scale() == reader.CopcConfig().LasHeader().Scale());
        REQUIRE(new_reader.CopcConfig().LasHeader().Offset() == reader.CopcConfig().LasHeader().Offset());
        REQUIRE(new_reader.CopcConfig().LasHeader().min == Vector3(1, 1, 1));
        REQUIRE(new_reader.CopcConfig().LasHeader().max == Vector3(50, 50, 50));
        REQUIRE(new_reader.CopcConfig().CopcInfo().spacing == reader.CopcConfig().CopcInfo().spacing);
        REQUIRE(new_reader.CopcConfig().CopcExtents().Intensity()->minimum ==
                reader.CopcConfig().CopcExtents().Intensity()->minimum);
        REQUIRE(new_reader.CopcConfig().Wkt() == reader.CopcConfig().Wkt());
        REQUIRE(new_reader.CopcConfig().ExtraBytesVlr().items == reader.CopcConfig().ExtraBytesVlr().items);
    }

    SECTION("Autzen")
    {
        FileReader reader("autzen-classified.copc.laz");

        stringstream out_stream;

        auto cfg = reader.CopcConfig();

        Writer writer(out_stream, cfg);

        for (const auto &node : reader.GetAllNodes())
        {
            // only write/compare compressed data or otherwise tests take too long
            writer.AddNodeCompressed(node.key, reader.GetPointDataCompressed(node), node.point_count);
        }

        writer.Close();

        Reader new_reader(&out_stream);

        for (const auto &node : reader.GetAllNodes())
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

TEST_CASE("Check Spatial Bounds", "[Writer]")
{
    string file_path = "writer_test.copc.laz";
    CopcConfigWriter cfg(7, {0.1, 0.1, 0.1}, {50, 50, 50});
    cfg.LasHeader()->min = {-10, -10, -5};
    cfg.LasHeader()->max = {10, 10, 5};
    bool verbose = false;

    SECTION("Las Header Bounds check (pass)")
    {
        FileWriter writer(file_path, cfg);

        auto header = *writer.CopcConfig()->LasHeader();

        las::Points points(header.PointFormatId(), header.Scale(), header.Offset());

        auto point = points.CreatePoint();
        point->X(10);
        point->Y(10);
        point->Z(5);

        points.AddPoint(point);
        writer.AddNode({1, 1, 1, 1}, points);
        writer.Close();

        FileReader reader(file_path);

        REQUIRE(reader.ValidateSpatialBounds(verbose) == true);
    }

    SECTION("Las Header Bounds check (node outside)")
    {
        FileWriter writer(file_path, cfg);

        auto header = *writer.CopcConfig()->LasHeader();

        las::Points points(header.PointFormatId(), header.Scale(), header.Offset());

        auto point = points.CreatePoint();
        point->X(10);
        point->Y(10);
        point->Z(5.1);

        points.AddPoint(point);
        writer.AddNode({2, 3, 3, 3}, points);
        writer.Close();

        FileReader reader(file_path);

        REQUIRE(reader.ValidateSpatialBounds(verbose) == false);
    }

    SECTION("Las Header Bounds check (node intersects)")
    {
        FileWriter writer(file_path, cfg);

        auto header = *writer.CopcConfig()->LasHeader();

        las::Points points(header.PointFormatId(), header.Scale(), header.Offset());

        auto point = points.CreatePoint();
        point->X(10);
        point->Y(10);
        point->Z(5.1);

        points.AddPoint(point);
        writer.AddNode({1, 1, 1, 1}, points);
        writer.Close();

        FileReader reader(file_path);

        REQUIRE(reader.ValidateSpatialBounds(verbose) == false);
    }

    SECTION("Node Bounds check")
    {
        FileWriter writer(file_path, cfg);

        auto header = *writer.CopcConfig()->LasHeader();

        las::Points points(header.PointFormatId(), header.Scale(), header.Offset());

        auto point = points.CreatePoint();
        point->X(0.1);
        point->Y(0.1);
        point->Z(0.1);

        points.AddPoint(point);
        writer.AddNode({1, 0, 0, 0}, points);
        writer.Close();

        FileReader reader(file_path);
        REQUIRE(reader.ValidateSpatialBounds(verbose) == false);
    }
}
