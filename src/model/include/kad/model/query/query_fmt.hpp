#pragma once

#include <kad/common/format/override_class.hpp>
#include <kad/common/format/override_iterable.hpp>

#include <kad/model/shared/common_fmt.hpp>
#include <kad/model/query/query.hpp>

#include <format>

STD_FMT_CLASS(kad::model::query::Query::Request, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "kind({}) version({}) clinet({})",
		object.kind,
		object.version,
		object.client
	);
});

STD_FMT_CLASS(kad::model::query::Query, [](const auto& object, std::format_context& ctx)
{
	return std::format_to(
		ctx.out(), "requests({}) client({})",
		kad::common::FmtIterable(object.requests),
		object.client
	);
});
