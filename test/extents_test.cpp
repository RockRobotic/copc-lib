#include <limits>
#include <string>

#include <catch2/catch.hpp>
#include <copc-lib/copc/extents.hpp>
#include <copc-lib/io/reader.hpp>
#include <copc-lib/io/writer.hpp>
#include <copc-lib/las/vlr.hpp>

using namespace copc;
using namespace std;

TEST_CASE("COPC Extents", "[CopcExtents]")
{

    int8_t point_format_id{7};
    uint16_t num_extra_bytes{5};

    SECTION("Constructor")
    {
        // Empty constructor
        {
            CopcExtents extents{point_format_id, num_extra_bytes};
            REQUIRE(extents.point_format_id == point_format_id);
            REQUIRE(extents.extra_bytes.size() == num_extra_bytes);
        }
        // Vlr Constructor
        {
            auto vlr = las::CopcExtentsVlr();
            vlr.items.resize(CopcExtents::NumberOfExtents(point_format_id, num_extra_bytes), {0, 0});
            CopcExtents extents{vlr, point_format_id, num_extra_bytes};
            REQUIRE(extents.point_format_id == point_format_id);
            REQUIRE(extents.extra_bytes.size() == num_extra_bytes);
            REQUIRE(extents.x.minimum == 0);
            REQUIRE(extents.x.maximum == 0);
        }
        // Point format checks
        REQUIRE_THROWS(CopcExtents(5));
        REQUIRE_THROWS(CopcExtents(9));
    }

    SECTION("Default Extents")
    {
        string file_path = "test/data/writer_test.copc.laz";

        Writer::LasHeaderConfig cfg(7);
        las::EbVlr eb_vlr(2);
        eb_vlr.items[0].data_type = 29; // byte size 12
        eb_vlr.items[1].data_type = 29; // byte size 12
        cfg.extra_bytes = eb_vlr;
        FileWriter writer(file_path, cfg, 256);

        auto extents = writer.GetCopcExtents();
        REQUIRE(extents.point_format_id == 7);
        REQUIRE(extents.extra_bytes.size() == 2);

        writer.Close();

        FileReader reader(file_path);

        extents = reader.GetCopcExtents();

        REQUIRE(extents.point_format_id == 7);
        REQUIRE(extents.extra_bytes.size() == 2);

        REQUIRE(extents.x.minimum == 0);
        REQUIRE(extents.y.minimum == 0);

        for (const auto &extent : extents.Extents())
        {
            REQUIRE(extent.minimum == 0);
            REQUIRE(extent.maximum == 0);
        }
    }

    SECTION("Set Extents")
    {
        string file_path = "test/data/writer_test.copc.laz";

        Writer::LasHeaderConfig cfg(7);
        las::EbVlr eb_vlr(1);
        eb_vlr.items[0].data_type = 29; // byte size 12
        cfg.extra_bytes = eb_vlr;
        FileWriter writer(file_path, cfg, 256);

        auto extents = writer.GetCopcExtents();

        extents.x.minimum = -numeric_limits<double>::max();
        extents.x.maximum = numeric_limits<double>::max();

        extents.extra_bytes[0].minimum = -numeric_limits<double>::max();
        extents.extra_bytes[0].maximum = numeric_limits<double>::max();

        writer.SetCopcExtents(extents);

        writer.Close();

        FileReader reader(file_path);

        extents = reader.GetCopcExtents();

        REQUIRE(extents.x.minimum == -numeric_limits<double>::max());
        REQUIRE(extents.x.maximum == numeric_limits<double>::max());

        REQUIRE(extents.extra_bytes[0].minimum == -numeric_limits<double>::max());
        REQUIRE(extents.extra_bytes[0].maximum == numeric_limits<double>::max());
    }

    SECTION("ToCopcExtentVlr")
    {

        CopcExtents extents{point_format_id, num_extra_bytes};

        auto vlr = extents.ToCopcExtentsVlr();
        REQUIRE(vlr.items.size() == CopcExtents::NumberOfExtents(point_format_id, num_extra_bytes));
    }

    SECTION("Set/Extents")
    {
        CopcExtents extents{point_format_id, num_extra_bytes};

        auto extent_vec =
            std::vector<CopcExtent>(CopcExtents::NumberOfExtents(point_format_id, num_extra_bytes), {1, 1});

        extents.Extents(extent_vec);

        for (const auto &extent : extents.Extents())
        {
            REQUIRE(extent.minimum == 1);
            REQUIRE(extent.maximum == 1);
        }

        // Test checks on size
        extent_vec.pop_back();
        REQUIRE_THROWS(extents.Extents(extent_vec));
        extent_vec.emplace_back(1, 1);
        extent_vec.emplace_back(1, 1);
        REQUIRE_THROWS(extents.Extents(extent_vec));
    }
}
