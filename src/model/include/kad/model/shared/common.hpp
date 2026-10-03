#pragma once

#include <cstdint>
#include <expected>
#include <filesystem>

namespace kad::model
{
	struct Path
	{
		// A string specifying the absolute path to the top-level source directory, represented with forward slashes.
		std::filesystem::path source;

		// A string specifying the absolute path to the top-level build directory, represented with forward slashes.
		std::filesystem::path build;

		constexpr bool operator==(const Path&) const = default;
	};

	struct Version
	{
		uint32_t major;
		uint32_t minor;

		constexpr bool operator==(const Version&) const = default;
	};

	struct ReplyReference
	{
		// A string specifying one of the Object Kinds.
		std::string kind;

		// A JSON object with members major and minor specifying integer version components of the object kind.
		Version version;

		// A JSON string specifying a path relative to the reply index file to another JSON file containing the object.
		std::filesystem::path jsonFile;

		constexpr bool operator==(const ReplyReference&) const = default;
	};

	struct Error
	{
		std::string error{ };

		constexpr bool operator==(const Error&) const = default;
	};

	template <typename Type>
	class MaybeError : public std::expected<Type, Error>
	{
	public:
		using Base = std::expected<Type, Error>;

		constexpr MaybeError()
			: Base{ std::unexpected{ Error{ .error = "" } } }
		{ }

		constexpr MaybeError(const Type& type)
			: Base{ type }
		{ }

		constexpr MaybeError(Type&& type)
			: Base{ std::move(type) }
		{ }

		constexpr MaybeError(const std::unexpected<Error>& error)
			: Base{ error }
		{ }

		constexpr MaybeError(std::unexpected<Error>&& error)
			: Base{ std::move(error) }
		{ }

		[[nodiscard]] const Base& base() const { return *this; }

		constexpr bool operator==(const MaybeError&) const = default;
	};
}
