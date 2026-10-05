#include <kad/bin/cli/target.hpp>
#include <kad/bin/process/cmake.hpp>

#include <print>

using namespace kad::cli;

namespace
{
	class SubCommandBuild final : public CommandBase
	{
	public:
		explicit SubCommandBuild(CommandBase* parent)
			: CommandBase{
				parent,
				parent->Command()->add_subcommand(
					"build",
					"Build target"
				)
			}
		{
			Command()->alias("b");
			Command()->require_subcommand(0);
			Command()->subcommand_fallthrough(false);
		}

	protected:
		void HandleCommandImpl()
		{
			const auto* parent_data = Parent()->DataAs<struct CommandTarget::Data>();
			const auto& config = parent_data->context->config();

			const auto& preset_name = parent_data->preset.empty()
				? config.data().active_preset
				: parent_data->preset;

			const auto* preset = config.FindPreset(preset_name);
			if (!preset)
			{
				std::println("Preset({}) does not exists", preset_name);
				return;
			}

			const auto& target_name = parent_data->target;

			// Only check if target exists if it's not a known reserved keyword such as "all"
			if (target_name != "all")
			{
				const auto& targets = preset->GetTargets();
				const auto* target = targets->FindTarget(target_name);

				if (!target)
				{
					std::println("Preset({}) target({}) does not exists", preset_name, target_name);
					return;
				}
			}

			kad::process::CMake cmake{ };
			auto handle = cmake.Build({
				.build_directory = preset->DataBuildDirectory(),
				.target = target_name
			});

			if (!handle.has_value())
			{
				std::println("{}", handle.error());
				return;
			}

			const auto result = kad::common::WaitUntilExit(*handle.value(), true, true, [](
				const kad::common::Process::Output output,
				const std::string& data
			)
			{
				std::ignore = output;
				std::print("{}", data);
			});

			if (result != 0)
			{
				std::println("Program exited with code: {}", result);
			}
		}

	private:

	};

	class SubCommandBuildAndRun final : public CommandBase
	{
	public:
		struct Data : public CommandData
		{
			bool debug{ false };
			std::vector<std::string> args{ };
		};

		explicit SubCommandBuildAndRun(CommandBase* parent)
			: CommandBase{
				parent,
				parent->Command()->add_subcommand(
					"run",
					"Build and run target"
				)
			}
		{
			Command()->alias("r");
			Command()->require_subcommand(0);
			Command()->subcommand_fallthrough(false);

			Command()
				->add_option("--debug", data_.debug, "Send SIGSTOP upon starting to give us time for the debugger to attach")
				->required(false);

			Command()
				->add_option("--args", data_.args, " Arguments to be passed to executable, will override any default arguments")
				->required(false);
		}

		CommandData* Data() override { return &data_; }
		const CommandData* Data() const override { return &data_; }

	protected:
		void HandleCommandImpl() override
		{
			const auto* parent_data = Parent()->DataAs<struct CommandTarget::Data>();
			std::println(
				"Handled target->run command preset({}) target({}) debug({}) args({})",
				parent_data->preset,
				parent_data->target,
				data_.debug,
				data_.args
			);
		}

	private:
		struct Data data_;

	};
}

CommandTarget::CommandTarget(CLI::App* parent)
	: CommandBase{
		nullptr,
		parent->add_subcommand(
			"target",
			"Manage CMake targets"
		)
	}
	, data_{
		.context = { [](auto& x) {
			x.emplace(lib::Context::Settings
			{
				.path_root = FindRootFolderOrThrow(),
				.log_sinks = { lib::Context::Settings::LogSink::FILE }
			});
		} }
	}
{
	Command()->alias("t");
	Command()->require_subcommand(0);
	Command()->subcommand_fallthrough(false);

	Command()
		->add_option("-p,--preset", data_.preset, "CMake preset to use. Will take precidence over active preset.")
		->required(false);

	auto target = Command()
		->add_option("target", data_.target, "Target name")
		->required(false);

	cmd_build = std::make_unique<SubCommandBuild>(this);
	cmd_build_and_run = std::make_unique<SubCommandBuildAndRun>(this);

	cmd_build->Command()->needs(target);
	cmd_build_and_run->Command()->needs(target);
}

void CommandTarget::HandleCommandImpl()
{
	std::println("Handled target command: preset({}) name({})", data_.preset, data_.target);
}
