#include <catch2/catch_all.hpp>

#include <kad/model/reply/codemodel.hpp>
#include <kad/model/reply/codemodel_fmt.hpp>
#include <kad/model/reply/codemodel_json.hpp>

#include <kad/model/reply/index.hpp>
#include <kad/model/reply/index_fmt.hpp>
#include <kad/model/reply/index_json.hpp>

#include <kad/model/reply/target.hpp>
#include <kad/model/reply/target_fmt.hpp>
#include <kad/model/reply/target_json.hpp>

#include <kad/common/test/json.hpp>

using namespace kad::model::reply;

TEST_CASE("codemodel", "[model]")
{
	const CodeModel obj =
	{
		.paths = { .source = "world", .build = "hello" },
		.version = { .major = 1, .minor = 2 },
		.configurations =
		{
			CodeModel::Configurations
			{
				.name = "fart",
				.targets =
				{
					CodeModel::Configurations::Target
					{
						.name = "target-1",
						.id = "id-1",
						.directoryIndex = 1,
						.projectIndex = 1,
						.jsonFile = "lol"
					}
				},
				.abstractTargets =
				{
					CodeModel::Configurations::Target
					{
						.name = "target-2",
						.id = "id-2",
						.directoryIndex = 2,
						.projectIndex = 2,
						.jsonFile = "lol"
					}
				}
			}
		}
	};

	const std::string_view json =
	R"({
		"paths": {
			"source": "world",
			"build": "hello"
		},
		"version": {
			"major": 1,
			"minor": 2
		},
		"configurations": [
			{
				"name": "fart",
				"targets": [
					{
						"name": "target-1",
						"id": "id-1",
						"directoryIndex": 1,
						"projectIndex": 1,
						"jsonFile": "lol"
					}
				],
				"abstractTargets": [
					{
						"name": "target-2",
						"id": "id-2",
						"directoryIndex": 2,
						"projectIndex": 2,
						"jsonFile": "lol"
					}
				]
			}
		]
	})";

	kad::common::test::CheckTypeSerializeDeserialize(obj, json);
}

TEST_CASE("index", "[model]")
{
	Index obj =
	{
		.cmake =
		{
			.version =
			{
				.major = 3,
				.minor = 20,
				.patch = 3,

				.suffix = "dev",
				.string = "12.34.4",
				.isDirty = true
			},
			.paths =
			{
				.cmake = "this",
				.ctest = "is not",
				.cpack = "real",
				.root = "lol"
			},
			.generator =
			{
				.multiConfig = false,
				.name = "Ninja",
			}
		},
		.reply =
		{
			.clients =
			{
				{
					"client-kad",
					{
						.queryJson =
						{
							Index::Reply::Client::QueryJson
							{
								.client = "hello",
								.responses = std::vector<Index::Reply::Client::QueryJson::ReplyReferenceOrError>
								{
									std::unexpected{ kad::model::Error{ .error = "world" } },
									kad::model::ReplyReference{ .kind = "codemodel", .version = { .major = 1, .minor = 2 }, .jsonFile = "fat" }
								}
							}
						}
					}
				}
			}
		}
	};

	const std::string_view json =
	R"({
		"cmake": {
			"version": {
				"major": 3,
				"minor": 20,
				"patch": 3,
				"suffix": "dev",
				"string": "12.34.4",
				"isDirty": true
			},
			"paths": {
				"cmake": "this",
				"ctest": "is not",
				"cpack": "real",
				"root": "lol"
			},
			"generator": {
				"multiConfig": false,
				"name": "Ninja"
			}
		},
		"reply": {
			"client-kad": {
				"query.json": {
					"client": "hello",
					"responses": [
						{
							"error": "world"
						},
						{
							"kind": "codemodel",
							"version": {
								"major": 1,
								"minor": 2
							},
							"jsonFile": "fat"
						}
					]
				}
			}
		}
	})";

	kad::common::test::CheckTypeSerializeDeserialize(obj, json);
}

TEST_CASE("target", "[model]")
{

}
