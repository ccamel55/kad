#pragma once

#include <format>

namespace kad::common
{

template <typename IterableType>
struct FmtIterable
{
public:
	explicit FmtIterable(const IterableType& type)
		: ref_{ type }
	{ }



	const IterableType& ref_;
};

}

template <typename Type>
struct std::formatter<kad::common::FmtIterable<Type>> : std::formatter<std::string_view>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const kad::common::FmtIterable<Type>& iterable, std::format_context& ctx) const
	{
		std::string buffer{ "[" };
		for (auto it = iterable.ref_.begin(); it != iterable.ref_.end(); ++it)
		{
			if (it == iterable.ref_.begin())
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
