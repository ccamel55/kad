#pragma once

#include <format>
#include <optional>

template <typename Type>
struct std::formatter<std::optional<Type>> : std::formatter<std::string_view>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const std::optional<Type>& optional, std::format_context& ctx) const
	{
		return optional.has_value()
			? std::format_to(ctx.out(), "{}", optional.value())
			: std::format_to(ctx.out(), "NULL");
	}
};
