#include <kad/lib/config.hpp>
#include <kad/lib/context.hpp>

#include <kad/common/release_assert.hpp>
#include <kad/common/json/helper.hpp>

#include <kad/lib/data.hpp>
#include <kad/lib/data_json.hpp>

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

Config::Config(Context& context)
	: context_{ context }
	, path_config_{ context_.path_kad_folder() / KAD_CONFIG }
	, path_config_presets_{ context_.path_kad_folder() / KAD_PRESETS }
{
	SPDLOG_LOGGER_INFO(logger(), "Config path({})", path_config_.string());
	SPDLOG_LOGGER_INFO(logger(), "Presets path({})", path_config_presets_.string());

	if (std::filesystem::is_regular_file(path_config_))
	{
		// Load existing config file
		data_ = common::json::ParseFile<config::Config>(path_config_);
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

		SPDLOG_LOGGER_DEBUG(logger(), "Loaded config from disk");
	}
	else
	{
		// Set to be dirty so that we create a default instance of the config.
		data_.SetDirty();

		SPDLOG_LOGGER_DEBUG(logger(), "Could not find config on disk, creating default instance");
	}

	if (std::filesystem::is_directory(path_config_presets_))
	{
		// Look for all presets. Preset file name must match CMake preset name.
		for (const auto& entry: std::filesystem::directory_iterator{ path_config_presets_ })
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

			SPDLOG_LOGGER_DEBUG(logger(), "Found preset({})", name);
		}
	}
	else
	{
		// Create presets directory if it doesn't exist
		std::filesystem::create_directories(path_config_presets_);
	}

	ResolveActivePreset();
}

Config::~Config()
{
	if (data_.dirty())
	{
		std::ofstream out(path_config_);
		out << common::json::Dump(data_.value());

		SPDLOG_LOGGER_DEBUG(logger(), "Overwritten config file({})", path_config_.string());
	}
}

spdlog::logger* Config::logger() const
{
	return context_.logger();
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
