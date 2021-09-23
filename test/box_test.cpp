#include <limits>

#include <catch2/catch.hpp>
#include <copc-lib/hierarchy/box.hpp>
#include <copc-lib/hierarchy/key.hpp>
#include <copc-lib/hierarchy/vector3.hpp>

using namespace copc;

TEST_CASE("Box constructor", "[Box]")
{
    SECTION("2D box constructor")
    {
        auto box = Box(1.0, 0.0, 1.0, 1.0);
        REQUIRE(box.x_min == 1.0);
        REQUIRE(box.y_min == 0.0);
        REQUIRE(box.x_max == 1.0);
        REQUIRE(box.y_max == 1.0);
        REQUIRE(box.z_min == -std::numeric_limits<double>::max());
        REQUIRE(box.z_max == std::numeric_limits<double>::max());

        REQUIRE_THROWS(Box(2, 0, 1, 1));
        REQUIRE_THROWS(Box(0, 2, 1, 1));
    }

    SECTION("3D box constructor")
    {
        auto box = Box(1.0, 0.0, 0.0, 1.0, 1.0, 1.0);
        REQUIRE(box.x_min == 1.0);
        REQUIRE(box.y_min == 0.0);
        REQUIRE(box.z_min == 0.0);
        REQUIRE(box.x_max == 1.0);
        REQUIRE(box.y_max == 1.0);
        REQUIRE(box.z_max == 1.0);

        REQUIRE_THROWS(Box(2, 0, 0, 1, 1, 1));
        REQUIRE_THROWS(Box(0, 2, 0, 1, 1, 1));
        REQUIRE_THROWS(Box(0, 0, 2, 1, 1, 1));
    }

    SECTION("VoxelKey constructor")
    {
        // Make a LasHeader with span 10
        auto header = las::LasHeader();
        header.min = Vector3(0, 0, 0);
        header.max = Vector3(10, 10, 10);
        auto box = Box(VoxelKey(1, 1, 0, 0), header);

        REQUIRE(box.x_min == 5.0);
        REQUIRE(box.y_min == 0.0);
        REQUIRE(box.z_min == 0.0);
        REQUIRE(box.x_max == 10.0);
        REQUIRE(box.y_max == 5.0);
        REQUIRE(box.z_max == 5.0);
    }
}

TEST_CASE("Box functions", "[Box]")
{
    SECTION("Intersects 2D box")
    {
        // Contains
        REQUIRE(Box(0, 0, 4, 4).Intersects(Box(1, 1, 2, 2)));
        // Crosses
        REQUIRE(Box(1, 1, 2, 2).Intersects(Box(1.5, 1.5, 2.5, 2.5)));
        REQUIRE(Box(1, 1, 2, 2).Intersects(Box(1.5, 0.5, 2.5, 2.5)));
        REQUIRE(Box(1, 1, 2, 2).Intersects(Box(0.5, 1.5, 2.5, 2.5)));
        // Equals
        REQUIRE(Box(0, 0, 1, 1).Intersects(Box(0, 0, 1, 1)));
        // Touches
        REQUIRE(Box(0, 0, 1, 1).Intersects(Box(1, 1, 2, 2)));
        // Within
        REQUIRE(Box(1, 1, 2, 2).Intersects(Box(0, 0, 4, 4)));
        // Outside
        REQUIRE(!Box(0, 0, 1, 1).Intersects(Box(1, 1.1, 2, 2)));
    }
    SECTION("Intersects 3D box")
    {
        // Contains
        REQUIRE(Box(0, 0, 0, 4, 4, 4).Intersects(Box(1, 1, 1, 2, 2, 2)));
        // Crosses
        REQUIRE(Box(1, 1, 1, 2, 2, 2).Intersects(Box(1.5, 1.5, 1.5, 2.5, 2.5, 2.5)));
        REQUIRE(Box(1, 1, 1, 2, 2, 2).Intersects(Box(1.5, 1.5, 0.5, 2.5, 2.5, 2.5)));
        REQUIRE(Box(1, 1, 1, 2, 2, 2).Intersects(Box(1.5, 0.5, 1.5, 2.5, 2.5, 2.5)));
        REQUIRE(Box(1, 1, 1, 2, 2, 2).Intersects(Box(0.5, 1.5, 1.5, 2.5, 2.5, 2.5)));
        // Equals
        REQUIRE(Box(0, 0, 0, 1, 1, 1).Intersects(Box(0, 0, 0, 1, 1, 1)));
        // Touches
        REQUIRE(Box(0, 0, 0, 1, 1, 1).Intersects(Box(1, 1, 1, 2, 2, 2)));
        // Within
        REQUIRE(Box(1, 1, 1, 2, 2, 2).Intersects(Box(0, 0, 0, 4, 4, 4)));
        // Outside
        REQUIRE(!Box(0, 0, 0, 1, 1, 1).Intersects(Box(1, 1, 1.1, 2, 2, 2)));
    }

    SECTION("Contains box")
    {
        // Make a LasHeader with span 10
        auto header = las::LasHeader();
        header.min = Vector3(0, 0, 0);
        header.max = Vector3(10, 10, 10);
        auto box1 = Box(VoxelKey(0, 0, 0, 0), header);
        auto box2 = Box(VoxelKey(1, 1, 0, 0), header);
        REQUIRE(box1.Contains(box2));
        REQUIRE(!box2.Contains(box1));
        // A box contains itself
        REQUIRE(box2.Contains(box2));
    }

    SECTION("Contains vector")
    {
        // 2D box
        REQUIRE(Box(0.0, 0.0, 1.0, 1.0).Contains(Vector3(0.5, 0.5, 5.0)));
        REQUIRE(Box(0.0, 0.0, 1.0, 1.0).Contains(Vector3(0.5, 1.0, 5.0)));
        REQUIRE(!Box(0.0, 0.0, 1.0, 1.0).Contains(Vector3(0.5, 5.0, 5.0)));

        // 3D box
        REQUIRE(Box(0.0, 0.0, 0.0, 1.0, 1.0, 1.0).Contains(Vector3(0.5, 0.5, 0.5)));
        REQUIRE(Box(0.0, 0.0, 0.0, 1.0, 1.0, 1.0).Contains(Vector3(1.0, 1.0, 1.0)));
        REQUIRE(!Box(0.0, 0.0, 0.0, 1.0, 1.0, 1.0).Contains(Vector3(0.5, 0.5, 5.0)));
    }

    SECTION("Within")
    {
        // Make a LasHeader with span 10
        auto header = las::LasHeader();
        header.min = Vector3(0, 0, 0);
        header.max = Vector3(10, 10, 10);
        auto box1 = Box(VoxelKey(0, 0, 0, 0), header);
        auto box2 = Box(VoxelKey(1, 1, 0, 0), header);
        REQUIRE(!box1.Within(box2));
        REQUIRE(box2.Within(box1));
        // A box is within itself
        REQUIRE(box2.Within(box2));
    }
}

TEST_CASE("VoxelKey Spatial functions", "[VoxelKey]")
{
    // Make a LasHeader with span 2
    auto header = las::LasHeader();
    header.min = Vector3(0, 0, 0);
    header.max = Vector3(2, 2, 2);

    SECTION("Intersects")
    {
        // Contains
        REQUIRE(VoxelKey(1, 1, 1, 1).Intersects(Box(1.1, 1.1, 1.1, 1.9, 1.9, 1.9), header));
        // Crosses
        REQUIRE(VoxelKey(1, 1, 1, 1).Intersects(Box(1.5, 1.5, 1.5, 2.5, 2.5, 2.5), header));
        REQUIRE(VoxelKey(1, 1, 1, 1).Intersects(Box(1.5, 1.5, 0.5, 2.5, 2.5, 2.5), header));
        REQUIRE(VoxelKey(1, 1, 1, 1).Intersects(Box(1.5, 0.5, 1.5, 2.5, 2.5, 2.5), header));
        REQUIRE(VoxelKey(1, 1, 1, 1).Intersects(Box(0.5, 1.5, 1.5, 2.5, 2.5, 2.5), header));
        // Equals
        REQUIRE(VoxelKey(1, 0, 0, 0).Intersects(Box(0, 0, 0, 1, 1, 1), header));
        // Touches
        REQUIRE(VoxelKey(1, 0, 0, 0).Intersects(Box(1, 1, 1, 2, 2, 2), header));
        // Within
        REQUIRE(VoxelKey(1, 1, 1, 1).Intersects(Box(0, 0, 0, 4, 4, 4), header));
        // Outside
        REQUIRE(!VoxelKey(1, 0, 0, 0).Intersects(Box(1, 1, 1.1, 2, 2, 2), header));
    }

    SECTION("Contains box")
    {
        REQUIRE(VoxelKey(0, 0, 0, 0).Contains(Box(0, 0, 0, 1, 1, 1), header));
        REQUIRE(!VoxelKey(2, 0, 0, 0).Contains(Box(0, 0, 0, 1, 1, 1), header));
        // A box contains itself
        REQUIRE(
            VoxelKey(0, 0, 0, 0).Contains(Box(0, 0, 0, header.GetSpan(), header.GetSpan(), header.GetSpan()), header));
    }

    SECTION("Contains vector")
    {
        REQUIRE(VoxelKey(0, 0, 0, 0).Contains(Vector3(1, 1, 1), header));
        REQUIRE(!VoxelKey(0, 0, 0, 0).Contains(Vector3(2.1, 1, 1), header));
    }

    SECTION("Within")
    {
        REQUIRE(VoxelKey(1, 1, 1, 1).Within(Box(0.99, 0.99, 0.99, 2.01, 2.01, 2.01), header));
        // A box is within itself
        REQUIRE(
            VoxelKey(0, 0, 0, 0).Within(Box(0, 0, 0, header.GetSpan(), header.GetSpan(), header.GetSpan()), header));
    }
}
