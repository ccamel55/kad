#pragma once

#include <utility>

namespace kad::common
{
	template <typename Type>
	class Tracked
	{
	public:
		constexpr Tracked() = default;
		constexpr Tracked(Type type)
			: value_{ std::move(type) }
		{ }

		Tracked& operator=(const Type& other)
		{
			SetDirty();
			value_ = other;
			return *this;
		}

		Tracked& operator=(Type&& other)
		{
			SetDirty();
			value_ = std::move(other);
			return *this;
		}

		constexpr void SetDirty() { dirty_ = true; }
		constexpr void SetClean() { dirty_ = false; }

		constexpr const Type* operator->() const { return get(); }
		constexpr const Type& operator*() const { return value(); }

		[[nodiscard]] Type& mut() { SetDirty(); return value_; }

		[[nodiscard]] constexpr const Type* get() const { return std::addressof(value_); }
		[[nodiscard]] constexpr const Type& value() const { return value_; }

		[[nodiscard]] constexpr bool dirty() const { return dirty_; }

	private:
		Type value_{ };
		bool dirty_{ false };

	};
}
