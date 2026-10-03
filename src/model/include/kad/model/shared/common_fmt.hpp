#pragma once

#include <kad/common/format/override_class.hpp>
#include <kad/model/shared/common.hpp>

#include <format>

STD_FMT_CLASS(kad::model::Path, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "source({}) build({})",
		object.source.string(),
		object.build.string()
	);
});

STD_FMT_CLASS(kad::model::Version, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "major({}) minor({})",
		object.major,
		object.minor
	);
});

STD_FMT_CLASS(kad::model::ReplyReference, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "kind({}) version({}) jsonFile({})",
		object.kind,
		object.version,
		object.jsonFile.string()
	);
});

STD_FMT_CLASS(kad::model::Error, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "error({})",
		object.error
	);
});

template <typename Type>
struct std::formatter<kad::model::MaybeError<Type>> : std::formatter<std::string_view>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const kad::model::MaybeError<Type>& maybe_error, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{}", maybe_error.base());
	}
};
