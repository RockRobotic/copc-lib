#include "hierarchy.hpp"
#include "reader.hpp"
#include "structs.hpp"
#include <catch2/catch.hpp>

using namespace copc;

TEST_CASE("Voxel Key checks key validity", "[Hierarchy]")
{
    REQUIRE(VoxelKey(-1, -1, -1, -1).IsValid() == false);
    REQUIRE(VoxelKey(-1, 0, 0, 0).IsValid() == false);
    REQUIRE(VoxelKey(0, -1, 0, 0).IsValid() == false);
    REQUIRE(VoxelKey(0, 0, -1, 0).IsValid() == false);
    REQUIRE(VoxelKey(0, 0, 0, -1).IsValid() == false);
    REQUIRE(VoxelKey(0, 0, 0, 0).IsValid() == true);
    REQUIRE(VoxelKey(1, 1, 1, 1).IsValid() == true);
}

TEST_CASE("GetParent Checks", "[Hierarchy]")
{
    REQUIRE(Hierarchy::GetParent(VoxelKey(-1, -1, -1, -1)).IsValid() == false);

    REQUIRE(Hierarchy::GetParent(VoxelKey(4, 4, 6, 12)) == VoxelKey(3, 2, 3, 6));
    REQUIRE(Hierarchy::GetParent(VoxelKey(4, 5, 6, 13)) == VoxelKey(3, 2, 3, 6));
    REQUIRE(Hierarchy::GetParent(VoxelKey(3, 2, 3, 6)) == VoxelKey(2, 1, 1, 3));

    REQUIRE(!(Hierarchy::GetParent(VoxelKey(3, 2, 3, 6)) == VoxelKey(0, 0, 0, 0)));

    REQUIRE(Hierarchy::GetParent(VoxelKey(1, 1, 1, 1)) == VoxelKey(0, 0, 0, 0));
    REQUIRE(Hierarchy::GetParent(VoxelKey(1, 1, 1, -1)) == VoxelKey(-1, -1, -1, -1));

    REQUIRE(Hierarchy::GetParent(VoxelKey(1, 1, 1, -1)).IsValid() == false);
    REQUIRE(Hierarchy::GetParent(VoxelKey(0, 0, 0, 0)).IsValid() == false);
}

TEST_CASE("GetParents Checks", "[Hierarchy]")
{
    REQUIRE(Hierarchy::GetParents(VoxelKey(-1, -1, -1, -1), true).empty() == true);
    REQUIRE(Hierarchy::GetParents(VoxelKey(-1, -1, -1, -1), false).empty() == true);

    std::vector<VoxelKey> testKeys{
        VoxelKey(4, 4, 6, 12), VoxelKey(3, 2, 3, 6), VoxelKey(2, 1, 1, 3), VoxelKey(1, 0, 0, 1), VoxelKey(0, 0, 0, 0),
    };

    std::vector<VoxelKey> getParentsInclusive = Hierarchy::GetParents(testKeys[0], true);
    REQUIRE(getParentsInclusive.size() == testKeys.size());
    REQUIRE_THAT(getParentsInclusive, Catch::Matchers::Equals(testKeys));

    std::vector<VoxelKey> testKeysExclusive = std::vector<VoxelKey>(testKeys.begin() + 1, testKeys.end());

    std::vector<VoxelKey> getParentsNonInclusive = Hierarchy::GetParents(testKeys[0], false);
    REQUIRE(getParentsNonInclusive.size() == testKeys.size() - 1);
    REQUIRE_THAT(getParentsNonInclusive, Catch::Matchers::Equals(testKeysExclusive));
}

TEST_CASE("GetKey Checks", "[Hierarchy]")
{
    fstream in_stream;
    in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
    io::CopcReader reader(in_stream);

    auto copc = reader.GetCopcHeader();

    auto rootPage = reader.ReadPage(copc.root_hier_offset, copc.root_hier_size);

    Hierarchy hier = Hierarchy(&reader);

    auto key = VoxelKey(0, 0, 0, 0);
    auto hier_entry = hier.GetKey(key);

    REQUIRE(hier_entry.key == key);
    REQUIRE(hier_entry.offset == 90837784);
    REQUIRE(hier_entry.byteSize == 1082752);
    REQUIRE(hier_entry.pointCount == 61022);

    auto key2 = VoxelKey(5, 9, 7, 0);
    auto hier_entry2 = hier.GetKey(key2);

    REQUIRE(hier_entry2.key == key2);
    REQUIRE(hier_entry2.offset == 35154197);
    REQUIRE(hier_entry2.byteSize == 102416);
    REQUIRE(hier_entry2.pointCount == 12019);
}

TEST_CASE("LoadChildren Checks", "[Hierarchy]")
{
    fstream in_stream;
    in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
    io::CopcReader reader(in_stream);

    auto copc = reader.GetCopcHeader();

    auto rootPage = reader.ReadPage(copc.root_hier_offset, copc.root_hier_size);

    Hierarchy hier = Hierarchy(&reader);

    auto hier_entry = hier.GetKey(VoxelKey(0, 0, 0, 0));
    REQUIRE(hier.LoadChildren(hier_entry) == 4);
    hier_entry = hier.GetKey(VoxelKey(1, 0, 0, 0));
    REQUIRE(hier.LoadChildren(hier_entry) == 4);
    hier_entry = hier.GetKey(VoxelKey(4, 11, 9, 0));
    REQUIRE(hier.LoadChildren(hier_entry) == 0);
    hier_entry = hier.GetKey(VoxelKey(-1, -1, -1, -1));
    REQUIRE(hier.LoadChildren(hier_entry) == 0);
}

TEST_CASE("GetKey Validity Checks", "[Hierarchy]")
{
    fstream in_stream;
    in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
    io::CopcReader reader(in_stream);

    auto copc = reader.GetCopcHeader();

    auto rootPage = reader.ReadPage(copc.root_hier_offset, copc.root_hier_size);

    Hierarchy hier = Hierarchy(&reader);

    auto hier_entry = hier.GetKey(VoxelKey(0, 0, 0, 0));
    REQUIRE(hier_entry.IsValid() == true);
    hier_entry = hier.GetKey(VoxelKey(4, 11, 9, 0));
    REQUIRE(hier_entry.IsValid() == true);

    hier_entry = hier.GetKey(VoxelKey(20, 11, 9, 0));
    REQUIRE(hier_entry.IsValid() == false);
    hier_entry = hier.GetKey(VoxelKey(-1, 0, 0, 0));
    REQUIRE(hier_entry.IsValid() == false);
}

TEST_CASE("LoadToDepth", "[Hierarchy]")
{
    fstream in_stream;
    in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
    io::CopcReader reader(in_stream);

    auto copc = reader.GetCopcHeader();

    auto rootPage = reader.ReadPage(copc.root_hier_offset, copc.root_hier_size);

    Hierarchy hier = Hierarchy(&reader);

    hier.LoadPagesToDepth(hier.GetRootPage(), 3);

    hier = Hierarchy(&reader, 3);

    hier = Hierarchy(&reader, 0);

    hier = Hierarchy(&reader, -1);
}