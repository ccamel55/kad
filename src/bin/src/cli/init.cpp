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

	command_
		->add_option("--root", state_.root, "Project root directory where .kad folder is created. This must be the root CMake directory.")
		->default_val(std::filesystem::current_path())
		->check(CLI::ExistingPath)
		->required(false);

	command_->callback([this]() { this->HandleCommand(); });
}

void CommandInit::HandleCommand()
{
	// Convert file path to absolute.
	state_.root = std::filesystem::absolute(state_.root);

	const auto folder = context::FindRootDirectory();
	const auto folder_safe = folder.value_or(state_.root);

	if (!folder.has_value())
	{
		// Check CMakeLists.txt exists
		std::filesystem::path cmake_file{ state_.root };
		cmake_file /= "CMakeLists.txt";

		if (!std::filesystem::exists(cmake_file) || !std::filesystem::is_regular_file(cmake_file)) [[unlikely]]
		{
			throw CLI::ValidationError(std::format("Path({}) is not a valid CMake directory", state_.root.string()));
		}

		std::println("Creating new kad folder in root path({})", folder_safe.string());
	}
	else
	{
		std::println("Kad folder already exists at root path({})", folder_safe.string());
	}

	context::Context context{ folder_safe, true };

	// TODO(ALLAN): remove this once we implement tui commands for creating presets etc.
	if (auto* preset = context.config().FindPreset("debug"); preset)
	{
		preset->CreateApiRequest();
	}
	else
	{
		auto& preset_ref = context.config().CreatePreset("debug", folder_safe / "build" / "debug");
		preset_ref.CreateApiRequest();
	}

	if (auto* preset = context.config().FindPreset("release"); preset)
	{
		preset->CreateApiRequest();
	}
	else
	{
		auto& preset_ref = context.config().CreatePreset("release", folder_safe / "build" / "release");
		preset_ref.CreateApiRequest();
	}
}
