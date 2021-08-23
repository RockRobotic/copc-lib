#include "copc_lib.hpp"
#include <catch2/catch.hpp>
#include <cstring>

using namespace copc;

TEST_CASE("Initializer Test", "[API]")
{
    REQUIRE_NOTHROW(Copc("test/data/autzen-classified.copc.laz"));

    REQUIRE_THROWS(Copc("bad_file.laz"));
}
