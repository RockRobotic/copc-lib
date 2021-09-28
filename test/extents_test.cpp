#include <catch2/catch.hpp>
#include <copc-lib/copc/extents.hpp>
#include <copc-lib/las/vlr.hpp>

using namespace copc;

TEST_CASE("COPC Stats", "[CopcExtents]")
{

    int8_t point_format_id{6};
    uint16_t num_extra_bytes{5};

    SECTION("Constructor")
    {
        // Empty constructor
        {
            CopcExtents extents{point_format_id, num_extra_bytes};
            REQUIRE(extents.point_format_id == point_format_id);
            REQUIRE(extents.extra_bytes.size() == num_extra_bytes);
        }
        // Filled Constructor
        {
            auto vlr = las::CopcExtentsVlr();
            CopcExtents extents{vlr, point_format_id, num_extra_bytes};
            REQUIRE(extents.point_format_id == point_format_id);
            REQUIRE(extents.extra_bytes.size() == num_extra_bytes);
            REQUIRE(extents.x.minimum == 1);
            REQUIRE(extents.x.maximum == 1);
        }
        // Point format checks
        REQUIRE_THROWS(CopcExtents(5));
        REQUIRE_THROWS(CopcExtents(9));
    }

    SECTION("ToCopcExtentVlr")
    {

        CopcExtents extents{point_format_id, num_extra_bytes};

        auto vlr = extents.ToCopcExtentsVlr();
        REQUIRE(vlr.items.size() == CopcExtents::NumberOfExtents(point_format_id, num_extra_bytes));
    }

    SECTION("SetCopcExtents")
    {
        CopcExtents extents{point_format_id, num_extra_bytes};

        auto extent_vec =
            std::vector<las::CopcExtent>(CopcExtents::NumberOfExtents(point_format_id, num_extra_bytes), {1, 1});

        extents.SetCopcExtents(extent_vec);

        REQUIRE(extents.x.minimum == 1);
        REQUIRE(extents.x.maximum == 1);

        REQUIRE_THROWS(extents.SetCopcExtents(std::vector<las::CopcExtent>(3)));
    }
}
