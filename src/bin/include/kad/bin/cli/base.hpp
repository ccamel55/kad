#pragma once

#include <CLI/CLI.hpp>

namespace kad::cli
{
	[[nodiscard]] inline bool AppHasSubcommand(const CLI::App* app)
	{
		const auto subcommands = app->get_subcommands();
		const bool has_subcommand = std::ranges::find_if(subcommands, [](const CLI::App* subcommand)
		{
			return subcommand && subcommand->parsed();
		}) != subcommands.end();

		return has_subcommand;
	}

	class CommandBase
	{
	public:
		CommandBase(CLI::App* parent, CLI::App* command)
			: parent_{ parent }
			, command_{ command }
		{ }

	protected:
		CLI::App* parent_;
		CLI::App* command_;

	};
}
