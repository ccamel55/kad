#include <catch2/catch_all.hpp>

#include <kad/common/json/recursive_filter.hpp>
#include <kad/common/json/serialize_json.hpp>
#include <kad/common/traits.hpp>

#include <print>
#include <string>
#include <vector>

using namespace kad::common;

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

JSON_SERIALIZE_STRUCT(SomeClassA);
JSON_SERIALIZE_STRUCT(SomeClassB);

template <typename InputType, kad::common::json::detail::SerializeWithFilterType Type>
[[nodiscard]] Type parse_json_object(InputType& input)
{
	std::vector<kad::common::json::RecursiveFilter*> object_filter;
	nlohmann::json::parser_callback_t filter_cb = [&](int depth, nlohmann::json::parse_event_t event, nlohmann::json& parsed)
	{
		// We should never see an array start/end as the top level root.
	};
	nlohmann::json::parse(input, filter_cb);
};

TEST_CASE("JSON filter", "[utils]")
{
	const auto& filter_a = nlohmann::adl_serializer<SomeClassA>::Filter;
	filter_a.Log();

	SomeClassA a{ .some_int = 66, .array_int = { 1, 2, 3 }, .array_double = { 43.23, 45645.5 } };

	nlohmann::json parsed_a = a;
	std::println("{}", parsed_a.dump(4));

	const auto& filter_b = nlohmann::adl_serializer<SomeClassB>::Filter;
	filter_b.Log();
}
