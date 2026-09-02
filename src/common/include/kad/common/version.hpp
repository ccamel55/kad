#pragma once

#include <array>
#include <cstdint>

namespace kad::common
{
	/// Version wrapper
	class Version
	{
	public:
		constexpr Version(const uint16_t major, const uint16_t minor, const uint16_t patch)
			: m_version{ major, minor, patch }
		{ }

		[[nodiscard]] uint16_t major() const { return m_version[0]; }
		[[nodiscard]] uint16_t minor() const { return m_version[1]; }
		[[nodiscard]] uint16_t patch() const { return m_version[2]; }

		bool operator==(const Version& other) const
		{
			return this->m_version == other.m_version;
		}

		/// Check if given version has a breaking change
		[[nodiscard]] bool breaks(const Version& other) const
		{
			return this->m_version[0] != other.m_version[0];
		}

		[[nodiscard]] std::array<uint16_t, 3>& version()
		{
			return m_version;
		}

	private:
		std::array<uint16_t, 3> m_version;

	};

	static_assert(sizeof(Version) == 6);
	static_assert(alignof(Version) == 2);

	static_assert(std::is_trivially_copyable_v<Version>);
}
