#include <kad/bin/context/config.hpp>
#include <kad/bin/context/context.hpp>
#include <kad/common/release_assert.hpp>

#include <nlohmann/json.hpp>

#include <fstream>

using namespace kad::context;

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

	std::filesystem::path ResolvePath(
		const std::filesystem::path& path,
		const std::filesystem::path& base
	)
	{
		const auto rel = std::filesystem::relative(path, base);
		const auto is_relative = !rel.empty() && rel.native()[0] != '.';

		return is_relative ? rel : std::filesystem::absolute(path);
	}
}

template <>
struct nlohmann::adl_serializer<kad::context::config::Config>
{
	using Type = kad::context::config::Config;

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
		nlohmann::json data_json(config_);
		{
			std::ofstream out(path_config_);
			out << data_json.dump(4);
		}
	}
	else
	{
		// Load existing config file
		std::ifstream in(path_config_);
		config_ = nlohmann::json::parse(in).get<config::Config>();

		if (config_.revision != config::REVISION) [[unlikely]]
		{
			if (!CanMigrate())
			{
				throw std::runtime_error(std::format(
					"config revisions incompatible, manual migration required. current_revision({}) config_revision({})",
					config::REVISION, config_.revision
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
	if (dirty_)
	{
		nlohmann::json data_json(config_);
		{
			std::ofstream out(path_config_);
			out << data_json.dump(4);
		}
	}
}

void Config::ResolveActivePreset()
{
	if (data().active_preset.empty())
	{
		return;
	}

	if (!presets_.contains(data().active_preset))
	{
		dirty_= true;
		data().active_preset = presets_.empty()
			? ""
			: presets_.begin()->first;
	}
}

void Config::SetActivePreset(const std::string& name)
{
	release_assert(presets_.contains(name), "preset must exist");

	dirty_ = true;
	data().active_preset = name;
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

	const auto build_dir_abs = std::filesystem::absolute(build_directory);
	const auto build_dir_parsed = ResolvePath(build_dir_abs, context_.path_root());

	const auto it = presets_.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(name),
		std::forward_as_tuple(*this, name, build_dir_parsed)
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
	if (name == data().active_preset)
	{
		ResolveActivePreset();
	}
}
