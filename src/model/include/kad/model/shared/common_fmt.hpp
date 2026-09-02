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
