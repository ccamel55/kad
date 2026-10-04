#include <CLI/CLI.hpp>

#include <kad/bin/cli/cli.hpp>
#include <kad/bin/tui/tui.hpp>

#include <kad/lib/config.hpp>
#include <kad/lib/context.hpp>
#include <kad/lib/preset.hpp>

#include <kad/model/query/query.hpp>
#include <kad/model/query/query_fmt.hpp>
#include <kad/model/query/query_json.hpp>

#include <kad/model/reply/index.hpp>
#include <kad/model/reply/index_fmt.hpp>
#include <kad/model/reply/index_json.hpp>

#include <kad/model/reply/codemodel.hpp>
#include <kad/model/reply/codemodel_fmt.hpp>
#include <kad/model/reply/codemodel_json.hpp>

#include <kad/common/json/helper.hpp>
#include <kad/common/process.hpp>
#include <kad/common/string.hpp>
#include <kad/common/version.hpp>

#include <print>

namespace
{
	constexpr kad::common::Version VERSION = { 0, 0, 1 };
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

		kad::lib::Context context{ kad::lib::Context::Settings
		{
			.path_root = root_folder.value(),
			.log_sinks = { kad::lib::Context::Settings::LogSink::FILE }
		}};

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

		const auto& code_model = preset.GetCodeModel();
		if (!code_model.has_value())
		{
			std::println("preset({}) missing codemodel {}", active_preset, code_model.error());
			std::abort();
		}

		kad::tui::TuiData data;

		for (const auto& configuration: code_model->configurations)
		{
			for (const auto& target: configuration.targets)
			{
				data.targets.emplace(target.name);
			}

			if (configuration.abstractTargets.has_value())
			{
				for (const auto& abstract_target: configuration.abstractTargets.value())
				{
					data.targets.emplace(abstract_target.name);
				}
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
