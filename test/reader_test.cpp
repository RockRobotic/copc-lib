#include "reader.hpp"
#include <catch2/catch.hpp>
#include <cstring>

using namespace copc;

TEST_CASE("Reader tests", "[Reader]")
{
    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);

        io::CopcReader reader(in_stream);

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
            auto header = reader.GetHeader();
            REQUIRE(header.header_size == 375);
            REQUIRE(header.point_format_id == 3);
            REQUIRE(header.point_count == 10653336);
        }

        SECTION("ReadPage Test")
        {
            auto copc = reader.GetCopcHeader();
            auto rootPage = reader.ReadPage(copc.root_hier_offset, copc.root_hier_size);
            REQUIRE(rootPage.size() == 278);
            auto rootNode = rootPage[0];
            REQUIRE(rootNode.key == VoxelKey(0, 0, 0, 0));
            REQUIRE(rootNode.offset == 90837784);
            REQUIRE(rootNode.byteSize == 1082752);
            REQUIRE(rootNode.pointCount == 61022);
        }

        SECTION("GetPoints Test")
        {
            auto copc = reader.GetCopcHeader();
            auto header = reader.GetHeader();
            auto rootPage = reader.ReadPage(copc.root_hier_offset, copc.root_hier_size);

            auto point_vec = reader.GetPoints(rootPage[0]);
            auto point_buff = point_vec.data();

            int x;
            std::memcpy(&x, point_buff, sizeof(x));
            REQUIRE(x == -144147);
            std::memcpy(&x, point_buff + header.point_record_length, sizeof(x));
            REQUIRE(x == -172520);
            std::memcpy(&x, point_buff + header.point_record_length * 2, sizeof(x));
            REQUIRE(x == -121603);
            std::memcpy(&x, point_buff + header.point_record_length * 3, sizeof(x));
            REQUIRE(x == -49828);
            std::memcpy(&x, point_buff + header.point_record_length * 4, sizeof(x));
            REQUIRE(x == -138082);
        }

        SECTION("Point Count") { REQUIRE(reader.GetPointCount() == 10653336); }

        SECTION("WKT")
        {
            auto wkt = reader.GetWkt();
            REQUIRE(wkt.size() > 0);
            REQUIRE(wkt.rfind("COMPD_CS[", 0) == 0);
        }
    }
}