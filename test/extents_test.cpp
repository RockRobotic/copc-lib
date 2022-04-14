#include <limits>
#include <string>

#include <catch2/catch.hpp>
#include <copc-lib/copc/extents.hpp>
#include <copc-lib/io/copc_reader.hpp>
#include <copc-lib/io/copc_writer.hpp>
#include <copc-lib/las/vlr.hpp>

using namespace copc;
using namespace std;

TEST_CASE("COPC Extent", "[CopcExtent]")
{
    CopcExtent extent(-numeric_limits<double>::max(), numeric_limits<double>::max(), 5, 10);

    REQUIRE(extent.minimum == -numeric_limits<double>::max());
    REQUIRE(extent.maximum == numeric_limits<double>::max());
    REQUIRE(extent.mean == 5);
    REQUIRE(extent.var == 10);

    CopcExtent other(-numeric_limits<double>::max(), numeric_limits<double>::max(), 5, 10);
    REQUIRE(extent == other);

    other = CopcExtent(-numeric_limits<double>::max(), 5);

    REQUIRE(extent != other);

    REQUIRE_THROWS(CopcExtent(1, 0));
}

TEST_CASE("COPC Extents", "[CopcExtents]")
{

    int8_t point_format_id{7};
    uint16_t num_eb_items{5};

    SECTION("Constructor")
    {
        // Empty constructor
        {
            CopcExtents extents{point_format_id, num_eb_items};
            REQUIRE(extents.PointFormatId() == point_format_id);
            REQUIRE(extents.ExtraBytes().size() == num_eb_items);
        }
        // Vlr Constructor
        {
            auto vlr = las::CopcExtentsVlr();
            vlr.items.resize(CopcExtents::NumberOfExtents(point_format_id, num_eb_items) + 3, {0, 0});
            CopcExtents extents{vlr, point_format_id, num_eb_items};
            REQUIRE(extents.PointFormatId() == point_format_id);
            REQUIRE(extents.ExtraBytes().size() == num_eb_items);
            REQUIRE(extents.Intensity()->minimum == 0);
            REQUIRE(extents.Intensity()->maximum == 0);
            REQUIRE(*extents.Intensity() == CopcExtent(0, 0)); // Test == operator
            REQUIRE(*extents.Intensity() != CopcExtent(0, 1)); // Test != operator
        }
        // Point format checks
        REQUIRE_THROWS(CopcExtents(5));
        REQUIRE_THROWS(CopcExtents(9));
    }

    SECTION("Default Extents")
    {
        string file_path = "writer_test.copc.laz";
        {
            las::EbVlr eb_vlr(2);
            eb_vlr.items[0].data_type = 29; // byte size 12
            eb_vlr.items[1].data_type = 29; // byte size 12

            CopcConfigWriter cfg(7, {}, {}, {}, eb_vlr);
            FileWriter writer(file_path, cfg);

            auto extents = writer.CopcConfig()->CopcExtents();
            REQUIRE(extents->PointFormatId() == 7);
            REQUIRE(extents->HasExtendedStats() == false);
            REQUIRE(extents->ExtraBytes().size() == 2);

            writer.Close();
        }
        {
            FileReader reader(file_path);

            auto extents = reader.CopcConfig().CopcExtents();

            REQUIRE(extents.PointFormatId() == 7);
            REQUIRE(extents.HasExtendedStats() == false);
            REQUIRE(extents.ExtraBytes().size() == 2);

            REQUIRE(extents.Intensity()->minimum == 0);
            REQUIRE(extents.Classification()->minimum == 0);

            for (const auto &extent : extents.Extents())
            {
                REQUIRE(extent->minimum == 0);
                REQUIRE(extent->maximum == 0);
            }
        }
    }

    SECTION("Set Extents")
    {
        string file_path = "writer_test.copc.laz";
        {
            las::EbVlr eb_vlr(1);
            eb_vlr.items[0].data_type = 29; // byte size 12

            CopcConfigWriter cfg(7, {}, {}, {}, eb_vlr, true);
            FileWriter writer(file_path, cfg);

            auto extents = writer.CopcConfig()->CopcExtents();

            REQUIRE(extents->HasExtendedStats() == true);

            extents->Intensity()->minimum = -numeric_limits<double>::max();
            extents->Intensity()->maximum = numeric_limits<double>::max();
            extents->Intensity()->mean = 15;
            extents->Intensity()->var = 5;

            extents->Classification()->minimum = 0;
            extents->Classification()->maximum = 255;

            extents->ExtraBytes()[0]->minimum = -numeric_limits<double>::max();
            extents->ExtraBytes()[0]->maximum = numeric_limits<double>::max();
            extents->ExtraBytes()[0]->mean = 566;
            extents->ExtraBytes()[0]->var = 158;

            // Vector accessor
            extents->Extents()[7]->minimum = 78;
            extents->Extents()[7]->maximum = 79;

            extents->Extents()[8]->minimum = 80;
            extents->Extents()[8]->maximum = 81;

            writer.Close();
        }
        {
            FileReader reader(file_path);

            auto extents = reader.CopcConfig().CopcExtents();

            REQUIRE(extents.Intensity()->minimum == -numeric_limits<double>::max());
            REQUIRE(extents.Intensity()->maximum == numeric_limits<double>::max());
            REQUIRE(extents.Intensity()->mean == 15);
            REQUIRE(extents.Intensity()->var == 5);

            REQUIRE(*extents.Classification() == CopcExtent(0, 255));
            REQUIRE(*extents.Classification() == CopcExtent(0, 255, 0, 1));

            REQUIRE(extents.ExtraBytes()[0]->minimum == -numeric_limits<double>::max());
            REQUIRE(extents.ExtraBytes()[0]->maximum == numeric_limits<double>::max());
            REQUIRE(extents.ExtraBytes()[0]->mean == 566);
            REQUIRE(extents.ExtraBytes()[0]->var == 158);

            // Vector accessor
            REQUIRE(*extents.UserData() == CopcExtent(78, 79));

            REQUIRE(extents.Extents()[8]->minimum == 80);
            REQUIRE(extents.Extents()[8]->maximum == 81);
        }
    }

    SECTION("ToCopcExtentVlr")
    {

        CopcExtents extents{point_format_id, num_eb_items};

        auto vlr = extents.ToLazPerf({}, {}, {});
        REQUIRE(vlr.items.size() == CopcExtents::NumberOfExtents(point_format_id, num_eb_items) + 3);
    }

    SECTION("Get/Set Extents")
    {
        CopcExtents extents{point_format_id, num_eb_items};

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
    // TODO[Leo]: Add all Extents functions to tests
}
