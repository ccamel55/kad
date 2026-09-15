#pragma once

#include <kad/bin/cli/base.hpp>

#include <filesystem>

namespace kad::cli
{
	class CommandInit final : public CommandBase
	{
	public:
		struct State
		{
			std::filesystem::path root;
		};

		explicit CommandInit(CLI::App* parent);

	private:
		void HandleCommand();

	private:
		State state_;

	};
}
