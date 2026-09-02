#pragma once

#include <CLI/CLI.hpp>

#include <kad/bin/cli/base.hpp>

#include <memory>
#include <vector>

namespace kad::cli
{
	class Cli
	{
	public:
		explicit Cli(const CLI::App_p& app);

		[[nodiscard]] bool ParsedCliCommand() const { return parsed_cli_command_; }

	private:
		CLI::App_p app_;

		bool parsed_cli_command_{ false };
		std::vector<std::unique_ptr<CommandBase>> commands_;

	};

}
