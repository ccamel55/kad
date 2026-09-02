#pragma once

#define STD_FMT_CLASS(ClassType, FormatCtxFn)									\
	template <>																	\
	struct std::formatter<ClassType>											\
	{																			\
		constexpr auto parse(std::format_parse_context& ctx)					\
		{																		\
			return ctx.begin();													\
		}																		\
		auto format(const ClassType& data, std::format_context& ctx) const		\
		{																		\
			return FormatCtxFn(data, ctx);										\
		}																		\
	};
