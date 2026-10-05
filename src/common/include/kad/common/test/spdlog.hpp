#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>

namespace kad::common::test
{
	// TODO(ALLAN): add some sort of filter for the sink.
	template <typename Mutex>
	class test_sink final : public spdlog::sinks::base_sink<Mutex>
	{
	public:
		void sink_it_(const spdlog::details::log_msg& msg) override
		{
 			msgs_.emplace_back(msg);
		}

		void flush_() override { }
		void set_pattern_(const std::string &pattern) override { }
		void set_formatter_(std::unique_ptr<spdlog::formatter> sink_formatter) override { }

		void Clear() { msgs_.clear(); }

		[[nodiscard]] std::vector<spdlog::details::log_msg>& messages() { return msgs_; }
		[[nodiscard]] const std::vector<spdlog::details::log_msg>& messages() const { return msgs_; }

	private:
		std::vector<spdlog::details::log_msg> msgs_;

	};

	using test_sink_st = test_sink<spdlog::details::null_mutex>;
	using test_sink_mt = test_sink<std::mutex>;
}
