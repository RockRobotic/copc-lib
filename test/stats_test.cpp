#include <catch2/catch.hpp>
#include <copc-lib/copc/stats.hpp>
#include <copc-lib/las/vlr.hpp>

using namespace copc;

TEST_CASE("COPC Stats", "[CopcStats]")
{

    int8_t point_format_id{6};
    uint16_t num_extra_bytes{5};

    SECTION("Constructor")
    {
        // Empty constructor
        {
            CopcStats stats{point_format_id, num_extra_bytes};
            REQUIRE(stats.point_format_id == point_format_id);
            REQUIRE(stats.extra_bytes.size() == num_extra_bytes);
        }
        // Filled Constructor
        {
            auto extents =
                std::vector<las::CopcExtent>(CopcStats::NumberOfExtents(point_format_id, num_extra_bytes), {1, 1});
            CopcStats stats{extents, point_format_id, num_extra_bytes};
            REQUIRE(stats.point_format_id == point_format_id);
            REQUIRE(stats.extra_bytes.size() == num_extra_bytes);
            REQUIRE(stats.x.minimum == 1);
            REQUIRE(stats.x.maximum == 1);
        }
        // Point format checks
        REQUIRE_THROWS(CopcStats(5));
        REQUIRE_THROWS(CopcStats(9));
        REQUIRE_THROWS(CopcStats(std::vector<las::CopcExtent>(3), point_format_id, num_extra_bytes));
    }

    SECTION("ToCopcExtents")
    {

        CopcStats stats{point_format_id, num_extra_bytes};

        auto extents = stats.ToCopcExtents();
        REQUIRE(extents.size() == CopcStats::NumberOfExtents(point_format_id, num_extra_bytes));
    }

    SECTION("FromCopcExtents")
    {
        CopcStats stats{point_format_id, num_extra_bytes};

        auto extents =
            std::vector<las::CopcExtent>(CopcStats::NumberOfExtents(point_format_id, num_extra_bytes), {1, 1});

        stats.FromCopcExtents(extents);

        REQUIRE(stats.x.minimum == 1);
        REQUIRE(stats.x.maximum == 1);

        REQUIRE_THROWS(stats.FromCopcExtents(std::vector<las::CopcExtent>(3)));
    }
}
