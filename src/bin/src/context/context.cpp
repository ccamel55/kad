#include <kad/bin/context/context.hpp>

using namespace kad::context;

namespace
{
	static constexpr auto KAD_FOLDER	= ".kad";
}

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

std::optional<std::filesystem::path> kad::context::FindRootDirectory(
	const std::filesystem::path& cwd,
	size_t max_depth
)
{
	std::filesystem::path current = std::filesystem::absolute(cwd);
	for (size_t i = 0; i < max_depth; ++i)
	{
		if (std::filesystem::is_directory(current / KAD_FOLDER))
		{
			return std::make_optional(std::move(current));
		}

		if (!current.has_relative_path())
		{
			return std::nullopt;
		}

		current = current.parent_path();
	}

	throw std::runtime_error("max search depth exceeded");
}

Context::Context(const std::filesystem::path& path_root, bool create_if_not_exists)
	: path_root_{ path_root }
	, path_kad_{ GetPathSafe(path_root_ / KAD_FOLDER, create_if_not_exists) }
	, config_{ *this }
{ }
