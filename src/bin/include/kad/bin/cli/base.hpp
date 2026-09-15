#pragma once

#include <kad/bin/context/context.hpp>

#include <CLI/CLI.hpp>

namespace kad::cli
{
	/// Recursively search for root folder and abort if it doesn't exist.
	[[nodiscard]] inline std::filesystem::path FindRootFolderOrThrow(
		const std::filesystem::path& cwd = std::filesystem::current_path(),
		size_t max_depth = 100
	)
	{
		const auto path = context::FindRootDirectory(cwd, max_depth);
		if (!path.has_value()) [[unlikely]]
		{
			throw CLI::ValidationError("Could not find root folder for current project");
		}
		return path.value();
	}

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
