#pragma once

#include <kad/bin/context/config.hpp>
#include <kad/common/no_copy_or_move.hpp>

#include <filesystem>
#include <optional>

namespace kad::context
{
	/// Recursively search the current working directory and it's parents to find
	/// the root directory.
	[[nodiscard]] std::optional<std::filesystem::path> FindRootDirectory(
		const std::filesystem::path& cwd = std::filesystem::current_path(),
		size_t max_depth = 100
	);

	class Context : public kad::common::NoCopy
	{
	public:
		explicit Context(const std::filesystem::path& path_root, bool create_if_not_exists = false);

		[[nodiscard]] Config& config() { return config_; }
		[[nodiscard]] const Config& config() const { return config_; }

		[[nodiscard]] const std::filesystem::path& path_root() const { return path_root_; }
		[[nodiscard]] const std::filesystem::path& path_kad_folder() const { return path_kad_; }

	private:
		std::filesystem::path path_root_;
		std::filesystem::path path_kad_;

		Config config_;

	};
}
