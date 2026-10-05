#pragma once

#include <kad/common/json/serialize_json.hpp>

#include <filesystem>
#include <fstream>
#include <string>

namespace kad::common::json
{
	template <typename Type>
	[[nodiscard]] std::string Dump(const Type& type)
	{
		std::string out;
		if (auto err = simdjson::to_json(type, out); err) [[unlikely]]
		{
			throw std::runtime_error(simdjson::error_message(err));
		}
		return simdjson::fractured_json_string(out);
	}

	template <typename Type>
	void DumpFile(const Type& type, const std::filesystem::path& path)
	{
		std::ofstream out(path);
		out << Dump(type);
	}

	template <typename Type>
	[[nodiscard]] Type Parse(simdjson::padded_string input)
	{
		simdjson::ondemand::parser parser;
		simdjson::ondemand::document parsed = parser.iterate(input);

		Type type;
		if (auto err = parsed.get(type); err) [[unlikely]]
		{
			throw std::runtime_error(simdjson::error_message(err));
		}
		return type;
	}

	template <typename Type>
	[[nodiscard]] Type ParseFile(std::filesystem::path file)
	{
		auto padded_str = simdjson::padded_string::load(file.string());
		if (!padded_str.has_value()) [[unlikely]]
		{
			throw std::runtime_error(simdjson::error_message(padded_str.error()));
		}
		return Parse<Type>(std::move(padded_str.value()));
	}
}
