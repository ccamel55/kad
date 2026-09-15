#include <kad/bin/context/preset.hpp>
#include <kad/common/release_assert.hpp>

#include <nlohmann/json.hpp>

#include <fstream>

using namespace kad::context;

namespace
{
	constexpr auto API_ID = "kad";
	constexpr auto API_REQUEST_FILENAME = "query.json";

	// Directories containing request and response
	const auto PATH_FILE_API_REQUEST	= std::filesystem::path(".cmake") / "api" / "v1" / "query" / std::format("client-{}", API_ID);
	const auto PATH_FILE_API_RESPONSE	= std::filesystem::path(".cmake") / "api" / "v1" / "reply";

	[[nodiscard]] nlohmann::json ApiRequest()
	{
		nlohmann::json requests = nlohmann::json::array();
		{
			auto& request = requests.emplace_back(nlohmann::json::object());
			request["kind"] = "codemodel";
			request["version"]["major"] = 2;
			request["version"]["minor"] = 9;
		}

		nlohmann::json json;
		json["client"] = nlohmann::json::object();
		json["requests"] = std::move(requests);
		return json;
	}

	bool CanMigrate()
	{
		return false;
	}

	void ApplyMigration()
	{

	}

	[[nodiscard]] std::optional<std::filesystem::path> GetApiReplyFile(const std::filesystem::path& path)
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
}

template <>
struct nlohmann::adl_serializer<kad::context::config::Preset>
{
	using Type = kad::context::config::Preset;

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

Preset::Preset(const std::filesystem::path& path_presets, const std::string& name)
	: path_preset_file_{ path_presets / std::filesystem::path{ name }.replace_extension(PRESET_EXTENSION) }
	, path_preset_folder_{ path_presets / name }
{
	release_assert(std::filesystem::is_regular_file(path_preset_file_), "preset file must exist");
	release_assert(std::filesystem::is_directory(path_preset_folder_), "preset folder must exist");
}

Preset::Preset(const std::filesystem::path& path_presets, const std::string& name, const std::filesystem::path& build_directory)
	: path_preset_file_{ path_presets / std::filesystem::path{ name }.replace_extension(PRESET_EXTENSION) }
	, path_preset_folder_{ path_presets / name }
	, preset_{ std::make_optional(config::Preset{
		.build_directory = build_directory
	})}
{
	release_assert(!std::filesystem::is_regular_file(path_preset_file_), "preset file must not exist");
	release_assert(!std::filesystem::is_directory(path_preset_folder_), "preset folder must not exist");

	nlohmann::json data_json(*preset_);
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

	if (dirty_ && preset_)
	{
		nlohmann::json data_json(*preset_);
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
	const auto api_request_folder = data().build_directory / PATH_FILE_API_REQUEST;
	const auto api_request = ApiRequest();

	// If build directory does not exist, create it so we can add our API request.
	if (!std::filesystem::exists(api_request_folder) || !std::filesystem::is_directory(api_request_folder)) [[unlikely]]
	{
		std::filesystem::create_directories(api_request_folder);
	}

	std::ofstream out(api_request_folder / API_REQUEST_FILENAME);
	out << api_request.dump(4);
}

bool Preset::HasApiRequest() const
{
	const auto api_request_file = data().build_directory / PATH_FILE_API_REQUEST / API_REQUEST_FILENAME;
	return std::filesystem::exists(api_request_file) && std::filesystem::is_regular_file(api_request_file);
}

bool Preset::HasApiResponse() const
{
	const auto api_response_folder = data().build_directory / PATH_FILE_API_RESPONSE;
	if (!std::filesystem::exists(api_response_folder) || !std::filesystem::is_directory(api_response_folder))
	{
		return false;
	}

	return GetApiReplyFile(api_response_folder).has_value();
}

void Preset::TryLoadPreset() const
{
	if (preset_.has_value())
	{
		return;
	}

	release_assert(std::filesystem::is_regular_file(path_preset_file_), "preset file must exist");

	std::ifstream in(path_preset_file_);
	preset_ = nlohmann::json::parse(in).get<config::Preset>();

	if (preset_->revision != config::REVISION_PRESET) [[unlikely]]
	{
		if (!CanMigrate())
		{
			throw std::runtime_error(std::format(
				"config revisions incompatible, manual migration required. current_revision({}) config_revision({})",
				config::REVISION_PRESET, preset_->revision
			));
		}

		ApplyMigration();
	}
}
