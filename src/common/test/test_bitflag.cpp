#include <catch2/catch_all.hpp>
#include <kad/common/bit_flag.hpp>

using namespace kad::common;

namespace
{
	enum class TestEnum
	{
		EMPTY   = 0,
		VALUE_1 = 1 << 0,
		VALUE_2 = 1 << 1,
	};
}

TEST_CASE("Bit flag", "[utils]")
{
	SECTION("1 - Constructor")
	{
		constexpr BitFlag<TestEnum> flag_1;
		constexpr BitFlag<TestEnum> flag_2{ 1 };
		constexpr BitFlag<TestEnum> flag_3{ TestEnum::VALUE_1 };

		REQUIRE_FALSE(flag_1.any());

		REQUIRE(flag_2.is_set(TestEnum::VALUE_1));
		REQUIRE_FALSE(flag_2.is_set(TestEnum::VALUE_2));

		REQUIRE(flag_3.is_set(TestEnum::VALUE_1));
		REQUIRE_FALSE(flag_3.is_set(TestEnum::VALUE_2));
	}

	SECTION("2 - Accessor")
	{
		BitFlag<TestEnum> flag;

		REQUIRE_FALSE(flag.any());

		flag.set(TestEnum::VALUE_1);

		REQUIRE(flag.any());
		REQUIRE(flag.is_set(TestEnum::VALUE_1));
		REQUIRE_FALSE(flag.is_set(TestEnum::VALUE_2));

		flag.set(TestEnum::VALUE_2);

		REQUIRE(flag.any());
		REQUIRE(flag.is_set(TestEnum::VALUE_1));
		REQUIRE(flag.is_set(TestEnum::VALUE_2));

		flag.unset(TestEnum::VALUE_1);

		REQUIRE(flag.any());
		REQUIRE_FALSE(flag.is_set(TestEnum::VALUE_1));
		REQUIRE(flag.is_set(TestEnum::VALUE_2));

		flag.clear();

		REQUIRE_FALSE(flag.any());
		REQUIRE_FALSE(flag.is_set(TestEnum::VALUE_1));
		REQUIRE_FALSE(flag.is_set(TestEnum::VALUE_2));
	}
}
