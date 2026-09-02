#include <kad/bin/cli/preset.hpp>

#include <print>

using namespace kad::cli;

namespace
{
	class SubCommandActive final
		: public CommandBase
	{
	public:
		SubCommandActive(
			const CommandPreset::State& parent_state,
			CLI::App* parent,
			CLI::Option* option_name
		)
			: CommandBase{
				parent,
				parent->add_subcommand(
					"active",
					"Mark selected preset as active"
				)
			}
		{
			command_->needs(option_name);
			command_->alias("a");
			command_->require_subcommand(0);
			command_->subcommand_fallthrough(false);

			command_->callback([this]() { this->HandleCommand(); });
		}

	private:
		void HandleCommand()
		{
			std::println("Handled preset->actve command");
		}

	private:

	};
}

CommandPreset::CommandPreset(CLI::App* parent)
	: CommandBase{
		parent,
		parent->add_subcommand(
			"preset",
			"Manage CMake presets"
		)
	}
{
	command_->alias("p");
	command_->require_subcommand(0);
	command_->subcommand_fallthrough(false);

	auto preset = command_
		->add_option("preset", state_.preset, "Preset name")
		->required(false);

	commands_.emplace_back(std::make_unique<SubCommandActive>(state_, command_, preset));

	command_->callback([this]()
	{
		if (AppHasSubcommand(command_))
		{
			return;
		}
		this->HandleCommand();
	});
}

void CommandPreset::HandleCommand()
{
	std::println("Handled preset command: preset({})", state_.preset);
}
