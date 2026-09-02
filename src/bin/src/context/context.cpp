#include <kad/bin/context/context.hpp>

using namespace kad::context;

namespace
{
	[[nodiscard]] std::filesystem::path GetPathSafe(const std::filesystem::path& path, bool create_if_not_exists)
	{
		if (!std::filesystem::exists(path))
		{
			if (create_if_not_exists)
			{
				std::filesystem::create_directories(path);
			}
			else
			{
				throw std::runtime_error(std::format("path({}) does not exist", path.string()));
			}
		}
		return path;
	}
}

std::optional<std::filesystem::path> kad::context::FindDataFolder(
	const std::filesystem::path& cwd,
	size_t max_depth
)
{
	std::filesystem::path current = std::filesystem::absolute(cwd);
	for (size_t i = 0; i < max_depth; ++i)
	{
		auto data_folder = current / KAD_FOLDER;
		if (std::filesystem::is_directory(data_folder))
		{
			return std::make_optional(std::move(data_folder));
		}

		if (!current.has_relative_path())
		{
			return std::nullopt;
		}

		current = current.parent_path();
	}

	throw std::runtime_error("max search depth exceeded");
}

Context::Context(const std::filesystem::path& path_kad, bool create_if_not_exists)
	: path_kad_{ GetPathSafe(path_kad, create_if_not_exists) }
	, config_{ path_kad }
{

}
