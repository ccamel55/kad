#pragma once

#include <kad/common/no_copy_or_move.hpp>
#include <kad/common/random.hpp>

#include <catch2/catch_all.hpp>

#include <filesystem>
#include <fstream>
#include <format>
#include <print>

namespace kad::common::test
{
	class ScopedTempDirectory : public NoCopyOrMove
	{
		static constexpr auto SUBFOLDER = "kad-test";

	public:
		ScopedTempDirectory(const std::filesystem::path& parent = std::filesystem::temp_directory_path() / SUBFOLDER)
			: path_{ parent / RandomString(10) }
		{
			REQUIRE_FALSE((std::filesystem::exists(path_) || std::filesystem::is_directory(path_)));

			std::filesystem::create_directories(path_);
			std::println("created scoped temp directory: {}", path_.string());
		}

		~ScopedTempDirectory()
		{
			std::filesystem::remove_all(path_);
			std::println("deleted scoped temp directory: {}", path_.string());
		}

		[[nodiscard]] const std::filesystem::path& path() const { return path_; }

	private:
		std::filesystem::path path_;

	};

	inline void CreateFolder(const std::filesystem::path& folder)
	{
		std::filesystem::create_directories(folder);

		REQUIRE(std::filesystem::exists(folder));
		REQUIRE(std::filesystem::is_directory(folder));
	}

	inline void CreateFile(const std::filesystem::path& file, const std::string& file_data)
	{
		if (!std::filesystem::exists(file.parent_path()) || !std::filesystem::is_directory(file.parent_path()))
		{
			CreateFolder(file.parent_path());
		}

		{
			std::ofstream out{ file };
			out << file_data;
		}

		REQUIRE(std::filesystem::exists(file));
		REQUIRE(std::filesystem::is_regular_file(file));
	}
}
