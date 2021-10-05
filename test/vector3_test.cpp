#include <catch2/catch.hpp>
#include <copc-lib/geometry/vector3.hpp>

using namespace copc;

TEST_CASE("Vector3", "[Vector3]")
{
    SECTION("Constructors")
    {
        auto vec = Vector3();
        REQUIRE(vec.x == 0.0);
        REQUIRE(vec.y == 0.0);
        REQUIRE(vec.z == 0.0);

        vec = Vector3(1.0, 2.0, 3.0);
        REQUIRE(vec.x == 1.0);
        REQUIRE(vec.y == 2.0);
        REQUIRE(vec.z == 3.0);

        vec = Vector3(std::vector<double>{1.0, 2.0, 3.0});
        REQUIRE(vec.x == 1.0);
        REQUIRE(vec.y == 2.0);
        REQUIRE(vec.z == 3.0);

        vec = Vector3::DefaultScale();
        REQUIRE(vec.x == DEFAULT_SCALE);
        REQUIRE(vec.y == DEFAULT_SCALE);
        REQUIRE(vec.z == DEFAULT_SCALE);

        vec = Vector3::DefaultOffset();
        REQUIRE(vec.x == 0.0);
        REQUIRE(vec.y == 0.0);
        REQUIRE(vec.z == 0.0);
    }
    SECTION("Vector3 Operators")
    {
        auto vec = Vector3(1.0, 2.0, 3.0);
        auto other = Vector3(1.0, 2.0, 3.0);

        auto res = vec * other;

        REQUIRE(res.x == 1.0);
        REQUIRE(res.y == 4.0);
        REQUIRE(res.z == 9.0);

        res = vec / other;

        REQUIRE(res.x == 1.0);
        REQUIRE(res.y == 1.0);
        REQUIRE(res.z == 1.0);

        res = vec + other;

        REQUIRE(res.x == 2.0);
        REQUIRE(res.y == 4.0);
        REQUIRE(res.z == 6.0);

        res = vec - other;

        REQUIRE(res.x == 0.0);
        REQUIRE(res.y == 0.0);
        REQUIRE(res.z == 0.0);
    }

    SECTION("Double Operators")
    {
        auto vec = Vector3(1.0, 2.0, 3.0);
        auto vec2 = vec;
        REQUIRE(vec2.x == 1.0);
        REQUIRE(vec2.y == 2.0);
        REQUIRE(vec2.z == 3.0);
        REQUIRE(vec2 == vec);

        vec2 = vec * 2;

        REQUIRE(vec2.x == 2.0);
        REQUIRE(vec2.y == 4.0);
        REQUIRE(vec2.z == 6.0);
        REQUIRE(vec2 != vec);

        vec2 = vec / 2;

        REQUIRE(vec2.x == 0.5);
        REQUIRE(vec2.y == 1.0);
        REQUIRE(vec2.z == 1.5);
        REQUIRE(vec2 != vec);

        vec2 = vec + 2;

        REQUIRE(vec2.x == 3.0);
        REQUIRE(vec2.y == 4.0);
        REQUIRE(vec2.z == 5.0);
        REQUIRE(vec2 != vec);

        vec2 = vec - 2;

        REQUIRE(vec2.x == -1.0);
        REQUIRE(vec2.y == 0.0);
        REQUIRE(vec2.z == 1.0);
        REQUIRE(vec2 != vec);
    }
}
