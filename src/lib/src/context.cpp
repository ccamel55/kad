#include <kad/lib/context.hpp>
#include <kad/common/file.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace kad::lib;

namespace
{
	static constexpr auto KAD_FOLDER = ".kad";

	[[nodiscard]] std::string GetDailyLogName(const std::string prefix)
	{
		const auto now = std::chrono::system_clock::now();
		const auto time_t = std::chrono::system_clock::to_time_t(now);

		std::tm tm{ };
		localtime_r(&time_t, &tm);

		return std::format(
			"{}-{:04d}-{:02d}-{:02d}.log",
			prefix, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday
		);
	}
}

ResultStr<std::filesystem::path> kad::lib::FindRootDirectory(
	const std::filesystem::path& cwd,
	size_t max_depth
)
{
	std::filesystem::path current = std::filesystem::absolute(cwd);
	for (size_t i = 0; i < max_depth; ++i)
	{
		if (std::filesystem::is_directory(current / KAD_FOLDER))
		{
			return current;
		}

		if (!current.has_relative_path())
		{
			return std::unexpected{ std::format("{} folder could not be found", KAD_FOLDER) };
		}

		current = current.parent_path();
	}

	throw std::runtime_error("max search depth exceeded");
}

Context::Context(Context::Settings settings)
	: path_root_{ settings.path_root }
	, path_kad_{ common::file::GetDirectorySafe(path_root_ / KAD_FOLDER, settings.create_if_missing) }
	, path_log_{ path_kad_ / "logs" }
	, logger_{ [&]() {
		using namespace spdlog;
		// Add default sinks from sink bit set
		if (settings.log_sinks.is_set(Context::Settings::LogSink::FILE))
		{
			// Create log directory if it doesn't exist
			if (!std::filesystem::is_directory(path_log_))
			{
				std::filesystem::create_directory(path_log_);
			}

			path_log_ /= GetDailyLogName("lib-kad");

			auto& sink = settings.custom_sinks.emplace_back(std::make_shared<sinks::basic_file_sink_st>(path_log_.string()));
			sink->set_level(level::trace);
			sink->set_pattern("[%x %X] [%l] [%!] %v");

			// Write something to indicate start of file.
			sink->log({ "ENTERY", level::info, "----" });
		}

		// Load all our sinks into the logger
		auto logger = spdlog::logger{ "lib-kad", settings.custom_sinks.begin(), settings.custom_sinks.end() };
		logger.set_level(level::trace);

		return logger;
	}() }
	, config_{ this }
{
	SPDLOG_LOGGER_INFO(logger(), "Context created");
}

Context::~Context()
{
	SPDLOG_LOGGER_INFO(logger(), "Context destroyed");
}
