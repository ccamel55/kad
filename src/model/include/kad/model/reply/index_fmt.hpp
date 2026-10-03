#pragma once

#include <kad/common/format/override_array.hpp>
#include <kad/common/format/override_class.hpp>
#include <kad/common/format/override_expected.hpp>
#include <kad/common/format/override_map.hpp>
#include <kad/common/format/override_optional.hpp>

#include <kad/model/reply/index.hpp>
#include <kad/model/shared/common_fmt.hpp>

STD_FMT_CLASS(kad::model::reply::Index::CMake::Version, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "major({}) minor({}) patch({}) suffix({}) string({}) isDirty({})",
		object.major,
		object.minor,
		object.patch,
		object.suffix,
		object.string,
		object.isDirty
	);
});

STD_FMT_CLASS(kad::model::reply::Index::CMake::Paths, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "cmake({}) ctest({}) cpack({}) root({})",
		object.cmake,
		object.ctest,
		object.cpack,
		object.root
	);
});

STD_FMT_CLASS(kad::model::reply::Index::CMake::Generator, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "multiConfig({}) name({}) platform({})",
		object.multiConfig,
		object.name,
		object.platform
	);
});


STD_FMT_CLASS(kad::model::reply::Index::CMake, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "version({}) paths({}) generator({})",
		object.version,
		object.paths,
		object.generator
	);
});

STD_FMT_CLASS(kad::model::reply::Index::Reply::Client::QueryJson, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "client({}) responses({})",
		object.client,
		object.responses
	);
});

STD_FMT_CLASS(kad::model::reply::Index::Reply::Client, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "query.json({})",
		object.queryJson
	);
});

STD_FMT_CLASS(kad::model::reply::Index::Reply, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "({})",
		object.clients
	);
});

STD_FMT_CLASS(kad::model::reply::Index, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "cmake({}) reply({})",
		object.cmake,
		object.reply
	);
});
