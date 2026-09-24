#include <kad/bin/cli/preset.hpp>
#include <kad/bin/process/cmake.hpp>

#include <kad/common/release_assert.hpp>

#include <print>

using namespace kad::cli;

namespace
{
	class CMake
	{
	public:
		explicit CMake(const std::filesystem::path& path_cmake = "cmake")
			: path_cmake_{ path_cmake }
		{ }

		void Configure()
		{

		}

		void Build()
		{

		}

	private:
		std::filesystem::path path_cmake_;

	};

	class SubCommandActive final : public CommandBase
	{
	public:
		explicit SubCommandActive(CommandBase* parent)
			: CommandBase{
				parent,
				parent->Command()->add_subcommand(
					"active",
					"Mark selected preset as active"
				)
			}
		{
			Command()->alias("a");
			Command()->require_subcommand(0);
			Command()->subcommand_fallthrough(false);
		}

	protected:
		void HandleCommandImpl() override
		{
			auto* parent_data = Parent()->DataAs<struct CommandPreset::Data>();
			auto& config = parent_data->context->config();

			if (config.data().active_preset == parent_data->preset)
			{
				std::println("Preset({}) already active", parent_data->preset);
				return;
			}

			config.SetActivePreset(parent_data->preset);
			std::println("Active preset({})", parent_data->preset);
		}

	};

	class SubCommandConfigure final : public CommandBase
	{
	public:
		explicit SubCommandConfigure(CommandBase* parent)
			: CommandBase{
				parent,
				parent->Command()->add_subcommand(
					"configure",
					"Run CMake configure on selected or active preset"
				)
			}
		{
			Command()->alias("c");
			Command()->require_subcommand(0);
			Command()->subcommand_fallthrough(false);
		}

	protected:
		void HandleCommandImpl() override
		{
			auto* parent_data = Parent()->DataAs<struct CommandPreset::Data>();
			auto& config = parent_data->context->config();

			const auto& preset = parent_data->preset.empty()
				? config.data().active_preset
				: parent_data->preset;

			auto it = config.presets().find(preset);
			if (it == config.presets().end())
			{
				std::println("Preset({}) does not exists", parent_data->preset);
				return;
			}

			kad::process::CMake cmake{ };
			auto handle = cmake.Configue({
				.cmake_root = config.context().path_root(),
				.build_directory = it->second.DataBuildDirectory(),
				.preset = preset
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

	};

	class SubCommandAdd final : public CommandBase
	{
	public:
		struct Data : public CommandData
		{
			std::filesystem::path build_directory;
		};

		explicit SubCommandAdd(CommandBase* parent)
			: CommandBase{
				parent,
				parent->Command()->add_subcommand(
					"add",
					"Add a CMake preset"
				)
			}
		{
			Command()->require_subcommand(0);
			Command()->subcommand_fallthrough(false);

			Command()
				->add_option("--build-dir", data_.build_directory, "CMake build directory associated with preset")
				->required(true);
		}

		CommandData* Data() override { return &data_; }
		const CommandData* Data() const override { return &data_; }

	private:
		void HandleCommandImpl() override
		{
			auto* parent_data = Parent()->DataAs<struct CommandPreset::Data>();
			auto& config = parent_data->context->config();

			if (config.FindPreset(parent_data->preset))
			{
				std::println("Preset({}) already exists", parent_data->preset);
				return;
			}

			auto& preset = config.CreatePreset(parent_data->preset, data_.build_directory);
			std::println("Added preset({}) with build_directory({})", parent_data->preset, preset.DataBuildDirectory().string());

			if (!preset.HasApiRequest())
			{
				preset.CreateApiRequest();
				std::println("Created API request, please reconfigure the preset");
			}
		}

	protected:
		struct Data data_;

	};

	class SubCommandRemove final : public CommandBase
	{
	public:
		explicit SubCommandRemove(CommandBase* parent)
			: CommandBase{
				parent,
				parent->Command()->add_subcommand(
					"remove",
					"Remove an existing preset"
				)
			}
		{
			Command()->require_subcommand(0);
			Command()->subcommand_fallthrough(false);
		}

	protected:
		void HandleCommandImpl() override
		{
			auto* parent_data = Parent()->DataAs<struct CommandPreset::Data>();
			auto& config = parent_data->context->config();

			auto it = config.presets().find(parent_data->preset);
			if (it == config.presets().end())
			{
				std::println("Preset({}) does not exists", parent_data->preset);
				return;
			}

			config.RemovePreset(it);
			std::println("Removed preset({})", parent_data->preset);
		}

	};
}

CommandPreset::CommandPreset(CLI::App* parent)
	: CommandBase{
		nullptr,
		parent->add_subcommand(
			"preset",
			"Manage CMake presets"
		)
	}
	, data_{
		.context = { [](auto& x){ x.emplace(FindRootFolderOrThrow()); } }
	}
{
	Command()->alias("p");
	Command()->require_subcommand(0);
	Command()->subcommand_fallthrough(false);

	auto preset = Command()
		->add_option("preset", data_.preset, "Preset name")
		->required(false);

	cmd_active = std::make_unique<SubCommandActive>(this);
	cmd_configure = std::make_unique<SubCommandConfigure>(this);
	cmd_add = std::make_unique<SubCommandAdd>(this);
	cmd_remove = std::make_unique<SubCommandRemove>(this);

	cmd_active->Command()->needs(preset);
	cmd_add->Command()->needs(preset);
	cmd_remove->Command()->needs(preset);
}

void CommandPreset::HandleCommandImpl()
{
	const auto& config = data_.context->config();
	if (data_.preset.empty())
	{
		// Show all preset information
		std::println("Available presets:");
		for (const auto& [name, preset]: config.presets())
		{
			const bool active = name == config.data().active_preset;
			std::println("\t- '{}' api_response({}) {}", name, preset.HasApiResponse(), active ? "(active)" : "");
		}
	}
	else
	{
		// Print info about selected preset
		const auto* preset = config.FindPreset(data_.preset);
		if (!preset) [[unlikely]]
		{
			std::println("Preset({}) does not exist", data_.preset);
			throw CLI::RuntimeError(1);
		}

		std::println("Preset: '{}'", data_.preset);
		std::println("\t- active: {}", config.data().active_preset == data_.preset);
		std::println("\t- {}: '{}'", lib::config::Preset::Name::BUILD_DIRECTORY, preset->DataBuildDirectory().string());
	}
}
