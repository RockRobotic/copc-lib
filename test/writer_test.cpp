#include <catch2/catch.hpp>
#include <copc-lib/io/writer.hpp>
#include <lazperf/readers.hpp>
#include <cstring>
#include <sstream>

using namespace copc::io;
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

            auto copc_header = writer.file->GetCopcHeader();
            REQUIRE(copc_header.span == 256);

            writer.Close();
        }
    }
}