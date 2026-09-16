#pragma once

#include <kad/common/no_copy_or_move.hpp>

#include <subprocess.h>

#include <condition_variable>
#include <expected>
#include <filesystem>
#include <mutex>
#include <ranges>
#include <thread>
#include <unordered_map>
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

			std::unordered_map<std::string, std::string> env_vars{ };
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

		Process(std::filesystem::path target, Params params)
		{
			// Add our program to the arguments list.
			params.args.insert(params.args.begin(), std::move(target));

			// Parse all env vars, if an envvar is inherited and a parameter, take the parameter.
			if (params.inherit_env_var)
			{
				size_t i = 0;
				while (environ[i] != nullptr)
				{
					constexpr auto delimiter = '=';
					const std::string environ_str{ environ[i++] };

					const auto delim_it = environ_str.find(delimiter);
					if  (delim_it == std::string::npos) [[unlikely]]
					{
						throw ErrorCodeException(subprocess_error_invalid_environment);
					}

					const std::string key = environ_str.substr(0, delim_it);
					const std::string value  = environ_str.substr(delim_it + 1);

					params.env_vars.try_emplace(key, value);
				}
			}

			// Parse into owned array of strings
			auto env_vars_str = params.env_vars
				| std::views::transform([](const std::pair<std::string, std::string>& x){ return std::format("{}={}", x.first, x.second); })
				| std::ranges::to<std::vector<std::string>>();

			//
			//  Convert all owned string arrays into a list of ptrs.
			//

			auto arg_vars_ptr = params.args
				| std::views::transform([](const std::string& x){ return x.c_str(); })
				| std::ranges::to<std::vector<const char*>>();

			auto env_vars_ptr = env_vars_str
				| std::views::transform([](const std::string& x){ return x.c_str(); })
				| std::ranges::to<std::vector<const char*>>();

			// Need to null terminate both of the arrays.
			arg_vars_ptr.emplace_back(nullptr);
			env_vars_ptr.emplace_back(nullptr);

			const int result = subprocess_create_ex(
				arg_vars_ptr.data(),
				OPTIONS,
				env_vars_ptr.data(),
				params.working_directory.c_str(),
				&subprocess_
			);

			if (result != 0)
			{
				throw ErrorCodeException(result);
			}

			// Note: we can't attach stdout + stderr since we are doing async read.
			stdin_ = subprocess_stdin(&subprocess_);

			// Background thread used for reading out + err
			active_ = !params.ignore_stdout || !params.ignore_stderr;

			reader_thread_ = std::jthread([
					ignore_stdout = params.ignore_stdout,
					ignore_stderr = params.ignore_stderr,
					this, poll_interval = params.reader_poll_interval
				](std::stop_token stop_token)
			{
				std::array<char, 4096> buffer_out{ };
				std::array<char, 4096> buffer_err{ };

				while (subprocess_alive(&subprocess_))
				{
					uint32_t size_out = 0;
					uint32_t size_err = 0;

					if (!ignore_stdout)
					{
						size_out = subprocess_read_stdout(&subprocess_, buffer_out.data(), buffer_out.size());
					}

					if (!ignore_stderr)
					{
						size_err = subprocess_read_stderr(&subprocess_, buffer_err.data(), buffer_err.size());
					}

					if (size_out > 0 || size_err > 0)
					{
						std::lock_guard lock{ mutex_ };
						out_.append( buffer_out.data(), size_out );
						err_.append( buffer_err.data(), size_err );
						cv_.notify_all();
					}

					if (stop_token.stop_requested())
					{
						break;
					}

					std::this_thread::sleep_for(poll_interval);
				}

				//
				// One last set of polls to catch any missed log and to unblock any readers.
				//

				uint32_t size_out = 0;
				uint32_t size_err = 0;

				if (!ignore_stdout)
				{
					size_out = subprocess_read_stdout(&subprocess_, buffer_out.data(), buffer_out.size());
				}

				if (!ignore_stderr)
				{
					size_err = subprocess_read_stderr(&subprocess_, buffer_err.data(), buffer_err.size());
				}

				{
					std::lock_guard lock{ mutex_ };
					out_.append( buffer_out.data(), size_out );
					err_.append( buffer_err.data(), size_err );
					active_ = false;
					cv_.notify_all();
				}
			});
		}

		~Process()
		{
			// TODO: if we don't shut down by some deadline we should send SIGKILL
			if (Alive())
			{
				Terminate();
				Join();
			}

			// Stop logging thead if not already stopped
			if (reader_thread_.joinable())
			{
				reader_thread_.request_stop();
				reader_thread_.join();
			}

			// This frees any allocated objects.
			// Must  be  done after thread has shut down since we can still be accessing data from out/err.
			subprocess_destroy(&subprocess_);
		}

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

	[[nodiscard]] inline CreateProcessResult CreateProcess(std::filesystem::path binary, Process::Params params)
	{
		try
		{
			return std::make_unique<Process>(std::move(binary), std::move(params));
		}
		catch (const Process::ErrorCodeException& e)
		{
			return std::unexpected{ std::format("Encountered error - {}", e.what()) };
		}
	}

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
