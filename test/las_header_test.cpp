#include <vector>

#include <catch2/catch.hpp>
#include <copc-lib/io/copc_reader.hpp>
#include <copc-lib/las/header.hpp>
#include <copc-lib/las/point.hpp>

using namespace copc;

TEST_CASE("Test constructor", "[LasHeader]")
{

    int8_t point_format_id = 7;
    int num_eb_items = 1;
    double test_spacing = 12;
    double test_intensity_min = 5;
    double test_intensity_max = 155;
    Vector3 test_scale(1, 1, 1);
    Vector3 test_offset(50, 50, 50);
    std::string test_wkt = "test_wkt";

    las::EbVlr test_extra_bytes_vlr(num_eb_items);
    test_extra_bytes_vlr.items[0].data_type = 0;
    test_extra_bytes_vlr.items[0].options = 4;
    test_extra_bytes_vlr.items[0].name = "eb1";

    SECTION("copc flag")
    {
        // In the case of a LAZ file, the LAS header should have a false COPC flag, and a single VLR when no EBs are
        // present
        las::LasHeader laz_header(point_format_id,
                                  las::PointBaseByteSize(point_format_id) + test_extra_bytes_vlr.size(), test_scale,
                                  test_offset, false);

        REQUIRE(laz_header.Copc() == false);
        REQUIRE(laz_header.ToLazPerf(380, 0, 390, 1, 0, false).vlr_count == 1);

        // In the case of a COPC file, the LAS header should have a true COPC flag, and three VLRs when no EBs are
        // present
        las::LasHeader copc_header(point_format_id,
                                   las::PointBaseByteSize(point_format_id) + test_extra_bytes_vlr.size(), test_scale,
                                   test_offset, true);

        REQUIRE(copc_header.Copc() == true);
        REQUIRE(copc_header.ToLazPerf(380, 0, 390, 1, 0, false).vlr_count == 3);
    }
}

TEST_CASE("Test reader and conversions", "[LasHeader]")
{
    GIVEN("A valid file_path")
    {
        FileReader reader("autzen-classified.copc.laz");
        auto las_header = reader.CopcConfig().LasHeader();
        auto lazperf_header = las_header.ToLazPerf(
            las_header.PointOffset(), las_header.PointCount(), las_header.EvlrOffset(), las_header.EvlrCount(),
            las_header.EbByteSize(), reader.CopcConfig().CopcExtents().HasExtendedStats());
        // Correct the bitshift happening in ToLazPerf for test purpose
        lazperf_header.point_format_id = las_header.PointFormatId();
        auto las_header_origin = las::LasHeader::FromLazPerf(lazperf_header);

        REQUIRE(las_header_origin.file_source_id == las_header.file_source_id);
        REQUIRE(las_header_origin.global_encoding == las_header.global_encoding);
        REQUIRE(las_header_origin.GUID() == las_header.GUID());
        REQUIRE(las_header_origin.SystemIdentifier() == las_header.SystemIdentifier());
        REQUIRE(las_header_origin.GeneratingSoftware() == las_header.GeneratingSoftware());
        REQUIRE(las_header_origin.creation_day == las_header.creation_day);
        REQUIRE(las_header_origin.creation_year == las_header.creation_year);
        REQUIRE(las_header_origin.PointFormatId() == las_header.PointFormatId());
        REQUIRE(las_header_origin.PointRecordLength() == las_header.PointRecordLength());
        REQUIRE(las_header_origin.PointOffset() == las_header.PointOffset());
        REQUIRE(las_header_origin.PointCount() == las_header.PointCount());
        REQUIRE(las_header_origin.points_by_return == las_header.points_by_return);
        REQUIRE(las_header_origin.Scale().x == las_header.Scale().x);
        REQUIRE(las_header_origin.Scale().y == las_header.Scale().y);
        REQUIRE(las_header_origin.Scale().z == las_header.Scale().z);
        REQUIRE(las_header_origin.Offset().x == las_header.Offset().x);
        REQUIRE(las_header_origin.Offset().y == las_header.Offset().y);
        REQUIRE(las_header_origin.Offset().z == las_header.Offset().z);
        REQUIRE(las_header_origin.max == las_header.max);
        REQUIRE(las_header_origin.min == las_header.min);
        // REQUIRE(las_header_origin.VlrCount() == las_header.VlrCount()); //TODO Allow copy of VLRs
        REQUIRE(las_header_origin.EvlrOffset() == las_header.EvlrOffset());
        REQUIRE(las_header_origin.EvlrCount() == las_header.EvlrCount());

        las_header_origin.ToString();
    }
}

TEST_CASE("Bounds", "[LasHeader]")
{
    GIVEN("A valid file_path")
    {
        FileReader reader("autzen-classified.copc.laz");
        auto las_header = reader.CopcConfig().LasHeader();
        auto box = las_header.Bounds();
        REQUIRE(box.x_min == las_header.min.x);
        REQUIRE(box.y_min == las_header.min.y);
        REQUIRE(box.z_min == las_header.min.z);
        REQUIRE(box.x_max == las_header.max.x);
        REQUIRE(box.y_max == las_header.max.y);
        REQUIRE(box.z_max == las_header.max.z);
    }
}

TEST_CASE("Bounds Update", "[LasHeader]")
{
    auto las_header = las::LasHeader();

    REQUIRE(las_header.min.x == 0.0);
    REQUIRE(las_header.min.y == 0.0);
    REQUIRE(las_header.min.z == 0.0);
    REQUIRE(las_header.max.x == 0.0);
    REQUIRE(las_header.max.y == 0.0);
    REQUIRE(las_header.max.z == 0.0);

    las::Point point(las_header);
    point.X(10);
    point.Y(-5);
    point.Z(1);
    las_header.UpdateBounds(point);
    REQUIRE(las_header.min.x == 0.0);
    REQUIRE(las_header.min.y == -5.0);
    REQUIRE(las_header.min.z == 0.0);
    REQUIRE(las_header.max.x == 10.0);
    REQUIRE(las_header.max.y == 0.0);
    REQUIRE(las_header.max.z == 1.0);

    point.X(-10);
    point.Y(5);
    point.Z(-1);
    las_header.UpdateBounds(point);
    REQUIRE(las_header.min.x == -10.0);
    REQUIRE(las_header.min.y == -5.0);
    REQUIRE(las_header.min.z == -1.0);
    REQUIRE(las_header.max.x == 10.0);
    REQUIRE(las_header.max.y == 5.0);
    REQUIRE(las_header.max.z == 1.0);
}
