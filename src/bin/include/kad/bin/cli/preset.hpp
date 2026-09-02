#pragma once

#include <kad/bin/cli/base.hpp>

namespace kad::cli
{
	class CommandPreset final
		: public CommandBase
	{
	public:
		struct State
		{
			std::string preset;
		};

		explicit CommandPreset(CLI::App* parent);

	private:
		void HandleCommand();

	private:
		State state_;
		std::vector<std::unique_ptr<CommandBase>> commands_;
	};
}
