#pragma once

#include <format>
#include <map>
#include <unordered_map>

namespace kad::common::format
{
	template <typename Type>
	struct is_map: public std::false_type { };

	template <typename Key, typename Value>
	struct is_map<std::map<Key, Value>>: public std::true_type { };

	template <typename Key, typename Value>
	struct is_map<std::unordered_map<Key, Value>>: public std::true_type { };

	template <typename Type>
	constexpr bool is_map_v = is_map<Type>::value;
}

template <typename Type>
	requires (kad::common::format::is_map_v<Type>)
struct std::formatter<Type> : std::formatter<std::string_view>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const Type& iterable, std::format_context& ctx) const
	{
		std::string buffer{ "{" };
		for (auto it = iterable.begin(); it != iterable.end(); ++it)
		{
			if (it == iterable.begin())
			{
				std::format_to(std::back_inserter(buffer), "{} -> {}", it->first, it->second);
			}
			else
			{
				std::format_to(std::back_inserter(buffer), ", {} -> {}", it->first, it->second);
			}
		}
		buffer += "}";
		return std::formatter<std::string_view>::format(buffer, ctx);
	}
};
