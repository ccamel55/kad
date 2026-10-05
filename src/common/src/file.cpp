#include <kad/common/file.hpp>

using namespace kad::common;

std::filesystem::path file::GetDirectorySafe(const std::filesystem::path& directory, bool create_if_missing)
{
	if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
	{
		if (create_if_missing)
		{
			std::filesystem::create_directories(directory);
		}
		else
		{
			throw std::runtime_error(std::format("directory({}) does not exist", directory.string()));
		}
	}
	return directory;
}

std::filesystem::path file::TryGetRelativeFromBase(
	const std::filesystem::path& path,
	const std::filesystem::path& base
)
{
	const auto rel = std::filesystem::relative(std::filesystem::absolute(path), base);
	const auto is_relative = !rel.empty() && rel.native()[0] != '.';

	return is_relative ? rel : std::filesystem::absolute(path);
}

std::vector<std::filesystem::path> file::GetDirectoryEntries(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
	{
		return { };
	}

	std::vector<std::filesystem::path> entries;
	for (auto entry: std::filesystem::directory_iterator{ path })
	{
		entries.emplace_back(entry.path());
	}

	return entries;
}
