#include <catch2/catch_all.hpp>

#include <kad/model/query/query.hpp>
#include <kad/model/query/query_fmt.hpp>
#include <kad/model/query/query_json.hpp>

#include <kad/common/test/json.hpp>

using namespace kad::model::query;

TEST_CASE("query", "[model]")
{
	SECTION("Query::Request")
	{
		const Query::Request obj
		{
			.kind = "codemodel",
			.version = { .major = 1, .minor = 2 },
			.client = "hello world"
		};

		const std::string_view json =
		R"({
			"kind": "codemodel",
			"version": {
				"major": 1,
				"minor": 2
			},
			"client": "hello world"
		})";

		kad::common::test::CheckTypeSerializeDeserialize(obj, json);
	}
}
