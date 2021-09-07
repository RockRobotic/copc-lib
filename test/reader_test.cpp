#include <catch2/catch.hpp>
#include <cfloat>
#include <copc-lib/copc/file.hpp>
#include <copc-lib/io/reader.hpp>
#include <cstring>
#include <fstream>

using namespace copc;
using namespace std;

TEST_CASE("Reader tests", "[Reader]")
{
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);

        Reader reader(in_stream);

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
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
        Reader reader(in_stream);

        auto key = VoxelKey(0, 0, 0, 0);
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

TEST_CASE("GetExtraByteVlrs Test", "[Reader] ")
{
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
        Reader reader(in_stream);

        auto eb_vlr = reader.GetExtraByteVlr();
        REQUIRE(eb_vlr.items.size() == 2);

        REQUIRE(eb_vlr.items[0].data_type == 1);
        REQUIRE(eb_vlr.items[0].name == "FIELD_0");

        REQUIRE(eb_vlr.items[1].data_type == 1);
        REQUIRE(eb_vlr.items[1].name == "FIELD_1");
    }
}

TEST_CASE("GetAllChildren Test", "[Reader] ")
{
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
        Reader reader(in_stream);

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
