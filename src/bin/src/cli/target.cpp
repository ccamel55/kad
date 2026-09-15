#include <kad/bin/cli/target.hpp>
#include <kad/common/format/override_iterable.hpp>

#include <print>

using namespace kad::cli;

namespace
{
	class SubCommandBuild final : public CommandBase
	{
	public:
		SubCommandBuild(
			const CommandTarget::State& parent_state,
			const std::optional<kad::context::Context>& context,
			CLI::App* parent,
			CLI::Option* option_name
		)
			: CommandBase{
				parent,
				parent->add_subcommand(
					"build",
					"Build target"
				)
			}
			, parent_state_{ parent_state }
		{
			command_->needs(option_name);
			command_->alias("b");
			command_->require_subcommand(0);
			command_->subcommand_fallthrough(false);

			command_->callback([this]() { this->HandleCommand(); });
		}

	private:
		void HandleCommand()
		{
			std::println("Handled target->build command preset({}) target({})", parent_state_.preset, parent_state_.target);
		}

	private:
		const CommandTarget::State& parent_state_;

	};

	class SubCommandBuildAndRun final : public CommandBase
	{
	public:
		struct State
		{
			bool debug;
			std::vector<std::string> args;
		};

		SubCommandBuildAndRun(
			const CommandTarget::State& parent_state,
			const std::optional<kad::context::Context>& context,
			CLI::App* parent,
			CLI::Option* option_name
		)
			: CommandBase{
				parent,
				parent->add_subcommand(
					"run",
					"Build and run target"
				)
			}
			, parent_state_{ parent_state }
		{
			command_->needs(option_name);
			command_->alias("r");
			command_->require_subcommand(0);
			command_->subcommand_fallthrough(false);

			command_
				->add_option("--debug", state_.debug, "Send SIGSTOP upon starting to give us time for the debugger to attach")
				->required(false);

			command_
				->add_option("--args", state_.args, " Arguments to be passed to executable, will override any default arguments")
				->required(false);

			command_->callback([this]() { this->HandleCommand(); });
		}

	private:
		void HandleCommand()
		{
			std::println(
				"Handled target->run command preset({}) target({}) debug({}) args({})",
				parent_state_.preset,
				parent_state_.target,
				state_.debug,
				kad::common::FmtIterable{ state_.args }
			);
		}

	private:
		const CommandTarget::State& parent_state_;
		State state_;

	};
}

CommandTarget::CommandTarget(CLI::App* parent)
	: CommandBase{
		parent,
		parent->add_subcommand(
			"target",
			"Manage CMake targets"
		)
	}
{
	command_->alias("t");
	command_->require_subcommand(0);
	command_->subcommand_fallthrough(false);

	command_
		->add_option("-p,--preset", state_.preset, "CMake preset to use. Will take precidence over active preset.")
		->required(false);

	auto target = command_
		->add_option("target", state_.target, "Target name")
		->required(false);

	commands_.emplace_back(std::make_unique<SubCommandBuild>(state_, context_, command_, target));
	commands_.emplace_back(std::make_unique<SubCommandBuildAndRun>(state_, context_, command_, target));

	command_->callback([this]()
	{
		context_.emplace(FindRootFolderOrThrow());

		if (AppHasSubcommand(command_))
		{
			return;
		}
		this->HandleCommand();
	});
}

void CommandTarget::HandleCommand()
{
	std::println("Handled target command: preset({}) name({})", state_.preset, state_.target);
}
