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

#include <kad/model/reply/index.hpp>
#include <kad/model/reply/index_json.hpp>
#include <kad/model/reply/codemodel.hpp>
#include <kad/model/reply/codemodel_json.hpp>

#include <fstream>

using namespace kad::lib;

namespace
{
	constexpr auto API_ID = "kad";
	constexpr auto API_REQUEST_FILENAME = "query.json";

	const auto CLIENT_ID = std::format("client-{}", API_ID);

	// Directories containing request and response
	const auto PATH_FILE_API_REQUEST	= std::filesystem::path(".cmake") / "api" / "v1" / "query" / CLIENT_ID;
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
	, data_{ [this, &config, &build_directory](auto& x) {
		if (std::filesystem::is_regular_file(path_preset_file_))
		{
			// If the config file exists, we should try to reload it.
			auto& preset = x.emplace(kad::common::json::ParseFile<config::Preset>(path_preset_file_));
			if (preset->revision != config::REVISION_PRESET) [[unlikely]]
			{
				SPDLOG_LOGGER_TRACE(
					logger(), "Revision on disk does not match current config revision ({} vs {})",
					preset->revision,
					config::REVISION_PRESET
				);

				if (!CanMigrate())
				{
					throw std::runtime_error(std::format(
						"Config revisions incompatible, manual migration required. current_revision({}) config_revision({})",
						config::REVISION_PRESET, preset->revision
					));
				}

				ApplyMigration();
			}

			SPDLOG_LOGGER_DEBUG(logger(), "Loaded preset({}) from disk", path_preset_file_.string());
		}
		else if (!build_directory.empty())
		{
			// If we don't have a config file we should try to create one.
			// Also mark it as dirty so the file gets created on destruction.
			x.emplace(config::Preset{.build_directory = common::file::TryGetRelativeFromBase(build_directory, config.context().path_root())});
			x.value().SetDirty();

			SPDLOG_LOGGER_DEBUG(logger(), "Could not find preset({}) on disk, creating default instance", path_preset_file_.string());
		}
		else [[unlikely]]
		{
			throw std::runtime_error("Could not create new file, build directory empty");
		}

		if (!std::filesystem::is_directory(path_preset_folder_))
		{
			std::filesystem::create_directory(path_preset_folder_);
		}
	}, [this](auto& x) {
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

			SPDLOG_LOGGER_DEBUG(
				logger(), "Deleted preset file({}) folder({}) from disk",
				path_preset_file_.string(),
				path_preset_folder_.string()
			);
		}
		else if (x.dirty())
		{
			{
				std::ofstream out(path_preset_file_);
				out << common::json::Dump(x.value());
			}

			SPDLOG_LOGGER_DEBUG(logger(), "Overwritten preset file({})", path_preset_file_.string());
		}
	}},
	reply_index_{[this](auto& x) {
		const auto api_response_file_opt = GetApiResponseFile();
		if (!api_response_file_opt.has_value())
		{
			x.emplace(std::unexpected{ api_response_file_opt.error() });
			return;
		}

		const auto& api_response_file = api_response_file_opt.value();
		const auto& api_response_file_path = api_response_file.has_value()
			? api_response_file.value()
			: api_response_file.error();

		if (!std::filesystem::exists(api_response_file_path) || !std::filesystem::is_regular_file(api_response_file_path))
		{
			x.emplace(std::unexpected{ std::format("Could not find api_response({}) on disk", api_response_file_path.string()) });
			return;
		}

		auto api_response = kad::common::json::ParseFile<kad::model::reply::Index>(api_response_file_path);

		api_response_file.has_value()
			? x.emplace(Preset::IndexOrError{ std::move(api_response) })
			: x.emplace(Preset::IndexOrError{ std::unexpected{ std::move(api_response) }});

		SPDLOG_LOGGER_DEBUG(
			logger(), "Loaded api response file({}) is_error({})",
			api_response_file_path.string(),
			api_response_file.has_value() == false
		);
	}},
	reply_codemodel_{[this]( auto& x) {
		const auto codemodel_file_opt = GetCodeModelFile();
		if (!codemodel_file_opt.has_value())
		{
			x.emplace(std::unexpected{ codemodel_file_opt.error() });
			return;
		}

		const auto& codemodel_file = codemodel_file_opt.value();
		if (!std::filesystem::exists(codemodel_file) || !std::filesystem::is_regular_file(codemodel_file))
		{
			x.emplace(std::unexpected{ std::format("Could not find codemodel({}) on disk", codemodel_file.string()) });
			return;
		}

		x.emplace(kad::common::json::ParseFile<kad::model::reply::CodeModel>(codemodel_file));
		SPDLOG_LOGGER_DEBUG(logger(), "Loaded codemodel file({})", codemodel_file.string());
	}}
{ }

spdlog::logger* Preset::logger() const
{
	return config_.logger();
}

void Preset::Delete()
{
	release_assert(!delete_, "preset already marked for deletion");
	release_assert(std::filesystem::is_regular_file(path_preset_file_), "preset file must exist");

	// Need to mark as dirty otherwise we won't call load/save code.
	delete_ = true;
	data_->SetDirty();
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

ResultStr<Preset::IndexOrErrorFile> Preset::GetApiResponseFile() const
{
	const auto api_response_folder = ApiResponseFolder();
	if (!std::filesystem::exists(api_response_folder) || !std::filesystem::is_directory(api_response_folder))
	{
		return std::unexpected{ std::format("Could not find api response folder({})", api_response_folder.string()) };
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
		return std::unexpected{ std::format("Could not find any responses in api response folder({})", api_response_folder.string()) };
	}

	auto response = (std::filesystem::absolute(api_response_folder) / reply_file_name).replace_extension(".json");

	return reply_file_name.starts_with(PREFIX_INDEX)
		? Preset::IndexOrErrorFile{ std::move(response) }
		: Preset::IndexOrErrorFile{ std::unexpected{ std::move(response) }};
}

ResultStr<std::filesystem::path> Preset::GetCodeModelFile() const
{
	const auto& api_response_opt = GetApiResponse();
	if (!api_response_opt.has_value() || !api_response_opt.value().has_value())
	{
		return std::unexpected{ std::format(
			"Could not get api response error({}) is_error({})",
			api_response_opt.error_or(""),
			api_response_opt.transform([](auto& x){ return x.has_value(); }).value_or(true)
		) };
	}

	const auto& api_response = api_response_opt.value().value();
	const auto reply_client = api_response.reply.clients.find(CLIENT_ID);

	if (reply_client == api_response.reply.clients.end() || !reply_client->second.queryJson.has_value())
	{
		return std::unexpected{ std::format(
			"Could not find client({}) in api response error({})",
			reply_client->first,
			reply_client != api_response.reply.clients.end() ? reply_client->second.queryJson.error().error : ""
		) };
	}

	const auto& responses = reply_client->second.queryJson->responses;
	if (!responses.has_value())
	{
		return std::unexpected{ std::format("Api response has no client responses error({})", responses.error().error) };
	}

	std::filesystem::path codemodel_file;
	for (const auto& response: responses.value())
	{
		if (!response.has_value())
		{
			continue;
		}

		// TODO(allan): maybe we want to warn about this too?
		// We only know about v2 of the codemodel API so any other major versions should be skipped.
		if (response->kind != "codemodel" || response->version.major != 2)
		{
			continue;
		}

		codemodel_file = response->jsonFile;
		break;
	}

	if (codemodel_file.empty())
	{
		return std::unexpected{ "Could not find valid codemodel response" };
	}

	codemodel_file = std::filesystem::absolute(ApiResponseFolder()) / codemodel_file;
	return codemodel_file;
}

std::filesystem::path Preset::DataBuildDirectory() const
{
	const auto& build_dir = data().build_directory;
	return build_dir.is_absolute() ? build_dir : config().context().path_root() / build_dir;
}

std::filesystem::path Preset::ApiResponseFolder() const
{
	return DataBuildDirectory() / PATH_FILE_API_RESPONSE;
}
