#pragma once

#include <kad/common/format/override_class.hpp>
#include <kad/common/format/override_iterable.hpp>

#include <kad/model/shared/common_fmt.hpp>
#include <kad/model/reply/codemodel.hpp>

#include <format>

STD_FMT_CLASS(kad::model::reply::CodeModel::Configurations::Target, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "name({}) id({}) directory_index({}) project_index({}) json_file({})",
		object.name,
		object.id,
		object.directory_index,
		object.project_index,
		object.json_file.string()
	);
});

STD_FMT_CLASS(kad::model::reply::CodeModel::Configurations, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "name({}) targets({}) abstract_targets({})",
		object.name,
		kad::common::FmtIterable(object.targets),
		kad::common::FmtIterable(object.abstract_targets)
	);
});


STD_FMT_CLASS(kad::model::reply::CodeModel, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "paths({}) version({}) configurations({})",
		object.paths,
		object.version,
		kad::common::FmtIterable(object.configurations)
	);
});
