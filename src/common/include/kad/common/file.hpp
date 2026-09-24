#pragma once

#include <filesystem>

namespace kad::common::file
{
	/// Get a directory and check if it exists.
	/// If `create-if_not_exists` is false then the function throws if path does not exist, otherwise it will create the path.
	[[nodiscard]] std::filesystem::path GetDirectorySafe(const std::filesystem::path& directory, bool create_if_not_exists);

	// Given some `path`, try find the relative path from `base`.
	// If `path` is not relative to `base` then return the absolute path.
	[[nodiscard]] std::filesystem::path TryGetRelativeFromBase(
		const std::filesystem::path& path,
		const std::filesystem::path& base
	);
}
