#include <kad/lib/config.hpp>
#include <kad/lib/context.hpp>

#include <kad/common/release_assert.hpp>

#include <nlohmann/json.hpp>

#include <fstream>

using namespace kad::lib;

namespace
{
	constexpr auto KAD_CONFIG = "config.json";
	constexpr auto KAD_PRESETS = "presets";

	bool CanMigrate()
	{
		return false;
	}

	void ApplyMigration()
	{

	}
}

template <>
struct nlohmann::adl_serializer<kad::lib::config::Config>
{
	using Type = kad::lib::config::Config;

	static void from_json(const json& json, Type& object)
	{
		object.revision			= json.at(Type::Name::REVISION);
		object.active_preset	= json.at(Type::Name::ACTIVE_PRESET);
	}

	static void to_json(json& json, const Type& object)
	{
		json[Type::Name::REVISION]		= object.revision;
		json[Type::Name::ACTIVE_PRESET]	= object.active_preset;
	}
};

Config::Config(Context& context)
	: context_{ context }
	, path_config_{ context_.path_kad_folder() / KAD_CONFIG }
	, path_config_presets_{ context_.path_kad_folder() / KAD_PRESETS }
{
	if (!std::filesystem::is_regular_file(path_config_))
	{
		// Create root config file if it doesn't exist
		nlohmann::json data_json(data_.value());
		{
			std::ofstream out(path_config_);
			out << data_json.dump(4);
		}
	}
	else
	{
		// Load existing config file
		std::ifstream in(path_config_);
		data_ = nlohmann::json::parse(in).get<config::Config>();
		data_.SetClean();

		if (data_->revision != config::REVISION) [[unlikely]]
		{
			if (!CanMigrate())
			{
				throw std::runtime_error(std::format(
					"config revisions incompatible, manual migration required. current_revision({}) config_revision({})",
					config::REVISION, data_->revision
				));
			}

			ApplyMigration();
		}
	}

	if (!std::filesystem::is_directory(path_config_presets_))
	{
		// Create presets directory if it doesn't exist
		std::filesystem::create_directories(path_config_presets_);
	}
	else
	{
		// Look for all presets. Preset file name must match CMake preset name.
		for (const auto entry: std::filesystem::directory_iterator{ path_config_presets_ })
		{
			if (!entry.is_regular_file() || entry.path().extension() != PRESET_EXTENSION)
			{
				continue;
			}
			const std::string name = entry.path().stem().string();
			presets_.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(name),
				std::forward_as_tuple(*this, name)
			);
		}
	}

	ResolveActivePreset();
}

Config::~Config()
{
	if (data_.dirty())
	{
		nlohmann::json data_json(data_.value());
		{
			std::ofstream out(path_config_);
			out << data_json.dump(4);
		}
	}
}

void Config::ResolveActivePreset()
{
	if (data_->active_preset.empty())
	{
		return;
	}

	if (!presets_.contains(data_->active_preset))
	{
		data_.mut().active_preset = presets_.empty()
			? ""
			: presets_.begin()->first;
	}
}

void Config::SetActivePreset(const std::string& name)
{
	release_assert(presets_.contains(name), "preset must exist");
	data_.mut().active_preset = name;
}

Preset* Config::FindPreset(const std::string& name)
{
	const auto preset = presets_.find(name);
	return preset == presets_.end()
		? nullptr
		: &preset->second;
}

const Preset* Config::FindPreset(const std::string& name) const
{
	const auto preset = presets_.find(name);
	return preset == presets_.end()
		? nullptr
		: &preset->second;
}

Preset& Config::CreatePreset(const std::string& name, const std::filesystem::path& build_directory)
{
	release_assert(!presets_.contains(name), "preset must not already exist");

	const auto it = presets_.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(name),
		std::forward_as_tuple(*this, name, build_directory)
	);

	// If no active preset, we should set new preset as active.
	if (presets_.size() == 1)
	{
		SetActivePreset(name);
	}

	return it.first->second;
}

void Config::RemovePreset(const std::string& name)
{
	RemovePreset(presets_.find(name));
}

void Config::RemovePreset(Config::PresetMap::iterator it)
{
	release_assert(it != presets_.end(), "preset must exist");
	const auto name = it->first;

	it->second.Delete();
	presets_.erase(it);

	// If we removed active preset we should set new preset as active.
	if (name == data_->active_preset)
	{
		ResolveActivePreset();
	}
}
