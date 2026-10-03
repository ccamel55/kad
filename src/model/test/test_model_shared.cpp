#include <catch2/catch_all.hpp>

#include <kad/model/shared/common.hpp>
#include <kad/model/shared/common_fmt.hpp>
#include <kad/model/shared/common_json.hpp>

#include <kad/common/test/json.hpp>

using namespace kad::model;

TEST_CASE("common", "[model]")
{
	SECTION("Path")
	{
		const Path obj
		{
			.source = "hello",
			.build = "world"
		};

		const std::string_view json =
		R"({
			"source": "hello",
			"build": "world"
		})";

		kad::common::test::CheckTypeSerializeDeserialize(obj, json);
	}

	SECTION("Version")
	{
		const Version obj
		{
			.major = 10,
			.minor = 20
		};

		const std::string_view json =
		R"({
			"major": 10,
			"minor": 20
		})";

		kad::common::test::CheckTypeSerializeDeserialize(obj, json);
	}

	SECTION("ReplyReference")
	{
		const ReplyReference obj
		{
			.kind = "codemodel",
			.version = { .major = 1, .minor = 2 },
			.jsonFile = "your mother"
		};

		const std::string_view json =
		R"({
			"kind": "codemodel",
			"version": {
				"major": 1,
				"minor": 2
			},
			"jsonFile": "your mother"
		})";

		kad::common::test::CheckTypeSerializeDeserialize(obj, json);
	}

	SECTION("Error")
	{
		const Error obj
		{
			.error= "fat"
		};

		const std::string_view json =
		R"({
			"error": "fat"
		})";

		kad::common::test::CheckTypeSerializeDeserialize(obj, json);
	}
}
