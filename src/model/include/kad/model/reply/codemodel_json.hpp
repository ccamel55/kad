#pragma once

#include <kad/model/shared/common_json.hpp>
#include <kad/model/reply/codemodel.hpp>

#include <nlohmann/json.hpp>

template <>
struct nlohmann::adl_serializer<kad::model::reply::CodeModel::Configurations::Target>
{
	using Type = kad::model::reply::CodeModel::Configurations::Target;

	static void from_json(const json& json, Type& object)
	{
		object.name				= json.at(Type::Name::NAME);
		object.id				= json.at(Type::Name::ID);
		object.directory_index	= json.at(Type::Name::DIRECTORY_INDEX);
		object.project_index	= json.at(Type::Name::PROJECT_INDEX);
		object.json_file		= json.at(Type::Name::JSON_FILE).get<std::filesystem::path>();
	}

	static void to_json(json& json, const Type& object)
	{
		json[Type::Name::NAME]				= object.name;
		json[Type::Name::ID]				= object.id;
		json[Type::Name::DIRECTORY_INDEX]	= object.directory_index;
		json[Type::Name::PROJECT_INDEX]		= object.project_index;
		json[Type::Name::JSON_FILE]			= object.json_file;
	}
};

template <>
struct nlohmann::adl_serializer<kad::model::reply::CodeModel::Configurations>
{
	using Type = kad::model::reply::CodeModel::Configurations;

	static void from_json(const json& json, Type& object)
	{
		object.name				= json.at(Type::Name::NAME);
		object.targets			= json.at(Type::Name::TARGETS);

		if (auto it = json.find(Type::Name::ABSTRACT_TARGETS); it != json.end() && !it->empty())
		{
			object.abstract_targets	= json.at(Type::Name::ABSTRACT_TARGETS);
		}
	}

	static void to_json(json& json, const Type& object)
	{
		json[Type::Name::NAME]		= object.name;
		json[Type::Name::TARGETS]	= object.targets;

		if (!object.abstract_targets.empty())
		{
			json[Type::Name::ABSTRACT_TARGETS] = object.abstract_targets;
		}
	}
};

template <>
struct nlohmann::adl_serializer<kad::model::reply::CodeModel>
{
	using Type = kad::model::reply::CodeModel;

	static void from_json(const json& json, Type& object)
	{
		if (json.at("kind") != Type::kind) [[unlikely]]
		{
			throw std::runtime_error(std::format("Kind({}) does not match expected kind(codemodel)", json.at("kind")));
		}

		object.paths			= json.at(Type::Name::PATHS);
		object.version			= json.at(Type::Name::VERSION);
		object.configurations	= json.at(Type::Name::CONFIGURATIONS);
	}

	static void to_json(json& json, const Type& object)
	{
		json["kind"]						= Type::kind;
		json[Type::Name::PATHS] 			= object.paths;
		json[Type::Name::VERSION]			= object.version;
		json[Type::Name::CONFIGURATIONS]	= object.configurations;
	}
};
