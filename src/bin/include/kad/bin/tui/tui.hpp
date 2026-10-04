#pragma once

#include <kad/bin/cli/base.hpp>

#include <kad/lib/target.hpp>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

namespace kad::tui
{
	class Tui
	{
	public:
		explicit Tui(const lib::Targets& targets);
		void RunBlocking();

	private:
		const lib::Targets& targets_;

	};

}
