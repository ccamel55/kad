#include <catch2/catch_all.hpp>

#include <kad/common/file.hpp>
#include <kad/common/string.hpp>

#include <kad/common/json/helper.hpp>
#include <kad/common/test/directory.hpp>

#include <kad/lib/config.hpp>
#include <kad/lib/context.hpp>

#include <kad/lib/data.hpp>
#include <kad/lib/data_json.hpp>

using namespace kad::lib;

namespace
{
	[[nodiscard]] Context GetContext(const std::filesystem::path& path)
	{
		Context::Settings settings
		{
			.path_root = path,
			.create_if_missing = true
		};

		return Context{ std::move(settings) };
	}
}

TEST_CASE("Config - init - load config", "[lib]")
{
	kad::common::test::ScopedTempDirectory tmp_dir{ };

	const auto config_path = tmp_dir.path() / ".kad" / "config.json";
	{
		const kad::lib::config::Config config
		{
			.revision = kad::lib::config::REVISION,
			.active_preset = "not-a-real-preset"
		};

		// Create config file for us to read
		kad::common::test::CreateFolder(config_path.parent_path());
		kad::common::json::DumpFile(config, config_path);

		CHECK(std::filesystem::is_regular_file(config_path));
	}

	{
		const auto context = GetContext(tmp_dir.path());
		const auto& config = context.config();

		// Note: active preset should have been restored to default since config preset doesn't exist.
		CHECK(config.data().revision == kad::lib::config::REVISION);
		CHECK(config.data().active_preset == "");
	}

	const auto file_config = kad::common::json::ParseFile<kad::lib::config::Config>(config_path);
	CHECK(file_config.revision == kad::lib::config::REVISION);
	CHECK(file_config.active_preset == "");
}

TEST_CASE("Config - init - create if missing", "[lib]")
{
	kad::common::test::ScopedTempDirectory tmp_dir{ };

	const auto config_path = tmp_dir.path() / ".kad" / "config.json";

	{
		const auto context = GetContext(tmp_dir.path());
		const auto& config = context.config();

		std::ignore = config;

		// Config file should not be written until the program has exited.
		CHECK_FALSE(std::filesystem::is_regular_file(config_path));

		CHECK(config.data().revision == kad::lib::config::REVISION);
		CHECK(config.data().active_preset == "");
	}

	// Program exists, config file should exist too.
	CHECK(std::filesystem::is_regular_file(config_path));

	const auto file_config = kad::common::json::ParseFile<kad::lib::config::Config>(config_path);
	CHECK(file_config.revision == kad::lib::config::REVISION);
	CHECK(file_config.active_preset == "");
}

TEST_CASE("Config - init - invalid config", "[lib]")
{
	kad::common::test::ScopedTempDirectory tmp_dir{ };

	const auto config_path = tmp_dir.path() / ".kad" / "config.json";
	{
		// Create config file that has bad entries
		kad::common::test::CreateFile(config_path, "not a real config");
		CHECK(std::filesystem::is_regular_file(config_path));
	}

	{
		// Loading should result in us throwing
		CHECK_THROWS_AS(GetContext(tmp_dir.path()), std::runtime_error);
	}
}

TEST_CASE("Config - presets")
{
	kad::common::test::ScopedTempDirectory tmp_dir{ };

	const auto presets_path = tmp_dir.path() / ".kad" / "presets";
	const auto build_path = tmp_dir.path() / "build";

	// Create presets
	{
		constexpr std::array<std::string_view, 2> PRESETS =
		{
			"debug",
			"release"
		};

		for (const auto& preset: PRESETS)
		{
			const kad::lib::config::Preset preset_obj
			{
				.revision = kad::lib::config::REVISION_PRESET,
				.build_directory = build_path
			};

			kad::common::test::CreateFolder(presets_path / preset);
			kad::common::json::DumpFile(preset_obj, (presets_path / preset).replace_extension(".json"));
		}
	}

	auto context = GetContext(tmp_dir.path());
	auto& config = context.config();

	CHECK(config.presets().size() == 2);

	// If active preset is none but we loaded a few presets,
	// the first one should be defaulted as the "default" preset.
	CHECK(config.data().active_preset == "debug");

	CHECK(config.FindPreset("debug"));
	CHECK(config.FindPreset("release"));

	SECTION("Set active presets")
	{
		config.SetActivePreset("release");
		CHECK(config.data().active_preset  == "release");
	}

	SECTION("Create preset")
	{
		// Creating the preset should not change the active preset since one already exists
		config.CreatePreset("hello", build_path / "hello");
		CHECK(config.data().active_preset  == "debug");
	}

	SECTION("Remove preset")
	{
		// Removing active preset should result in us choosing next preset.
		config.RemovePreset("debug");
		CHECK(config.data().active_preset  == "release");

		config.RemovePreset("release");
		CHECK(config.data().active_preset.empty());
	}
}
