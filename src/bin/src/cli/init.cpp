#include <kad/bin/cli/init.hpp>
#include <kad/bin/context/context.hpp>

#include <print>

using namespace kad::cli;

CommandInit::CommandInit(CLI::App* parent)
	: CommandBase{
		parent,
		parent->add_subcommand(
			"init",
			"Setup KAD or modify existing KAD instance"
		)
	}
{
	command_->require_subcommand(0);
	command_->subcommand_fallthrough(false);

	command_->callback([this]() { this->HandleCommand(); });
}

void CommandInit::HandleCommand()
{
	const auto folder = context::FindDataFolder();
	const auto folder_safe = folder.value_or(std::filesystem::current_path() / context::KAD_FOLDER);

	if (!folder.has_value())
	{
		std::println("Creating new kad folder at path({})", folder_safe.string());
	}

	context::Context context{ folder_safe, true };
}
