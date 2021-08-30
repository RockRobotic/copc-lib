#include <copc-lib/copc/file.hpp>
#include <catch2/catch.hpp>
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

        CopcFile file(in_stream);

        SECTION("GetCopc Test")
        {
            auto copc = file.GetCopcHeader();
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
            auto header = file.GetLasHeader();
            REQUIRE(header.header_size == 375);
            REQUIRE(header.point_format_id == 3);
            REQUIRE(header.point_count == 10653336);
        }

        SECTION("WKT")
        {
            auto wkt = file.GetWkt();
            REQUIRE(wkt.size() > 0);
            REQUIRE(wkt.rfind("COMPD_CS[", 0) == 0);
        }
    }
}