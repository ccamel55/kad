#pragma once

#include <kad/common/format/override_array.hpp>
#include <kad/common/format/override_class.hpp>
#include <kad/common/format/override_optional.hpp>

#include <kad/model/shared/common_fmt.hpp>
#include <kad/model/reply/codemodel.hpp>

#include <format>

STD_FMT_CLASS(kad::model::reply::CodeModel::Configurations::Target, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "name({}) id({}) directoryIndex({}) projectIndex({}) jsonFile({})",
		object.name,
		object.id,
		object.directoryIndex,
		object.projectIndex,
		object.jsonFile.string()
	);
});

STD_FMT_CLASS(kad::model::reply::CodeModel::Configurations, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "name({}) targets({}) abstractTargets({})",
		object.name,
		object.targets,
		object.abstractTargets
	);
});

STD_FMT_CLASS(kad::model::reply::CodeModel, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "paths({}) version({}) configurations({})",
		object.paths,
		object.version,
		object.configurations
	);
});
