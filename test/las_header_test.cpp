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
        auto las_header = reader.CopcConfig().LasHeader();
        auto lazperf_header =
            las_header.ToLazPerf(las_header.PointOffset(), las_header.PointCount(), las_header.EvlrOffset(),
                                 las_header.EvlrCount(), las_header.EbByteSize());
        // Correct the bitshift happening in ToLazPerf for test purpose
        lazperf_header.point_format_id = las_header.PointFormatID();
        auto las_header_origin = las::LasHeader::FromLazPerf(lazperf_header);

        REQUIRE(las_header_origin.file_source_id == las_header.file_source_id);
        REQUIRE(las_header_origin.global_encoding == las_header.global_encoding);
        REQUIRE(las_header_origin.GUID() == las_header.GUID());
        REQUIRE(las_header_origin.SystemIdentifier() == las_header.SystemIdentifier());
        REQUIRE(las_header_origin.GeneratingSoftware() == las_header.GeneratingSoftware());
        REQUIRE(las_header_origin.creation_day == las_header.creation_day);
        REQUIRE(las_header_origin.creation_year == las_header.creation_year);
        REQUIRE(las_header_origin.PointFormatID() == las_header.PointFormatID());
        REQUIRE(las_header_origin.PointRecordLength() == las_header.PointRecordLength());
        REQUIRE(las_header_origin.PointOffset() == las_header.PointOffset());
        REQUIRE(las_header_origin.PointCount() == las_header.PointCount());
        REQUIRE(las_header_origin.points_by_return == las_header.points_by_return);
        REQUIRE(las_header_origin.scale.x == las_header.scale.x);
        REQUIRE(las_header_origin.scale.y == las_header.scale.y);
        REQUIRE(las_header_origin.scale.z == las_header.scale.z);
        REQUIRE(las_header_origin.offset.x == las_header.offset.x);
        REQUIRE(las_header_origin.offset.y == las_header.offset.y);
        REQUIRE(las_header_origin.offset.z == las_header.offset.z);
        REQUIRE(las_header_origin.max == las_header.max);
        REQUIRE(las_header_origin.min == las_header.min);
        REQUIRE(las_header_origin.VlrCount() == las_header.VlrCount());
        REQUIRE(las_header_origin.EvlrOffset() == las_header.EvlrOffset());
        REQUIRE(las_header_origin.EvlrCount() == las_header.EvlrCount());

        las_header_origin.ToString();
    }
}

TEST_CASE("GetBounds", "[LasHeader]")
{
    GIVEN("A valid file_path")
    {
        FileReader reader("autzen-classified.copc.laz");
        auto las_header = reader.CopcConfig().LasHeader();
        auto box = las_header.GetBounds();
        REQUIRE(box.x_min == las_header.min.x);
        REQUIRE(box.y_min == las_header.min.y);
        REQUIRE(box.z_min == las_header.min.z);
        REQUIRE(box.x_max == las_header.max.x);
        REQUIRE(box.y_max == las_header.max.y);
        REQUIRE(box.z_max == las_header.max.z);
    }
}
