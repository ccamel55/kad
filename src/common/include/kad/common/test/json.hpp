#pragma once

#include <catch2/catch_all.hpp>
#include <simdjson.h>

#include <print>

namespace kad::common::test
{
	template <bool Print = false, typename Type>
	void CheckTypeSerializeDeserialize(const Type& obj, const std::string_view json_string)
	{
		std::println("{}", obj);

		{
			std::string out;
			CHECK_FALSE(simdjson::to_json(obj, out));

			if constexpr (Print)
			{
				std::println("{}", out);
			}
		}

		{
			simdjson::padded_input json_str(json_string);

			simdjson::ondemand::parser parser;
			simdjson::ondemand::document parsed = parser.iterate(json_str);

			const auto value = parsed.get<Type>();

			CHECK_FALSE(value.error());
			CHECK(value.value_unsafe() == obj);
		}
	}
}
