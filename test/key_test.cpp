#include <catch2/catch.hpp>
#include <copc-lib/hierarchy/key.hpp>

using namespace copc::hierarchy;

TEST_CASE("Voxel Key checks key validity", "[Key]")
{
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

    REQUIRE(!(VoxelKey(3, 2, 3, 6).GetParent() == VoxelKey(0, 0, 0, 0)));

    REQUIRE(VoxelKey(1, 1, 1, 1).GetParent() == VoxelKey(0, 0, 0, 0));
    REQUIRE(VoxelKey(1, 1, 1, -1).GetParent() == VoxelKey(-1, -1, -1, -1));

    REQUIRE(VoxelKey(1, 1, 1, -1).GetParent().IsValid() == false);
    REQUIRE(VoxelKey(0, 0, 0, 0).GetParent().IsValid() == false);
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