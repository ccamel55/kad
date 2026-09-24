#include <CLI/CLI.hpp>

#include <kad/bin/cli/cli.hpp>
#include <kad/bin/tui/tui.hpp>

#include <kad/lib/config.hpp>
#include <kad/lib/context.hpp>
#include <kad/lib/preset.hpp>

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

		const auto& config = context.config();
		const auto& active_preset = config.data().active_preset;
		if (active_preset.empty() || !config.FindPreset(active_preset))
		{
			std::println("active_preset({}) does not exist", active_preset);
			std::abort();
		}

		const auto& preset = *config.FindPreset(active_preset);
		if (!preset.GetApiResponseFile())
		{
			std::println("preset({}) missing cmake api response", active_preset);
			std::abort();
		}

		const auto index_file = preset.GetApiResponseFile().value();
		std::println("CMake index file: {}", index_file.string());

		const auto codemodel_file = GetCodeModelFile(index_file, "kad");
		if (!codemodel_file)
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
				data.targets.emplace(target.name);
			}

			for (const auto& abstract_target: configuration.abstract_targets)
			{
				data.targets.emplace(abstract_target.name);
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
