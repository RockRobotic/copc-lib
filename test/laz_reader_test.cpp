#include <catch2/catch.hpp>
#include <cmath>
#include <copc-lib/io/laz_reader.hpp>
#include <fstream>
#include <limits>

using namespace copc;
using namespace copc::laz;
using namespace std;

TEST_CASE("LazReader tests", "[LazReader]")
{
    GIVEN("A valid file path")
    {

        LazFileReader reader("autzen-classified.copc.laz");

        SECTION("GetHeader Test")
        {
            auto header = reader.LazConfig().LasHeader();
            REQUIRE(header.PointFormatId() == 7);
            REQUIRE(header.PointCount() == 10653336);
            REQUIRE(header.PointRecordLength() == 36);
            REQUIRE(header.EbByteSize() == 0);
        }

        SECTION("WKT")
        {
            auto wkt = reader.LazConfig().Wkt();
            REQUIRE(wkt.empty());
        }
    }

    GIVEN("An invalid file path") { REQUIRE_THROWS(LazFileReader("invalid_path/non_existant_file.copc.laz")); }

    GIVEN("A valid input stream")
    {
        fstream in_stream;
        in_stream.open("autzen-classified.copc.laz", ios::in | ios::binary);

        LazReader reader(&in_stream);

        SECTION("GetHeader Test")
        {
            auto header = reader.LazConfig().LasHeader();
            REQUIRE(header.PointFormatId() == 7);
            REQUIRE(header.PointCount() == 10653336);
        }

        SECTION("WKT")
        {
            auto wkt = reader.LazConfig().Wkt();
            REQUIRE(wkt.empty());
        }
    }
}

TEST_CASE("LazReader GetExtraByteVlrs Test", "[LazReader]")
{
    GIVEN("A valid file path")
    {
        LazFileReader reader("autzen-classified.copc.laz");

        auto eb_vlr = reader.LazConfig().ExtraBytesVlr();
        REQUIRE(eb_vlr.items.empty());
    }
}

