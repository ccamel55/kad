#pragma once

#include <kad/common/no_copy_or_move.hpp>

#include <subprocess.h>

#include <condition_variable>
#include <expected>
#include <filesystem>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

namespace kad::common
{
	class Process : kad::common::NoCopy
	{
		static constexpr auto BUFFER_SIZE = 2048;

		static constexpr auto OPTIONS =
			subprocess_option_enable_async |
			subprocess_option_search_user_path |
			subprocess_option_enable_async_no_wait;

	public:
		enum class Output
		{
			OUT,
			ERROR
		};

		struct Params
		{
			bool ignore_stdout{ false };
			bool ignore_stderr{ false };

			bool inherit_env_var{ true };

			std::map<std::string, std::string> env_vars{ };
			std::vector<std::string> args{ };

			std::filesystem::path working_directory{ std::filesystem::current_path() };
			std::chrono::milliseconds reader_poll_interval{ std::chrono::milliseconds(1) };
		};

		class ErrorCodeException final : std::exception
		{
		public:
			explicit ErrorCodeException(int error_code)
			{
				switch (error_code)
				{
					case subprocess_error_invalid_options:
						what_ = "internal: invalid options";
						break;
					case subprocess_error_invalid_environment:
						what_ = "internal: invalid environment variables";
						break;
					case subprocess_error_not_found:
						what_ = "internal: cannot find process";
						break;
					case subprocess_error_permission_denied:
						what_ = "process: permission denied";
						break;
					case subprocess_error_no_memory:
						what_ = "process: no memory";
						break;
					case subprocess_error_pipe:
						what_ = "process: pipe";
						break;
					case subprocess_error_spawn:
						what_ = "process: spawn";
						break;
					case subprocess_error_not_supported:
						what_ = "process: not supported";
						break;
					default:
						what_ = "internal: unknown error";
						break;
				}
			}

			const char* what() const noexcept override
			{
				return what_.c_str();
			}

		private:
			std::string what_;

		};

		Process(std::filesystem::path target, Params params);
		~Process();

		[[nodiscard]] bool Alive() { return subprocess_alive(&subprocess_); }

		void Write(const char* input) { if (!stdin_) return; fputs(input, stdin_); }

		template <typename Fn>
			requires (std::invocable<Fn, Output, const std::string&>)
		bool Read(Fn&& callback)
		{
			bool active;

			{
				std::lock_guard lock{ mutex_ };
				active = active_;

				if (!out_.empty())
				{
					callback(Output::OUT, out_);
				}

				if (!err_.empty())
				{
					callback(Output::ERROR, err_);
				}

				out_.clear();
				err_.clear();
			}

			return active;
		}

		template <typename Fn>
			requires (std::invocable<Fn, Output, const std::string&>)
		bool ReadBlocking(Fn&& callback)
		{
			bool active;

			{
				std::unique_lock lock{ mutex_ };
				active = active_;

				if (active && out_.empty() && err_.empty())
				{
					cv_.wait(lock, [&](){ return !active_ || !out_.empty() || !err_.empty(); });
				}

				active = active_;

				if (!out_.empty())
				{
					callback(Output::OUT, out_);
				}

				if (!err_.empty())
				{
					callback(Output::ERROR, err_);
				}

				out_.clear();
				err_.clear();
			}

			return active;
		}

		void Stop() { if (!Alive()) return; kill(subprocess_.child, SIGSTOP); }
		void Continue() { if (!Alive()) return; kill(subprocess_.child, SIGKILL); }
		void Terminate() { if (!Alive()) return; kill(subprocess_.child, SIGTERM); }

		int Join()
		{
			if (!exit_code_ || Alive())
			{
				subprocess_join(&subprocess_, &exit_code_.emplace(0));
				stdin_ = nullptr;
			}

			return exit_code_.value();
		}

	private:
		subprocess_s subprocess_{ };
		FILE* stdin_{ nullptr };

		std::mutex mutex_;
		std::condition_variable cv_;

		bool active_{ false };

		std::string out_;
		std::string err_;

		std::jthread reader_thread_;
		std::optional<int> exit_code_{ std::nullopt };

	};

	using CreateProcessResult = std::expected<std::unique_ptr<Process>, std::string>;

	/// Create/start a process from the `binary` name and `params`
	[[nodiscard]] CreateProcessResult CreateProcess(std::filesystem::path binary, Process::Params params);

	template <typename Fn>
		requires (std::invocable<Fn, Process::Output, const std::string&>)
	int WaitUntilExit(Process& process, bool sink_out, bool sink_err, Fn&& callback)
	{
		while (true)
		{
			if (!process.ReadBlocking(std::forward<Fn>(callback)))
			{
				break;
			}
		}

		return process.Join();
	}
}
