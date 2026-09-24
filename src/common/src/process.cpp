#include <kad/common/process.hpp>

#include <ranges>

using namespace kad::common;

Process::Process(std::filesystem::path target, Params params)
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

Process::~Process()
{
	// TODO(ALLAN): if we don't shut down by some deadline we should send SIGKILL
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

CreateProcessResult kad::common::CreateProcess(std::filesystem::path binary, Process::Params params)
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
