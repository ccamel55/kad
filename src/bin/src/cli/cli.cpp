#include <kad/bin/cli/cli.hpp>

#include <kad/bin/cli/init.hpp>
#include <kad/bin/cli/preset.hpp>
#include <kad/bin/cli/target.hpp>

using namespace kad::cli;

Cli::Cli(const CLI::App_p& app)
	: app_{ app  }
{
	// Callback is invoked after parsing CLI arguments
	app->callback([this]() { this->parsed_cli_command_ = AppHasSubcommand(this->app_.get()); });

	commands_.emplace_back(std::make_unique<CommandInit>(app_.get()));
	commands_.emplace_back(std::make_unique<CommandPreset>(app_.get()));
	commands_.emplace_back(std::make_unique<CommandTarget>(app_.get()));
}
