#include <catch2/catch_all.hpp>
#include <kad/common/version.hpp>

using namespace kad::common;

TEST_CASE("Version", "[utils]")
{
    constexpr Version v1{ 0, 0, 1 };
    constexpr Version v2{ 1, 0, 0 };

    REQUIRE_FALSE(v1 == v2);

    REQUIRE(v1.major() == 0);
    REQUIRE(v1.minor() == 0);
    REQUIRE(v1.patch() == 1);

    REQUIRE(v2.major() == 1);
    REQUIRE(v2.minor() == 0);
    REQUIRE(v2.patch() == 0);
}
