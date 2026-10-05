#include <catch2/catch_all.hpp>

#include <kad/common/test/directory.hpp>
#include <kad/common/test/spdlog.hpp>

#include <kad/common/file.hpp>
#include <kad/common/string.hpp>

#include <kad/lib/context.hpp>

using namespace kad::lib;

TEST_CASE("FindRootDirectory - no root", "[lib]")
{
	kad::common::test::ScopedTempDirectory tmp_dir{ };

	const auto cwd = tmp_dir.path() / "hello" / "world";
	kad::common::test::CreateFolder(cwd);

	SECTION("No .kad folder")
	{
		// Make a random folder
		kad::common::test::CreateFolder(tmp_dir.path() / ".kad_like");
	}

	SECTION("With .kad file")
	{
		// Make something name .kad but not a folder.
		kad::common::test::CreateFile(tmp_dir.path() / ".kad", "hello world");
	}

	CHECK_FALSE(FindRootDirectory(cwd).has_value());
}

TEST_CASE("FindRootDirectory - with root", "[lib]")
{
	kad::common::test::ScopedTempDirectory tmp_dir{ };

	const auto cwd = tmp_dir.path() / "hello" / "kad";
	kad::common::test::CreateFolder(cwd);

	SECTION("Single .kad root")
	{
		// Make a single kad folder
		kad::common::test::CreateFolder(tmp_dir.path() / ".kad");

		CHECK(FindRootDirectory(cwd).has_value());
		CHECK(FindRootDirectory(cwd).value() == tmp_dir.path());
	}

	SECTION("Multiple .kad root")
	{
		// When multiple roots exist, we should take the first one.
		kad::common::test::CreateFolder(tmp_dir.path() / ".kad");
		kad::common::test::CreateFolder(cwd / ".kad");

		CHECK(FindRootDirectory(cwd).has_value());
		CHECK(FindRootDirectory(cwd).value() == cwd);
	}
}

TEST_CASE("Context - init - throw if missing", "[lib]")
{
	kad::common::test::ScopedTempDirectory tmp_dir{ };

	std::optional<Context> context;
	CHECK_THROWS_AS(
		context.emplace(Context::Settings
		{
			.path_root = tmp_dir.path(),
			.create_if_missing = false,
		}),
		std::runtime_error
	);
}

TEST_CASE("Context - init - create if missing", "[lib]")
{
	kad::common::test::ScopedTempDirectory tmp_dir{ };

	{
		Context context
		{
			Context::Settings
			{
				.path_root = tmp_dir.path(),
				.create_if_missing = true,
			}
		};
	}

	// We should be able to find root directory once it has been created.
	CHECK(FindRootDirectory(tmp_dir.path()).has_value());
	CHECK(FindRootDirectory(tmp_dir.path()).value() == tmp_dir.path());
}

TEST_CASE("Context - sinks", "[lib]")
{
	kad::common::test::ScopedTempDirectory tmp_dir{ };

	Context::Settings settings
	{
		.path_root = tmp_dir.path(),
		.create_if_missing = true
	};

	SECTION("File Sink")
	{
		{
			settings.log_sinks.set(Context::Settings::LogSink::FILE);
			Context context{ std::move(settings) };
		}

		const auto expected_log_path = tmp_dir.path() / ".kad" / "logs";
		const auto directory_log_entries = kad::common::file::GetDirectoryEntries(expected_log_path)
			| std::ranges::views::filter([](const std::filesystem::path& path){ return path.stem().string().starts_with("lib-kad-"); })\
			| std::ranges::to<std::vector>();

		CHECK_FALSE(directory_log_entries.empty());
	}

	SECTION("Custom Sink")
	{
		auto test_sink = std::make_shared<kad::common::test::test_sink_st>();
		test_sink->set_level(spdlog::level::trace);

		{
			settings.log_sinks.set(Context::Settings::LogSink::FILE);
			settings.custom_sinks.emplace_back(test_sink);

			Context context{ std::move(settings) };
		}

		CHECK_FALSE(test_sink->messages().empty());
	}
}
