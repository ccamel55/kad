#include <kad/lib/preset.hpp>
#include <kad/lib/target.hpp>

#include <kad/lib/data.hpp>
#include <kad/lib/data_json.hpp>

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

Target::Target(Preset& preset, const std::string& name, const std::filesystem::path& path_target_json)
	: preset_{ preset }
	, path_target_json_{ path_target_json }
	, path_target_config_{ preset_.path_preset_folder() / name }
{

}

Target::~Target()
{

}
