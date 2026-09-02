#pragma once

#include <kad/model/query/query.hpp>
#include <kad/model/shared/common_json.hpp>

#include <nlohmann/json.hpp>

template <>
struct nlohmann::adl_serializer<kad::model::query::Query::Request>
{
	using Type = kad::model::query::Query::Request;

	static void from_json(const json& json, Type& object)
	{
		object.kind		= json.at(Type::Name::KIND);
		object.version	= json.at(Type::Name::VERSION);
		object.client	= json.at(Type::Name::CLIENT);
	}

	static void to_json(json& json, const Type& object)
	{
		json[Type::Name::KIND]		= object.kind;
		json[Type::Name::VERSION]	= object.version;
		json[Type::Name::CLIENT]	= object.client;
	}
};

template <>
struct nlohmann::adl_serializer<kad::model::query::Query>
{
	using Type = kad::model::query::Query;

	static void from_json(const json& json, Type& object)
	{
		object.requests	= json.at(Type::Name::REQUESTS);
		object.client	= json.at(Type::Name::CLIENT);
	}

	static void to_json(json& json, const Type& object)
	{
		json[Type::Name::REQUESTS]	= object.requests;
		json[Type::Name::CLIENT]	= object.client;
	}
};
