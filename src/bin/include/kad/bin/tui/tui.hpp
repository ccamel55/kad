#pragma once

#include <kad/bin/target.hpp>
#include <kad/bin/cli/base.hpp>

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

#include <vector>

namespace kad::tui
{
	struct TuiData
	{
		std::vector<Target> targets;
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
