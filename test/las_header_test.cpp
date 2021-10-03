#include <vector>

#include <catch2/catch.hpp>
#include <copc-lib/io/reader.hpp>
#include <copc-lib/las/header.hpp>

using namespace copc;

TEST_CASE("Test constructor and conversions", "[LasHeader]")
{
    GIVEN("A valid file_path")
    {
        FileReader reader("autzen-classified.copc.laz");
        auto las_header = reader.GetLasHeader();
        auto lazperf_header = las_header.ToLazPerf();
        auto las_header_orig = las::LasHeader::FromLazPerf(lazperf_header);

        REQUIRE(las_header_orig.file_source_id == las_header.file_source_id);
        REQUIRE(las_header_orig.global_encoding == las_header.global_encoding);
        REQUIRE(las_header_orig.GUID() == las_header.GUID());
        REQUIRE(las_header_orig.version_major == las_header.version_major);
        REQUIRE(las_header_orig.version_minor == las_header.version_minor);
        REQUIRE(las_header_orig.SystemIdentifier() == las_header.SystemIdentifier());
        REQUIRE(las_header_orig.GeneratingSoftware() == las_header.GeneratingSoftware());
        REQUIRE(las_header_orig.creation_day == las_header.creation_day);
        REQUIRE(las_header_orig.creation_year == las_header.creation_year);
        REQUIRE(las_header_orig.header_size == las_header.header_size);
        REQUIRE(las_header_orig.point_offset == las_header.point_offset);
        REQUIRE(las_header_orig.vlr_count == las_header.vlr_count);
        REQUIRE(las_header_orig.point_format_id == las_header.point_format_id);
        REQUIRE(las_header_orig.point_record_length == las_header.point_record_length);
        REQUIRE(las_header_orig.point_count == las_header.point_count);
        REQUIRE(las_header_orig.points_by_return == las_header.points_by_return);
        REQUIRE(las_header_orig.scale.x == las_header.scale.x);
        REQUIRE(las_header_orig.scale.y == las_header.scale.y);
        REQUIRE(las_header_orig.scale.z == las_header.scale.z);
        REQUIRE(las_header_orig.offset.x == las_header.offset.x);
        REQUIRE(las_header_orig.offset.y == las_header.offset.y);
        REQUIRE(las_header_orig.offset.z == las_header.offset.z);
        REQUIRE(las_header_orig.max == las_header.max);
        REQUIRE(las_header_orig.min == las_header.min);
        REQUIRE(las_header_orig.wave_offset == las_header.wave_offset);
        REQUIRE(las_header_orig.evlr_offset == las_header.evlr_offset);
        REQUIRE(las_header_orig.evlr_count == las_header.evlr_count);
        REQUIRE(las_header_orig.points_by_return_14 == las_header.points_by_return_14);
    }
}

TEST_CASE("GetBounds", "[LasHeader]")
{
    GIVEN("A valid file_path")
    {
        FileReader reader("autzen-classified.copc.laz");
        auto las_header = reader.GetLasHeader();
        auto box = las_header.GetBounds();
        REQUIRE(box.x_min == las_header.min.x);
        REQUIRE(box.y_min == las_header.min.y);
        REQUIRE(box.z_min == las_header.min.z);
        REQUIRE(box.x_max == las_header.max.x);
        REQUIRE(box.y_max == las_header.max.y);
        REQUIRE(box.z_max == las_header.max.z);
    }
}
