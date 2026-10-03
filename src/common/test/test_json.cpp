#include <catch2/catch_all.hpp>

#include <kad/common/json/serialize_json.hpp>

#include <simdjson.h>
#include <magic_enum/magic_enum.hpp>
#include <visit_struct/visit_struct.hpp>

#include <print>
#include <string>
#include <vector>

enum class MyEnum
{
	HELLO,
	WORLD,
	HOW,
	ARE,
	YOU
};

struct SomeClassA
{
	MyEnum some_enum;
	int some_int;
	std::vector<int> array_int;
	std::vector<double> array_double;
};

struct SomeClassB
{
	std::string my_string;
	SomeClassA other_class;
	std::vector<SomeClassA> other_class_vector;
};

VISITABLE_STRUCT(
	SomeClassA,
	some_enum,
	some_int,
	array_int,
	array_double
);

VISITABLE_STRUCT(
	SomeClassB,
	my_string,
	other_class,
	other_class_vector
);

JSON_SERIALIZE_ENUM(MyEnum);

JSON_SERIALIZE_STRUCT(SomeClassA);
JSON_SERIALIZE_STRUCT(SomeClassB);

TEST_CASE("JSON filter", "[utils]")
{
	const std::string_view text = R"(
	{
		"some_enum": "ARE",
		"some_int": 1234,
		"array_int": [1, 2, 3],
		"array_double": [12.23, 35.12]
	}
	)";

	const SomeClassA class_a
	{
		.some_enum = MyEnum::ARE,
		.some_int = 1234,
		.array_int = { 1, 2, 3 },
		.array_double = { 12.23, 35.12 }
	};

	const SomeClassB class_b
	{
		.my_string = "poo",
		.other_class = class_a,
		.other_class_vector = { class_a, class_a }
	};

	{
		simdjson::padded_input json(text);

		simdjson::ondemand::parser parser;
		simdjson::ondemand::document parsed = parser.iterate(json);

		const auto value = parsed.get<SomeClassA>();
		CHECK_FALSE(value.error());

		CHECK(value->some_int == class_a.some_int);
		CHECK(value->array_int == class_a.array_int);
		CHECK(value->array_double == class_a.array_double);
	}

	{
		std::string out;

		simdjson::fractured_json_options options;
		CHECK_FALSE(simdjson::to_json(class_a, out));

		std::println("{}", out);
	}

	{
		std::string out;

		simdjson::fractured_json_options options;
		CHECK_FALSE(simdjson::to_json(class_b, out));

		std::println("{}", out);
	}
}
