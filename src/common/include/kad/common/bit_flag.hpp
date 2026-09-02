#pragma once

#include <initializer_list>
#include <type_traits>

namespace kad::common
{
	/// Bit flag helper with scoped enums.
	template <typename EnumT>
		requires (std::is_enum_v<EnumT>)
	class BitFlag
	{
		using UnderlyingT = std::underlying_type_t<EnumT>;

	public:
		constexpr BitFlag()
			: m_underlying{ static_cast<UnderlyingT>(0) }
		{ }

		explicit constexpr BitFlag(const EnumT e)
			: m_underlying{ static_cast<UnderlyingT>(e) }
		{ }

		explicit constexpr BitFlag(const UnderlyingT u)
			: m_underlying{ u }
		{ }

		constexpr BitFlag(const std::initializer_list<EnumT> e_list)
			: m_underlying{ static_cast<UnderlyingT>(0) }
		{
			for (const auto e: e_list)
			{
				m_underlying |= static_cast<UnderlyingT>(e);
			}
		}

		//
		// Accessors
		//

		[[nodiscard]] constexpr bool any() const
		{
			return m_underlying != 0;
		}

		[[nodiscard]] constexpr bool is_set(const EnumT e) const
		{
			return (m_underlying & static_cast<UnderlyingT>(e)) == static_cast<UnderlyingT>(e);
		}

		constexpr void set(const EnumT e)
		{
		   m_underlying |= static_cast<UnderlyingT>(e);
		}

		constexpr void unset(const EnumT e)
		{
			m_underlying &= ~static_cast<UnderlyingT>(e);
		}

		constexpr void clear()
		{
			m_underlying = static_cast<UnderlyingT>(0);
		}

		//
		// Operators
		//

		friend constexpr BitFlag operator|(const BitFlag lhs, const EnumT rhs)
		{
			return { lhs.m_underlying | static_cast<UnderlyingT>(rhs) };
		}

		friend constexpr BitFlag operator|(const BitFlag lhs, const BitFlag rhs)
		{
			return { lhs.m_underlying | rhs.m_underlying };
		}

		friend constexpr BitFlag operator&(const BitFlag lhs, const EnumT rhs)
		{
			return { lhs.m_underlying & static_cast<UnderlyingT>(rhs) };
		}

		friend constexpr BitFlag operator&(const BitFlag lhs, const BitFlag rhs)
		{
			return { lhs.m_underlying & rhs.m_underlying };
		}

		friend constexpr BitFlag operator^(const BitFlag lhs, const EnumT rhs)
		{
			return { lhs.m_underlying ^ static_cast<UnderlyingT>(rhs) };
		}

		friend constexpr BitFlag operator^(const BitFlag lhs, const BitFlag rhs)
		{
			return { lhs.m_underlying ^ rhs.m_underlying };
		}

		friend constexpr BitFlag& operator|=(BitFlag& lhs, const EnumT rhs)
		{
			lhs.m_underlying |= static_cast<UnderlyingT>(rhs);
			return lhs;
		}

		friend constexpr BitFlag& operator|=(BitFlag& lhs, const BitFlag rhs)
		{
			lhs.m_underlying |= rhs.m_underlying;
			return lhs;
		}

		friend constexpr BitFlag& operator&=(BitFlag& lhs, const EnumT rhs)
		{
			lhs.m_underlying &= static_cast<UnderlyingT>(rhs);
			return lhs;
		}

		friend constexpr BitFlag& operator&=(BitFlag& lhs, const BitFlag rhs)
		{
			lhs.m_underlying &= rhs.m_underlying;
			return lhs;
		}

		friend constexpr BitFlag& operator^=(BitFlag& lhs, const EnumT rhs)
		{
			lhs.m_underlying ^= ToUnderlying(rhs);
			return lhs;
		}

		friend constexpr BitFlag& operator^=(BitFlag& lhs, const BitFlag rhs)
		{
			lhs.m_underlying ^= rhs.m_underlying;
			return lhs;
		}

		friend constexpr BitFlag operator~(const BitFlag& bf)
		{
			return { ~bf.m_underlying };
		}

		friend constexpr bool operator==(const BitFlag& lhs, const BitFlag& rhs)
		{
			return lhs.m_underlying == rhs.m_underlying;
		}

		friend constexpr bool operator!=(const BitFlag& lhs, const BitFlag& rhs)
		{
			return lhs.m_underlying != rhs.m_underlying;
		}

	private:
		UnderlyingT m_underlying;

	};
}
