#pragma once

#include <kad/bin/cli/base.hpp>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

namespace kad::tui
{
	struct TuiData
	{
		std::set<std::string> targets;
	};

	class Tui
	{
	public:
		explicit Tui(const TuiData& data);

		void RunBlocking();

	private:
		TuiData data_;

	};

}
