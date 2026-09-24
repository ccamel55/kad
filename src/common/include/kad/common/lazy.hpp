#pragma once

#include <kad/common/release_assert.hpp>

#include <functional>
#include <optional>

namespace kad::common
{
	template <typename Type>
	class Lazy
	{
	public:
		constexpr Lazy(Type init)
			: init_{ nullptr }
			, value_{ std::make_optional(std::move(init)) }
		{ }

		constexpr Lazy(std::function<void(std::optional<Type>&)> init)
			: init_{ std::move(init) }
			, value_{ std::nullopt }
		{ }

		constexpr Type* operator->() { TryInit(); return  get(); }
		constexpr const Type* operator->() const { TryInit(); return get(); }

		constexpr Type& operator*() { TryInit(); return value(); }
		constexpr const Type& operator*() const { TryInit(); return value(); }

		[[nodiscard]] constexpr Type* get() { TryInit(); return std::addressof(value_.value()); }
		[[nodiscard]] constexpr const Type* get() const { TryInit(); return std::addressof(value_.value()); }

		[[nodiscard]] constexpr Type& value() { TryInit(); return value_.value(); }
		[[nodiscard]] constexpr const Type& value() const { TryInit(); return value_.value(); }

		[[nodiscard]] constexpr bool has_value() const { return value_.has_value(); }

		constexpr explicit operator bool() const { return value_.has_value(); }

	private:
		constexpr void TryInit() const
		{
			if (value_.has_value()) [[likely]]
			{
				return;
			}

			release_assert(init_, "init function must exist if not initializing from value");
			init_(value_);
		}

	private:
		mutable std::function<void(std::optional<Type>&)>	init_{ nullptr };
		mutable std::optional<Type>							value_;

	};
}
