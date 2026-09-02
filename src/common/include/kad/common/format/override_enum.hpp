#pragma once

#include <magic_enum/magic_enum.hpp>

#define STD_FORMAT_ENUM(EnumType) 													\
	template <>																		\
	struct std::formatter<EnumType>													\
	{																				\
		static_assert(std::is_enum_v<EnumType>, "Type must be enum.");				\
		constexpr auto parse(std::format_parse_context& ctx)						\
		{																			\
			return ctx.begin();														\
		}																			\
		auto format(const EnumType val, std::format_context& ctx) const				\
		{																			\
			return std::format_to(ctx.out(), "{}", magic_enum::enum_name(val));		\
		}																			\
	};
