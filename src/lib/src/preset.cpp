#include <kad/common/file.hpp>
#include <kad/common/release_assert.hpp>
#include <kad/common/json/helper.hpp>

#include <kad/lib/config.hpp>
#include <kad/lib/context.hpp>
#include <kad/lib/preset.hpp>

#include <kad/lib/data.hpp>
#include <kad/lib/data_json.hpp>

#include <kad/model/query/query.hpp>
#include <kad/model/query/query_json.hpp>
#include <kad/model/reply/codemodel.hpp>
#include <kad/model/reply/codemodel_json.hpp>

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

Preset::Preset(
	Config& config,
	const std::string& name,
	const std::filesystem::path& build_directory
)
	: config_{ config }
	, path_preset_file_{ config.path_config_presets() / std::filesystem::path{ name }.replace_extension(PRESET_EXTENSION) }
	, path_preset_folder_{ config.path_config_presets() / name }
	, data_{ [&](auto& x) {
		if (std::filesystem::is_regular_file(path_preset_file_))
		{
			// If the config file exists, we should try to reload it.
			auto& preset = x.emplace(kad::common::json::ParseFile<config::Preset>(path_preset_file_));
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
		}
		else if (!build_directory.empty())
		{
			// If we don't have a config file we should try to create one.
			x.emplace(config::Preset{
				.build_directory = common::file::TryGetRelativeFromBase(build_directory, config.context().path_root())
			});

			{
				std::ofstream out(path_preset_file_);
				out << common::json::Dump(data_.value().value());
			}
		}
		else [[unlikely]]
		{
			throw std::runtime_error("could not create new file, build directory empty");
		}

		if (!std::filesystem::is_directory(path_preset_folder_))
		{
			std::filesystem::create_directory(path_preset_folder_);
		}
	}}
{ }

Preset::~Preset()
{
	if (delete_)
	{
		if (std::filesystem::is_regular_file(path_preset_file_))
		{
			std::filesystem::remove(path_preset_file_);
		}

		if (std::filesystem::is_directory(path_preset_folder_))
		{
			std::filesystem::remove_all(path_preset_folder_);
		}
	}
	else if (data_.has_value() && data_.value().dirty())
	{
		std::ofstream out(path_preset_file_);
		out << common::json::Dump(data_.value().value());
	}
}

void Preset::Delete()
{
	release_assert(!delete_, "preset already marked for deletion");
	release_assert(std::filesystem::is_regular_file(path_preset_file_), "preset file must exist");

	delete_ = true;
}

void Preset::CreateApiRequest() const
{
	const auto api_request_folder = DataBuildDirectory() / PATH_FILE_API_REQUEST;
	const auto api_request = model::query::Query{
		.requests = { model::query::Query::Request{
			.kind = "codemodel",
			.version = model::Version{ .major = 2, .minor = 5 }
		}}
	};

	// If build directory does not exist, create it so we can add our API request.
	if (!std::filesystem::exists(api_request_folder) || !std::filesystem::is_directory(api_request_folder)) [[unlikely]]
	{
		std::filesystem::create_directories(api_request_folder);
	}

	{
		std::ofstream out(api_request_folder / API_REQUEST_FILENAME);
		out << common::json::Dump(api_request);
	}
}

bool Preset::HasApiRequest() const
{
	const auto api_request_file = DataBuildDirectory() / PATH_FILE_API_REQUEST / API_REQUEST_FILENAME;
	return std::filesystem::exists(api_request_file) && std::filesystem::is_regular_file(api_request_file);
}

bool Preset::HasApiResponse() const
{
	return GetApiResponseFile().has_value();
}

std::optional<Preset::IndexOrErrorFile> Preset::GetApiResponseFile() const
{
	const auto api_response_folder = DataBuildDirectory() / PATH_FILE_API_RESPONSE;
	if (!std::filesystem::exists(api_response_folder) || !std::filesystem::is_directory(api_response_folder))
	{
		return std::nullopt;
	}

	constexpr auto PREFIX_INDEX = "index-";
	constexpr auto PREFIX_ERROR = "error-";

	std::string reply_file_name;
	std::string reply_file_ordering;

	// File with largest lexicographic order is the latest/current file.
	// Index files follow naming: index-{random string}.json
	// Error files follow naming: error-{random-string}.json
	for (const auto& entry: std::filesystem::directory_iterator{ api_response_folder })
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

	auto response = (std::filesystem::absolute(api_response_folder) / reply_file_name).replace_extension(".json");

	return reply_file_name.starts_with(PREFIX_INDEX)
		? std::make_optional<Preset::IndexOrErrorFile>(std::move(response))
		: std::make_optional<Preset::IndexOrErrorFile>(std::unexpected{ std::move(response) });
}

std::optional<Preset::IndexOrError> Preset::GetApiResponse() const
{
	return std::nullopt;
}

std::filesystem::path Preset::DataBuildDirectory() const
{
	const auto& build_dir = data().build_directory;
	return build_dir.is_absolute() ? build_dir : config().context().path_root() / build_dir;
}
