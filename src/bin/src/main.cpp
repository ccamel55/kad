#include <CLI/CLI.hpp>

#include <kad/bin/target.hpp>

#include <kad/bin/cli/cli.hpp>
#include <kad/bin/tui/tui.hpp>

#include <kad/lib/context.hpp>

#include <kad/model/query/query.hpp>
#include <kad/model/query/query_fmt.hpp>
#include <kad/model/query/query_json.hpp>

#include <kad/model/reply/codemodel.hpp>
#include <kad/model/reply/codemodel_fmt.hpp>
#include <kad/model/reply/codemodel_json.hpp>

#include <kad/common/process.hpp>
#include <kad/common/string.hpp>
#include <kad/common/version.hpp>

#include <expected>
#include <filesystem>
#include <fstream>
#include <print>

namespace
{
	constexpr kad::common::Version VERSION = { 0, 0, 1 };

	template <typename Type>
	using ResultStr = std::expected<Type, std::string>;

	const std::string_view CLIENT_NAME = "kad";
	const std::filesystem::path RESPONSE_PATH = "/run/media/allan/external/srcs/kad/build/debug/.cmake/api/v1/reply";

	// TODO(ALLAN): we should really be checking index- and error- files and taking the latest one.
	// If we have an error file as latest, we should return unexpected with the error.
	[[nodiscard]] ResultStr<std::filesystem::path> GetReplyIndexFile(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
		{
			return std::unexpected{ std::format("Could not find API resonse at path({})", path.string()) };
		}

		std::string index_file_name;

		// File with largest lexicographic order is the latest/current file.
		// Index files allow follow naming: index-{random string}.json
		for (const auto& entry: std::filesystem::directory_iterator{ path })
		{
			if (entry.is_directory())
			{
				continue;
			}

			const auto& entry_path = entry.path();
			if (!entry_path.stem().string().starts_with("index-") || entry_path.extension().string() != ".json")
			{
				continue;
			}

			if (entry_path.filename().string() > index_file_name)
			{
				index_file_name = entry_path.filename().string();
			}
		}

		if (index_file_name.empty())
		{
			return std::unexpected{ std::format("Could not find API respose index file in path({})", path.string()) };
		}

		return std::filesystem::absolute(path) / index_file_name;
	}

	[[nodiscard]] ResultStr<std::filesystem::path> GetCodeModelFile(const std::filesystem::path& index_path, const std::string& client_name)
	{
		if (!std::filesystem::exists(index_path) || !std::filesystem::is_regular_file(index_path))
		{
			return std::unexpected{ std::format("Could not find API resonse index file({})", index_path.string()) };
		}

		std::ifstream index_file{ index_path };

		const std::string client_id = std::format("client-{}", client_name);
		const nlohmann::json document = nlohmann::json::parse(index_file);

		if (!document["reply"].contains(client_id) || !document["reply"][client_id].contains("query.json"))
		{
			return std::unexpected{ std::format("Could not find client({}) in reply index({})", client_id, index_path.string()) };
		}

		std::string codemodel_file_name;

		// Files should be parsed according to the CMake docs.
		// https://cmake.org/cmake/help/latest/manual/cmake-file-api.7.html#v1-reply-file-reference
		const nlohmann::json& responses = document["reply"][client_id]["query.json"]["responses"];
		for (const auto& response: responses)
		{
			// TODO(allan): maybe we want to warn about this too?
			// We only know about v2 of the codemodel API so any other major versions should be skipped.
			if (response["kind"] != "codemodel" || response["version"]["major"] != 2)
			{
				continue;
			}

			codemodel_file_name = response["jsonFile"];
			break;
		}

		if (codemodel_file_name.empty())
		{
			return std::unexpected{ std::format("Could not find codemodel repsone in index file({})", index_path.string()) };
		}

		return std::filesystem::absolute(index_path.parent_path()) / codemodel_file_name;
	}

	[[nodiscard]] ResultStr<kad::model::reply::CodeModel> LoadCodeModel(const std::filesystem::path& codemodel_path)
	{
		if (!std::filesystem::exists(codemodel_path) || !std::filesystem::is_regular_file(codemodel_path))
		{
			return std::unexpected{ std::format("Could not find codemodel file({})", codemodel_path.string()) };
		}

		std::ifstream codemodel_file{ codemodel_path };
		return nlohmann::json::parse(codemodel_file).get<kad::model::reply::CodeModel>();
	}
}

int main(int argc, char** argv)
{
	// CLI
	{
		CLI::App_p app = std::make_shared<CLI::App>(
			"KAD CMake command line friend",
			"kad"
		);

		app->set_help_all_flag("--help-all", "Print full help message and exit");
		app->set_version_flag("-v,--version", std::format("v{}-{}-{}", VERSION.major(), VERSION.major(), VERSION.patch()), "Version info");

		app->get_formatter()->column_width(50);

		app->require_subcommand(0);
		app->subcommand_fallthrough(false);

		kad::cli::Cli cli{ app };

		try
		{
			app->parse(argc, argv);
		}
		catch (const CLI::ParseError& e)
		{
			return app->exit(e);
		}

		if (cli.ParsedCliCommand())
		{
			return 0;
		}
	}

	// TUI
	// Only launch if we didn't pass any CLI commands
	{
		const auto root_folder = kad::lib::FindRootDirectory();
		if (!root_folder.has_value())
		{
			std::println("could not find kad folder for current project");
			std::println("use `kad init` to create one");
			std::abort();
		}

		kad::lib::Context context{ root_folder.value() };

		const auto index_file = GetReplyIndexFile(RESPONSE_PATH);
		if (!index_file)
		{
			std::println("{}", index_file.error());
			std::abort();
		}

		std::println("CMake index file: {}", index_file->string());

		const auto codemodel_file = GetCodeModelFile(index_file.value(), std::string(CLIENT_NAME));
		if (!index_file)
		{
			std::println("{}", codemodel_file.error());
			std::abort();
		}

		std::println("Client codemodel file: {}", codemodel_file->string());

		const auto code_model = LoadCodeModel(codemodel_file.value());
		if (!code_model)
		{
			std::println("{}", code_model.error());
			std::abort();
		}

		std::println("Version: {}", code_model->version);
		std::println("Path: {}", code_model->paths);

		kad::tui::TuiData data;

		for (const auto& configuration: code_model->configurations)
		{
			for (const auto& target: configuration.targets)
			{
				data.targets.emplace_back(target.name, kad::TargetType::TARGET);
			}

			for (const auto& abstract_target: configuration.abstract_targets)
			{
				data.targets.emplace_back(abstract_target.name, kad::TargetType::ABSTRACT_TARGET);
			}
		}

		// TUI instance
		{
			kad::tui::Tui tui{ data };
			tui.RunBlocking();
		}
	}

	return 0;
}
