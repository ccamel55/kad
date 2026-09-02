#include <kad/bin/context/config.hpp>

#include <nlohmann/json.hpp>

#include <fstream>

using namespace kad::context;

namespace
{
	constexpr auto KAD_CONFIG = "config.json";
	constexpr auto KAD_PRESETS = "presets";
}

template <>
struct nlohmann::adl_serializer<kad::context::config::Data>
{
	using Type = kad::context::config::Data;

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

Config::Config(const std::filesystem::path& path_kad)
	: path_config_{ path_kad / KAD_CONFIG }
	, path_config_presets_{ path_kad / KAD_PRESETS }
{
	// Create root config file.
	if (!std::filesystem::is_regular_file(path_config_))
	{
		data_.revision = config::REVISION;
		data_.active_preset = "";

		nlohmann::json data_json(data_);
		{
			std::ofstream out(path_config_);
			out << data_json.dump(4);
		}
	}
	else
	{
		std::ifstream in(path_config_);
		data_ = nlohmann::json::parse(in).get<config::Data>();

		if (data_.revision != config::REVISION)
		{
			throw std::runtime_error(std::format(
				"config revisions incompatible, manual migration required. current_revision({}) config_revision({})",
				config::REVISION, data_.revision
			));
		}
	}

	// Create presets config file.
	if (!std::filesystem::is_directory(path_config_presets_))
	{
		std::filesystem::create_directories(path_config_presets_);
	}
	else
	{
		for (const auto entry: std::filesystem::directory_iterator{ path_config_presets_ })
		{
			if (!entry.is_regular_file() || entry.path().extension() != ".json")
			{
				continue;
			}
			presets_.emplace(entry.path().stem());
		}
	}

	if (!data_.active_preset.empty())
	{
		if (!presets_.contains(data_.active_preset))
		{
			data_.active_preset = presets_.empty()
				? ""
				: *presets_.begin();
		}
	}
}

Config::~Config()
{
	// TODO: we should probably only write when we have to
	nlohmann::json data_json(data_);
	{
		std::ofstream out(path_config_);
		out << data_json.dump(4);
	}
}
