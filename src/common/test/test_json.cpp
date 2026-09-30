#include <catch2/catch_all.hpp>

#include <simdjson.h>
#include <visit_struct/visit_struct.hpp>

#include <print>
#include <string>
#include <vector>

struct SomeClassA
{
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

namespace simdjson
{
	template <typename Json>
	auto tag_invoke(simdjson::serialize_tag, Json& obj, const SomeClassA& value)
	{
		using Type = std::remove_cvref_t<decltype(value)>;
		bool first = true;

		obj.start_object();
		visit_struct::visit_pointers<Type>([&]<typename FieldType>(const char* name, FieldType Type::* ptr)
		{
			if (!first)
			{
				obj.append_comma();
			}

			obj.append_key_value(name, value.*ptr);
			first = false;
		});
		obj.end_object();
	}

	template <typename Json>
	auto tag_invoke(simdjson::deserialize_tag, Json& obj, SomeClassA& value)
	{
		simdjson::ondemand::object object;
		auto err = obj.get_object().get(object);

		using Type = std::remove_cvref_t<decltype(value)>;
		visit_struct::visit_pointers<Type>([&]<typename FieldType>(const char* name, FieldType Type::* ptr)
		{
			if (err)
			{
				return;
			}

			err = object[name].get<FieldType>(value.*ptr);
		});

		if (err)
		{
			return err;
		}

		return simdjson::SUCCESS;
	}
}

TEST_CASE("JSON filter", "[utils]")
{
	const std::string_view text = R"(
	{
		"some_int": 1234,
		"array_int": [1, 2, 3],
		"array_double": [12.23, 35.12]
	}
	)";

	const SomeClassA class_a
	{
		.some_int = 1234,
		.array_int = { 1, 2, 3 },
		.array_double = { 12.23, 35.12 }
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
}
