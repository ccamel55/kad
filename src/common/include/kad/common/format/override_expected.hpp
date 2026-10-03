#pragma once

#include <expected>
#include <format>

template <typename Type, typename Error>
struct std::formatter<std::expected<Type, Error>> : std::formatter<std::string_view>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const std::expected<Type, Error>& expected, std::format_context& ctx) const
	{
		return expected.has_value()
			? std::format_to(ctx.out(), "{}", expected.value())
			: std::format_to(ctx.out(), "{}", expected.error());
	}
};
