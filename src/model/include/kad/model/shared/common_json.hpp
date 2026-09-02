#pragma once

#include <kad/model/shared/common.hpp>
#include <nlohmann/json.hpp>

template <>
struct nlohmann::adl_serializer<kad::model::Path>
{
	using Type = kad::model::Path;

	static void from_json(const json& json, Type& object)
	{
		object.source	= json.at(Type::Name::SOURCE).get<std::filesystem::path>();
		object.build	= json.at(Type::Name::BUILD ).get<std::filesystem::path>();
	}

	static void to_json(json& json, const Type& object)
	{
		json[Type::Name::SOURCE]	= object.source;
		json[Type::Name::BUILD]		= object.build;
	}
};

template <>
struct nlohmann::adl_serializer<kad::model::Version>
{
	using Type = kad::model::Version;

	static void from_json(const json& json, Type& object)
	{
		object.major = json.at(Type::Name::MAJOR);
		object.minor = json.at(Type::Name::MINOR);
	}

	static void to_json(json& json, const Type& object)
	{
		json[Type::Name::MAJOR] = object.major;
		json[Type::Name::MINOR] = object.minor;
	}
};
