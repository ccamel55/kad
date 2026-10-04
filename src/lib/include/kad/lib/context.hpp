#pragma once

#include <kad/lib/config.hpp>

#include <kad/common/alias.hpp>
#include <kad/common/bit_flag.hpp>
#include <kad/common/no_copy_or_move.hpp>

#include <spdlog/spdlog.h>

#include <filesystem>

namespace kad::lib
{
	/// Recursively search the current working directory and it's parents to find
	/// the root directory.
	[[nodiscard]] ResultStr<std::filesystem::path> FindRootDirectory(
		const std::filesystem::path& cwd = std::filesystem::current_path(),
		size_t max_depth = 100
	);

	class Context : public kad::common::NoCopyOrMove
	{
	public:
		struct Settings
		{
			enum class LogSink
			{
				NONE 	= 0,
				FILE 	= 1 << 0,
			};

			std::filesystem::path path_root;
			bool create_if_not_exist{ false };

			kad::common::BitFlag<LogSink> log_sinks{ LogSink::NONE };
			std::vector<std::shared_ptr<spdlog::sinks::sink>> custom_sinks{ };
		};

		explicit Context(Settings settings);
		~Context();

		[[nodiscard]] spdlog::logger* logger() const { return &logger_; }

		[[nodiscard]] Config& config() { return config_; }
		[[nodiscard]] const Config& config() const { return config_; }

		[[nodiscard]] const std::filesystem::path& path_root() const { return path_root_; }
		[[nodiscard]] const std::filesystem::path& path_kad_folder() const { return path_kad_; }

	private:
		std::filesystem::path path_root_;
		std::filesystem::path path_kad_;

		mutable spdlog::logger logger_;

		Config config_;
	};
}
