#include <catch2/catch_all.hpp>
#include <kad/common/scope_exit.hpp>

using namespace kad::common;

TEST_CASE("Scope exit", "[utils]")
{
	bool did_exit = false;
	REQUIRE_FALSE(did_exit);

	{
		ScopeExit exit{[&]{ did_exit = true; }};
		REQUIRE_FALSE(did_exit);
	}

	REQUIRE(did_exit);
}
