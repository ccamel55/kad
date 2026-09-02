#pragma once

#include <kad/common/no_copy_or_move.hpp>

#include <functional>

namespace kad::common
{
	/// Invokes a given function on scope exit.
	class ScopeExit : NoCopyOrMove
	{
	public:
		explicit ScopeExit(std::function<void()>&& fn)
			: m_on_exit{std::move(fn)}
		{ }

		~ScopeExit()
		{
			m_on_exit();
		}

	private:
		std::function<void()> m_on_exit;

	};
}
