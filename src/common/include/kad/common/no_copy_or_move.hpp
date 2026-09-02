#pragma once

namespace kad::common
{
	/// Makes inherited class non-copyable or movable
	class NoCopyOrMove
	{
	public:
		NoCopyOrMove() = default;

		NoCopyOrMove(const NoCopyOrMove&) = delete;
		NoCopyOrMove(NoCopyOrMove&&) = delete;

		NoCopyOrMove& operator=(const NoCopyOrMove&) = delete;
		NoCopyOrMove& operator=(NoCopyOrMove&&) = delete;

	};

	class NoCopy
	{
	public:
		NoCopy() = default;

		NoCopy(const NoCopyOrMove&) = delete;
		NoCopy& operator=(const NoCopyOrMove&) = delete;

	};
}
