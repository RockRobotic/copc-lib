#include <cfloat>

#include <catch2/catch.hpp>
#include <copc-lib/hierarchy/key.hpp>
#include <copc-lib/las/header.hpp>

using namespace copc;

TEST_CASE("Voxel Key checks key validity", "[Key]")
{
    REQUIRE(VoxelKey().IsValid() == false);
    REQUIRE(VoxelKey(-1, -1, -1, -1).IsValid() == false);
    REQUIRE(VoxelKey(-1, 0, 0, 0).IsValid() == false);
    REQUIRE(VoxelKey(0, -1, 0, 0).IsValid() == false);
    REQUIRE(VoxelKey(0, 0, -1, 0).IsValid() == false);
    REQUIRE(VoxelKey(0, 0, 0, -1).IsValid() == false);
    REQUIRE(VoxelKey(0, 0, 0, 0).IsValid() == true);
    REQUIRE(VoxelKey(1, 1, 1, 1).IsValid() == true);
}

TEST_CASE("GetParent Checks", "[Key]")
{
    REQUIRE(VoxelKey(-1, -1, -1, -1).GetParent().IsValid() == false);

    REQUIRE(VoxelKey(4, 4, 6, 12).GetParent() == VoxelKey(3, 2, 3, 6));
    REQUIRE(VoxelKey(4, 5, 6, 13).GetParent() == VoxelKey(3, 2, 3, 6));
    REQUIRE(VoxelKey(3, 2, 3, 6).GetParent() == VoxelKey(2, 1, 1, 3));

    REQUIRE(!(VoxelKey(3, 2, 3, 6).GetParent() == VoxelKey::BaseKey()));

    REQUIRE(VoxelKey(1, 1, 1, 1).GetParent() == VoxelKey::BaseKey());
    REQUIRE(VoxelKey(1, 1, 1, -1).GetParent() == VoxelKey::InvalidKey());

    REQUIRE(VoxelKey(1, 1, 1, -1).GetParent().IsValid() == false);
    REQUIRE(VoxelKey(0, 0, 0, 0).GetParent().IsValid() == false);
}

TEST_CASE("IsChild Checks", "[Key]")
{
    REQUIRE(VoxelKey(-1, -1, -1, -1).ChildOf(VoxelKey::BaseKey()) == false);
    REQUIRE(VoxelKey::BaseKey().ChildOf(VoxelKey::InvalidKey()) == false);

    REQUIRE(VoxelKey(4, 4, 6, 12).ChildOf(VoxelKey(3, 2, 3, 6)));
    REQUIRE(VoxelKey(3, 2, 3, 6).ChildOf(VoxelKey(2, 1, 1, 3)));
    REQUIRE(VoxelKey(3, 2, 3, 6).ChildOf(VoxelKey::BaseKey()));

    REQUIRE(!VoxelKey(4, 4, 6, 12).ChildOf(VoxelKey(3, 4, 8, 6)));
    REQUIRE(!VoxelKey(3, 2, 3, 6).ChildOf(VoxelKey(2, 2, 2, 2)));
}

TEST_CASE("GetParents Checks", "[Key]")
{
    REQUIRE(VoxelKey(-1, -1, -1, -1).GetParents(true).empty() == true);
    REQUIRE(VoxelKey(-1, -1, -1, -1).GetParents(false).empty() == true);

    std::vector<VoxelKey> testKeys{
        VoxelKey(4, 4, 6, 12), VoxelKey(3, 2, 3, 6), VoxelKey(2, 1, 1, 3), VoxelKey(1, 0, 0, 1), VoxelKey(0, 0, 0, 0),
    };

    std::vector<VoxelKey> getParentsInclusive = testKeys[0].GetParents(true);
    REQUIRE(getParentsInclusive.size() == testKeys.size());
    REQUIRE_THAT(getParentsInclusive, Catch::Matchers::Equals(testKeys));

    std::vector<VoxelKey> testKeysExclusive = std::vector<VoxelKey>(testKeys.begin() + 1, testKeys.end());

    std::vector<VoxelKey> getParentsNonInclusive = testKeys[0].GetParents(false);
    REQUIRE(getParentsNonInclusive.size() == testKeys.size() - 1);
    REQUIRE_THAT(getParentsNonInclusive, Catch::Matchers::Equals(testKeysExclusive));
}

TEST_CASE("Box constructor", "[Box]")
{
    SECTION("3D box constructor")
    {
        auto box = Box(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
        REQUIRE(box.x_min == 0.0);
        REQUIRE(box.x_max == 1.0);
        REQUIRE(box.y_min == 0.0);
        REQUIRE(box.y_max == 1.0);
        REQUIRE(box.z_min == 0.0);
        REQUIRE(box.z_max == 1.0);
    }

    SECTION("2D box constructor")
    {
        auto box = Box(0.0, 1.0, 0.0, 1.0);
        REQUIRE(box.x_min == 0.0);
        REQUIRE(box.x_max == 1.0);
        REQUIRE(box.y_min == 0.0);
        REQUIRE(box.y_max == 1.0);
        REQUIRE(box.z_min == DBL_MIN);
        REQUIRE(box.z_max == DBL_MAX);
    }

    SECTION("VoxelKey constructor")
    {
        auto header = las::LasHeader();
        header.span = 10;
        auto box = Box(VoxelKey(1, 1, 0, 0), header);

        REQUIRE(box.x_min == 5.0);
        REQUIRE(box.x_max == 10.0);
        REQUIRE(box.y_min == 0.0);
        REQUIRE(box.y_max == 5.0);
        REQUIRE(box.z_min == 0.0);
        REQUIRE(box.z_max == 5.0);
    }
}
TEST_CASE("Box functions", "[Box]")
{
    SECTION("Intersects")
    {
        // X axis only crosses boundary
        REQUIRE(Box(1, 2, 0.5, 1).Intersects(Box(0.5, 1.5, 0, 2)));
        REQUIRE(Box(0.5, 1.5, 0, 2).Intersects(Box(1, 2, 0.5, 1)));

        REQUIRE(!Box(1, 2, 0.5, 1).Intersects(Box(2.01, 2.5, 0, 2)));
        REQUIRE(!Box(2.01, 2.5, 0, 2).Intersects(Box(1, 2, 0.5, 1)));

        // If one box contains the other they also intersect
        REQUIRE(Box(1, 2, 1, 2).Intersects(Box(0, 4, 0, 4)));
        REQUIRE(Box(0, 4, 0, 4).Intersects(Box(1, 2, 1, 2)));
    }

    SECTION("Contains box")
    {
        auto header = las::LasHeader();
        header.span = 10;
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
        REQUIRE(Box(0, 1, 0, 1).Contains(Vector3(0.5, 0.5, 5)));
        REQUIRE(Box(0, 1, 0, 1).Contains(Vector3(0.5, 1, 5)));
        REQUIRE(!Box(0, 1, 0, 1).Contains(Vector3(0.5, 5, 5)));

        // 3D box
        REQUIRE(Box(0, 1, 0, 1, 0, 1).Contains(Vector3(0.5, 0.5, 0.5)));
        REQUIRE(Box(0, 1, 0, 1, 0, 1).Contains(Vector3(1, 1, 1)));
        REQUIRE(!Box(0, 1, 0, 1, 0, 1).Contains(Vector3(0.5, 0.5, 5)));
    }

    SECTION("Within")
    {
        auto header = las::LasHeader();
        header.span = 10;
        auto box1 = Box(VoxelKey(0, 0, 0, 0), header);
        auto box2 = Box(VoxelKey(1, 1, 0, 0), header);
        REQUIRE(!box1.Within(box2));
        REQUIRE(box2.Within(box1));
        // A box is not within itself
        REQUIRE(box2.Within(box2));
    }
}
