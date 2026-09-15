#include <kad/bin/cli/init.hpp>
#include <kad/bin/context/context.hpp>

#include <print>

using namespace kad::cli;

CommandInit::CommandInit(CLI::App* parent)
	: CommandBase{
		nullptr,
		parent->add_subcommand(
			"init",
			"Setup KAD or modify existing KAD instance"
		)
	}
{
	Command()->require_subcommand(0);
	Command()->subcommand_fallthrough(false);

	Command()
		->add_option("--root", data_.root, "Project root directory where .kad folder is created. This must be the root CMake directory.")
		->default_val(std::filesystem::current_path())
		->check(CLI::ExistingPath)
		->required(false);
}

void CommandInit::HandleCommandImpl()
{
	// Convert file path to absolute.
	data_.root = std::filesystem::absolute(data_.root);

	const auto folder = context::FindRootDirectory();
	const auto folder_safe = folder.value_or(data_.root);

	if (!folder.has_value())
	{
		// Check CMakeLists.txt exists
		std::filesystem::path cmake_file{ data_.root };
		cmake_file /= "CMakeLists.txt";

		if (!std::filesystem::exists(cmake_file) || !std::filesystem::is_regular_file(cmake_file)) [[unlikely]]
		{
			throw CLI::ValidationError(std::format("Path({}) is not a valid CMake directory", data_.root.string()));
		}

		std::println("Creating new kad folder in root path({})", folder_safe.string());
	}
	else
	{
		std::println("Kad folder already exists at root path({})", folder_safe.string());
	}

	context::Context context{ folder_safe, true };
}
