#include <kad/lib/preset.hpp>
#include <kad/lib/target.hpp>

#include <nlohmann/json.hpp>

using namespace kad::lib;

namespace
{
	bool CanMigrate()
	{
		return false;
	}

	void ApplyMigration()
	{

	}
}

template <>
struct nlohmann::adl_serializer<kad::lib::config::Target>
{
	using Type = kad::lib::config::Target;

	static void from_json(const json& json, Type& object)
	{
		object.revision					= json.at(Type::Name::REVISION);
		object.environment_variables	= json.at(Type::Name::ENVIRONMENT_VARIABLES);
		object.arguments				= json.at(Type::Name::ARGUMENTS);
		object.workding_directory		= json.at(Type::Name::WORKING_DIRECTORY).get<std::filesystem::path>();
	}

	static void to_json(json& json, const Type& object)
	{
		json[Type::Name::REVISION]				= object.revision;
		json[Type::Name::ENVIRONMENT_VARIABLES]	= object.environment_variables;
		json[Type::Name::ARGUMENTS]				= object.arguments;
		json[Type::Name::WORKING_DIRECTORY]		= object.workding_directory;
	}
};

Target::Target(Preset& preset, const std::string& name, const std::filesystem::path& path_target_json)
	: preset_{ preset }
	, path_target_json_{ path_target_json }
	, path_target_config_{ preset_.path_preset_folder() / name }
{

}

Target::~Target()
{

}
