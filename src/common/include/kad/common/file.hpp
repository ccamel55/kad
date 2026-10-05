#pragma once

#include <filesystem>
#include <vector>

namespace kad::common::file
{
	/// Get a directory and check if it exists.
	/// If `create-if_not_exists` is false then the function throws if path does not exist, otherwise it will create the path.
	[[nodiscard]] std::filesystem::path GetDirectorySafe(const std::filesystem::path& directory, bool create_if_missing);

	/// Given some `path`, try find the relative path from `base`.
	/// If `path` is not relative to `base` then return the absolute path.
	[[nodiscard]] std::filesystem::path TryGetRelativeFromBase(
		const std::filesystem::path& path,
		const std::filesystem::path& base
	);

	/// For a given directory get all the files.
	[[nodiscard]] std::vector<std::filesystem::path> GetDirectoryEntries(const std::filesystem::path& path);
}
