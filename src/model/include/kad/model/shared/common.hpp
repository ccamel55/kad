#pragma once

#include <cstdint>
#include <filesystem>

namespace kad::model
{
	struct Path
	{
		struct Name
		{
			static constexpr auto SOURCE = "source";
			static constexpr auto BUILD  = "build";
		};

		// A string specifying the absolute path to the top-level source directory, represented with forward slashes.
		std::filesystem::path source;

		// A string specifying the absolute path to the top-level build directory, represented with forward slashes.
		std::filesystem::path build;
	};

	struct Version
	{
		struct Name
		{
			static constexpr auto MAJOR = "major";
			static constexpr auto MINOR = "minor";
		};

		uint32_t major;
		uint32_t minor;
	};
}
