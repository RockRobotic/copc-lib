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
    uint16_t extra_byte_count{5};

    SECTION("Constructor")
    {
        // Empty constructor
        {
            CopcExtents extents{point_format_id, extra_byte_count};
            REQUIRE(extents.PointFormatID() == point_format_id);
            REQUIRE(extents.ExtraBytes().size() == extra_byte_count);
        }
        // Vlr Constructor
        {
            auto vlr = las::CopcExtentsVlr();
            vlr.items.resize(CopcExtents::NumberOfExtents(point_format_id, extra_byte_count), {0, 0});
            CopcExtents extents{vlr, point_format_id, extra_byte_count};
            REQUIRE(extents.PointFormatID() == point_format_id);
            REQUIRE(extents.ExtraBytes().size() == extra_byte_count);
            REQUIRE(extents.X()->minimum == 0);
            REQUIRE(extents.X()->maximum == 0);
        }
        // Point format checks
        REQUIRE_THROWS(CopcExtents(5));
        REQUIRE_THROWS(CopcExtents(9));
    }

    SECTION("Default Extents")
    {
        string file_path = "writer_test.copc.laz";

        Writer::LasHeaderConfig cfg(7);
        las::EbVlr eb_vlr(2);
        eb_vlr.items[0].data_type = 29; // byte size 12
        eb_vlr.items[1].data_type = 29; // byte size 12
        cfg.extra_bytes = eb_vlr;
        FileWriter writer(file_path, cfg, 16);

        auto extents = writer.GetCopcExtents();
        REQUIRE(extents.PointFormatID() == 7);
        REQUIRE(extents.ExtraBytes().size() == 2);

        writer.Close();

        FileReader reader(file_path);

        extents = reader.GetCopcExtents();

        REQUIRE(extents.PointFormatID() == 7);
        REQUIRE(extents.ExtraBytes().size() == 2);

        REQUIRE(extents.X()->minimum == 0);
        REQUIRE(extents.Y()->minimum == 0);

        for (const auto &extent : extents.Extents())
        {
            REQUIRE(extent->minimum == 0);
            REQUIRE(extent->maximum == 0);
        }
    }

    SECTION("Set Extents")
    {
        string file_path = "writer_test.copc.laz";

        Writer::LasHeaderConfig cfg(7);
        las::EbVlr eb_vlr(1);
        eb_vlr.items[0].data_type = 29; // byte size 12
        cfg.extra_bytes = eb_vlr;
        FileWriter writer(file_path, cfg, 256);

        auto extents = writer.GetCopcExtents();

        extents.X()->minimum = -numeric_limits<double>::max();
        extents.X()->maximum = numeric_limits<double>::max();

        extents.ExtraBytes()[0]->minimum = -numeric_limits<double>::max();
        extents.ExtraBytes()[0]->maximum = numeric_limits<double>::max();

        writer.SetCopcExtents(extents);

        writer.Close();

        FileReader reader(file_path);

        extents = reader.GetCopcExtents();

        REQUIRE(extents.X()->minimum == -numeric_limits<double>::max());
        REQUIRE(extents.X()->maximum == numeric_limits<double>::max());

        REQUIRE(extents.ExtraBytes()[0]->minimum == -numeric_limits<double>::max());
        REQUIRE(extents.ExtraBytes()[0]->maximum == numeric_limits<double>::max());
    }

    SECTION("ToCopcExtentVlr")
    {

        CopcExtents extents{point_format_id, extra_byte_count};

        auto vlr = extents.ToCopcExtentsVlr();
        REQUIRE(vlr.items.size() == CopcExtents::NumberOfExtents(point_format_id, extra_byte_count));
    }

    SECTION("Get/Set Extents")
    {
        CopcExtents extents{point_format_id, extra_byte_count};

        for (const auto &extent : extents.Extents())
        {
            extent->minimum = 1;
            extent->maximum = 1;
        }

        for (const auto &extent : extents.Extents())
        {
            REQUIRE(extent->minimum == 1);
            REQUIRE(extent->maximum == 1);
        }
    }
}
