#pragma once

#include <kad/bin/cli/base.hpp>
#include <kad/bin/context/context.hpp>

namespace kad::cli
{
	class CommandTarget final : public CommandBase
	{
	public:
		struct State
		{
			std::string preset;
			std::string target;
		};

		explicit CommandTarget(CLI::App* parent);

	private:
		void HandleCommand();

	private:
		std::optional<context::Context> context_;

		State state_;
		std::vector<std::unique_ptr<CommandBase>> commands_;

	};
}
