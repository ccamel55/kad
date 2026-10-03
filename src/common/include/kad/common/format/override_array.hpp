#pragma once

#include <array>
#include <format>
#include <vector>

namespace kad::common::format
{
	template <typename Type>
	struct is_array: public std::false_type { };

	template <typename Type, size_t Size>
	struct is_array<std::array<Type, Size>>: public std::true_type { };

	template <typename Type>
	struct is_array<std::vector<Type>>: public std::true_type { };

	template <typename Type>
	constexpr bool is_array_v = is_array<Type>::value;
}

template <typename Type>
	requires (kad::common::format::is_array_v<Type>)
struct std::formatter<Type> : std::formatter<std::string_view>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const Type& iterable, std::format_context& ctx) const
	{
		std::string buffer{ "[" };
		for (auto it = iterable.begin(); it != iterable.end(); ++it)
		{
			if (it == iterable.begin())
			{
				std::format_to(std::back_inserter(buffer), "{}", *it);
			}
			else
			{
				std::format_to(std::back_inserter(buffer), ", {}", *it);
			}
		}
		buffer += "]";
		return std::formatter<std::string_view>::format(buffer, ctx);
	}
};
