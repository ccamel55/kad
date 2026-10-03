#include <catch2/catch_all.hpp>

#include <kad/common/test/directory.hpp>
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
