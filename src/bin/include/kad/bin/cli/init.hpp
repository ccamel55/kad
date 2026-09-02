#pragma once

#include <kad/bin/cli/base.hpp>

namespace kad::cli
{
	class CommandInit final
		: public CommandBase
	{
	public:
		explicit CommandInit(CLI::App* parent);

	private:
		void HandleCommand();

	};
}
