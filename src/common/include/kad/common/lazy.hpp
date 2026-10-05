#pragma once

#include <kad/common/no_copy_or_move.hpp>
#include <kad/common/release_assert.hpp>

#include <functional>
#include <optional>

namespace kad::common
{
	template <typename Type>
	class Lazy : public common::NoCopy
	{
	public:
		constexpr Lazy(
			std::function<void(std::optional<Type>&)> init,
			std::function<void(Type&)> destory = nullptr
		)
			: init_{ std::move(init) }
			, destroy_{ std::move(destory) }
			, value_{ std::nullopt }
		{ }

		constexpr ~Lazy()
		{
			if (value_.has_value())
			{
				if (destroy_)
				{
					destroy_(value_.value());
				}
			}
		}

		constexpr Lazy(Lazy&& other)
		{
			init_ = std::move(other.init_);
			other.init_ = nullptr;

			destroy_= std::move(other.destroy_);
			other.destroy_ = nullptr;

			value_ = std::move(other.value_);
			other.value_.reset();
		}

		constexpr Lazy& operator=(Lazy&& other)
		{
			init_ = std::move(other.init_);
			other.init_ = nullptr;

			destroy_= std::move(other.destroy_);
			other.destroy_ = nullptr;

			value_ = std::move(other.value_);
			other.value_ = std::nullopt;

			return *this;
		}

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

		constexpr void invalidate()
		{
			if (value_.has_value())
			{
				if (destroy_)
				{
					destroy_(value_.value());
				}
			}

			value_.reset();
		}

	private:
		constexpr void TryInit() const
		{
			if (value_.has_value()) [[likely]]
			{
				return;
			}

			init_(value_);
		}

	private:
		mutable std::function<void(std::optional<Type>&)>	init_{ nullptr };
		mutable std::function<void(Type&)>					destroy_{ nullptr };
		mutable std::optional<Type>							value_{ std::nullopt };

	};
}
