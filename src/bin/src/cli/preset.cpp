#include <kad/bin/cli/preset.hpp>
#include <kad/common/release_assert.hpp>

#include <print>

using namespace kad::cli;

namespace
{
	class SubCommandActive final : public CommandBase
	{
	public:
		SubCommandActive(
			const CommandPreset::State& parent_state,
			const std::optional<kad::context::Context>& context,
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
			, parent_state_{ parent_state }
			, context_{ context }
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
			release_assert(context_, "context must exist");
			release_assert(!parent_state_.preset.empty(), "preset must be selected");

			std::println("Handled preset->actve command preset({})", parent_state_.preset);
		}

	private:
		const CommandPreset::State& parent_state_;
		const std::optional<kad::context::Context>& context_;

	};

	class SubCommandConfigure final : public CommandBase
	{
	public:
		SubCommandConfigure(
			const CommandPreset::State& parent_state,
			const std::optional<kad::context::Context>& context,
			CLI::App* parent
		)
			: CommandBase{
				parent,
				parent->add_subcommand(
					"configure",
					"Run CMake configure on selected or active preset"
				)
			}
			, parent_state_{ parent_state }
			, context_{ context }
		{
			command_->alias("c");
			command_->require_subcommand(0);
			command_->subcommand_fallthrough(false);

			command_->callback([this]() { this->HandleCommand(); });
		}

	private:
		void HandleCommand()
		{
			release_assert(context_, "context must exist");

			std::println("Handled preset->configure command preset({})", parent_state_.preset);
		}

	private:
		const CommandPreset::State& parent_state_;
		const std::optional<kad::context::Context>& context_;

	};

	class SubCommandAdd final : public CommandBase
	{
	public:
		struct State
		{
			std::filesystem::path build_directory;
		};

		SubCommandAdd(
			const CommandPreset::State& parent_state,
			const std::optional<kad::context::Context>& context,
			CLI::App* parent,
			CLI::Option* option_name
		)
			: CommandBase{
				parent,
				parent->add_subcommand(
					"add",
					"Add a CMake preset"
				)
			}
			, parent_state_{ parent_state }
			, context_{ context }
		{
			command_->needs(option_name);
			command_->require_subcommand(0);
			command_->subcommand_fallthrough(false);

			command_
				->add_option("--build-dir", state_.build_directory, "CMake build directory associated with preset")
				->check(CLI::ExistingPath)
				->required(true);

			command_->callback([this]() { this->HandleCommand(); });
		}

	private:
		void HandleCommand()
		{
			std::println("Handled preset->add command preset({})", parent_state_.preset);
		}

	private:
		const CommandPreset::State& parent_state_;
		const std::optional<kad::context::Context>& context_;
		State state_;

	};

	class SubCommandRemove final : public CommandBase
	{
	public:
		SubCommandRemove(
			const CommandPreset::State& parent_state,
			const std::optional<kad::context::Context>& context,
			CLI::App* parent,
			CLI::Option* option_name
		)
			: CommandBase{
				parent,
				parent->add_subcommand(
					"remove",
					"Remove an existing preset"
				)
			}
			, parent_state_{ parent_state }
			, context_{ context }
		{
			command_->needs(option_name);
			command_->require_subcommand(0);
			command_->subcommand_fallthrough(false);

			command_->callback([this]() { this->HandleCommand(); });
		}

	private:
		void HandleCommand()
		{
			std::println("Handled preset->remove command preset({})", parent_state_.preset);
		}

	private:
		const CommandPreset::State& parent_state_;
		const std::optional<kad::context::Context>& context_;

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

	commands_.emplace_back(std::make_unique<SubCommandActive>(state_, context_, command_, preset));
	commands_.emplace_back(std::make_unique<SubCommandConfigure>(state_, context_, command_));
	commands_.emplace_back(std::make_unique<SubCommandAdd>(state_, context_, command_, preset));
	commands_.emplace_back(std::make_unique<SubCommandRemove>(state_, context_, command_, preset));

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

void CommandPreset::HandleCommand()
{
	release_assert(context_, "context must exist");

	const auto& config = context_->config();
	if (state_.preset.empty())
	{
		// Show all preset information
		std::println("Available presets:");
		for (const auto& [name, preset]: config.presets())
		{
			const bool active = name == config.data().active_preset;
			std::println("\t- '{}' api_response({}) {}", name, preset.HasApiResponse(), active ? " (active)" : "");
		}
	}
	else
	{
		// Print info about selected preset
		const auto* preset = config.FindPreset(state_.preset);
		if (!preset) [[unlikely]]
		{
			std::println("Preset({}) does not exist", state_.preset);
			throw CLI::RuntimeError(1);
		}

		std::println("Preset: '{}'", state_.preset);
		std::println("\t- {}: '{}'", context::config::Preset::Name::BUILD_DIRECTORY, preset->data().build_directory.string());
	}
}
