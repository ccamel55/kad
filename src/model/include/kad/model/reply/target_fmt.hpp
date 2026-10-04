#pragma once

#include <kad/common/format/override_array.hpp>
#include <kad/common/format/override_class.hpp>
#include <kad/common/format/override_enum.hpp>
#include <kad/common/format/override_optional.hpp>

#include <kad/model/reply/target.hpp>

STD_FMT_ENUM(kad::model::reply::TargetType);

STD_FMT_CLASS(kad::model::reply::Target::Paths, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "source({}) build({})",
		object.source.string(),
		object.build.string()
	);
});

STD_FMT_CLASS(kad::model::reply::Target::Artifact, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "path({})",
		object.path.string()
	);
});

STD_FMT_CLASS(kad::model::reply::Target::Debugger, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "workingDirectory({})",
		object.workingDirectory.string()
	);
});

STD_FMT_CLASS(kad::model::reply::Target, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "name({}) id({}) type({}) paths({}) nameOnDisk({}) artifacts({}) debugger({})",
		object.name,
		object.id,
		object.type,
		object.paths,
		object.nameOnDisk,
		object.artifacts,
		object.debugger
	);
});
