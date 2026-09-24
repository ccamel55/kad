#include <kad/common/file.hpp>
#include <kad/common/release_assert.hpp>

#include <kad/lib/config.hpp>
#include <kad/lib/context.hpp>
#include <kad/lib/preset.hpp>

#include <kad/model/query/query.hpp>
#include <kad/model/query/query_json.hpp>
#include <kad/model/reply/codemodel.hpp>
#include <kad/model/reply/codemodel_json.hpp>

#include <nlohmann/json.hpp>

#include <fstream>

using namespace kad::lib;

namespace
{
	constexpr auto API_ID = "kad";
	constexpr auto API_REQUEST_FILENAME = "query.json";

	// Directories containing request and response
	const auto PATH_FILE_API_REQUEST	= std::filesystem::path(".cmake") / "api" / "v1" / "query" / std::format("client-{}", API_ID);
	const auto PATH_FILE_API_RESPONSE	= std::filesystem::path(".cmake") / "api" / "v1" / "reply";

	bool CanMigrate()
	{
		return false;
	}

	void ApplyMigration()
	{

	}
}

template <>
struct nlohmann::adl_serializer<kad::lib::config::Preset>
{
	using Type = kad::lib::config::Preset;

	static void from_json(const json& json, Type& object)
	{
		object.revision			= json.at(Type::Name::REVISION);
		object.build_directory	= json.at(Type::Name::BUILD_DIRECTORY).get<std::filesystem::path>();
	}

	static void to_json(json& json, const Type& object)
	{
		json[Type::Name::REVISION]			= object.revision;
		json[Type::Name::BUILD_DIRECTORY]	= object.build_directory;
	}
};

std::optional<std::filesystem::path> kad::lib::GetApiReplyFile(const std::filesystem::path& path)
{
	std::string reply_file_name;
	std::string reply_file_ordering;

	// File with largest lexicographic order is the latest/current file.
	// Index files follow naming: index-{random string}.json
	// Error files follow naming: error-{random-string}.json
	for (const auto& entry: std::filesystem::directory_iterator{ path })
	{
		if (entry.is_directory())
		{
			continue;
		}

		const auto& entry_path = entry.path();
		if (entry_path.extension().string() != ".json")
		{
			continue;
		}

		constexpr auto PREFIX_INDEX = "index-";
		constexpr auto PREFIX_ERROR = "error-";

		const std::string filename = entry_path.stem().string();
		std::string ordering;

		if (filename.starts_with(PREFIX_INDEX))
		{
			ordering = filename.substr(std::strlen(PREFIX_INDEX));
		}
		else if (filename.starts_with(PREFIX_ERROR))
		{
			ordering = filename.substr(std::strlen(PREFIX_ERROR));
		}
		else
		{
			continue;
		}

		if (ordering > reply_file_ordering)
		{
			reply_file_name = filename;
			reply_file_ordering = ordering;
		}
	}

	if (reply_file_name.empty())
	{
		return std::nullopt;
	}

	return std::make_optional(std::filesystem::absolute(path) / reply_file_name);
}

Preset::Preset(Config& config, const std::string& name)
	: config_{ config }
	, path_preset_file_{ config.path_config_presets() / std::filesystem::path{ name }.replace_extension(PRESET_EXTENSION) }
	, path_preset_folder_{ config.path_config_presets() / name }
	, data_{ [&](auto& x) {
		release_assert(std::filesystem::is_regular_file(path_preset_file_), "preset file must exist");

		std::ifstream in(path_preset_file_);
		auto& preset = x.emplace(nlohmann::json::parse(in).get<config::Preset>());

		if (preset->revision != config::REVISION_PRESET) [[unlikely]]
		{
			if (!CanMigrate())
			{
				throw std::runtime_error(std::format(
					"config revisions incompatible, manual migration required. current_revision({}) config_revision({})",
					config::REVISION_PRESET, preset->revision
				));
			}

			ApplyMigration();
		}
	}}
{
	release_assert(std::filesystem::is_regular_file(path_preset_file_), "preset file must exist");
	release_assert(std::filesystem::is_directory(path_preset_folder_), "preset folder must exist");
}

Preset::Preset(Config& config, const std::string& name, const std::filesystem::path& build_directory)
	: config_{ config }
	, path_preset_file_{ config.path_config_presets() / std::filesystem::path{ name }.replace_extension(PRESET_EXTENSION) }
	, path_preset_folder_{ config.path_config_presets() / name }
	, data_{ config::Preset{
		.build_directory = common::file::TryGetRelativeFromBase(build_directory, config.context().path_root())
	}}
{
	release_assert(!std::filesystem::is_regular_file(path_preset_file_), "preset file must not exist");
	release_assert(!std::filesystem::is_directory(path_preset_folder_), "preset folder must not exist");

	nlohmann::json data_json(data_.value().value());
	{
		std::ofstream out(path_preset_file_);
		out << data_json.dump(4);
	}

	std::filesystem::create_directory(path_preset_folder_);
}

Preset::~Preset()
{
	if (delete_)
	{
		std::filesystem::remove(path_preset_file_);
		std::filesystem::remove_all(path_preset_folder_);
		return;
	}

	if (data_.has_value() && data_.value().dirty())
	{
		nlohmann::json data_json(data_.value().value());
		{
			std::ofstream out(path_preset_file_);
			out << data_json.dump(4);
		}
	}
}

void Preset::Delete()
{
	release_assert(!delete_, "preset already marked for deletion");
	release_assert(std::filesystem::is_regular_file(path_preset_file_), "preset file must exist");

	delete_ = true;
}

void Preset::CreateApiRequest()
{
	const auto api_request_folder = DataBuildDirectory() / PATH_FILE_API_REQUEST;
	const auto api_request = model::query::Query{
		.requests = { model::query::Query::Request{
			.kind = "codemodel",
			.version = model::Version{ .major = 2, .minor = 9 }
		}}
	};

	// If build directory does not exist, create it so we can add our API request.
	if (!std::filesystem::exists(api_request_folder) || !std::filesystem::is_directory(api_request_folder)) [[unlikely]]
	{
		std::filesystem::create_directories(api_request_folder);
	}

	std::ofstream out(api_request_folder / API_REQUEST_FILENAME);
	out << nlohmann::json(api_request).dump(4);
}

bool Preset::HasApiRequest() const
{
	const auto api_request_file = DataBuildDirectory() / PATH_FILE_API_REQUEST / API_REQUEST_FILENAME;
	return std::filesystem::exists(api_request_file) && std::filesystem::is_regular_file(api_request_file);
}

bool Preset::HasApiResponse() const
{
	const auto api_response_folder = DataBuildDirectory() / PATH_FILE_API_RESPONSE;
	if (!std::filesystem::exists(api_response_folder) || !std::filesystem::is_directory(api_response_folder))
	{
		return false;
	}

	return GetApiReplyFile(api_response_folder).has_value();
}

[[nodiscard]] std::filesystem::path Preset::DataBuildDirectory() const
{
	const auto& build_dir = data().build_directory;
	return build_dir.is_absolute() ? build_dir : config().context().path_root() / build_dir;
}

Target* Preset::FindTarget(const std::string& name)
{
	const auto target = targets_.find(name);
	return target == targets_.end()
		? nullptr
		: &target->second;
}

const Target* Preset::FindTarget(const std::string& name) const
{
	const auto target = targets_.find(name);
	return target == targets_.end()
		? nullptr
		: &target->second;
}
