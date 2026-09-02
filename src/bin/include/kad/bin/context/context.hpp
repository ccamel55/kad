#pragma once

#include <kad/bin/context/config.hpp>
#include <kad/common/no_copy_or_move.hpp>

#include <filesystem>
#include <optional>

namespace kad::context
{
	static constexpr auto KAD_FOLDER	= ".kad";

	/// Recursively search the current working directory and it's parents to find
	/// the data folder.
	[[nodiscard]] std::optional<std::filesystem::path> FindDataFolder(
		const std::filesystem::path& cwd = std::filesystem::current_path(),
		size_t max_depth = 100
	);

	class Context : public kad::common::NoCopy
	{
	public:
		explicit Context(const std::filesystem::path& path_kad, bool create_if_not_exists = false);

	private:
		std::filesystem::path path_kad_;

		Config config_;

	};
}
