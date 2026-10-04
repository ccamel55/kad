#pragma once

#include <kad/model/shared/common.hpp>

#include <filesystem>
#include <vector>

namespace kad::model::reply
{
	enum class TargetType
	{
		EXECUTABLE,
		STATIC_LIBRARY,
		SHARED_LIBRARY,
		MODULE_LIBRARY,
		OBJECT_LIBRARY,
		INTERFACE_LIBRARY,
		UTILITY,
	};

	struct Target
	{
		struct Paths
		{
			// The target's source directory: relative to the top-level source directory when inside it
			// ("." for the top-level directory itself), absolute otherwise.
			std::filesystem::path source;

			// The target's build directory: relative to the top-level build directory when inside it
			// ("." for the top-level directory itself), absolute otherwise.
			std::filesystem::path build;

			constexpr bool operator==(const Paths&) const = default;
		};

		struct Artifact
		{
			// Relative to the top-level build directory when inside it, absolute otherwise.
			std::filesystem::path path;

			constexpr bool operator==(const Artifact&) const = default;
		};

		// Added in codemodel version 2.8.
		struct Debugger
		{
			// Present when DEBUGGER_WORKING_DIRECTORY (or VS_DEBUGGER_WORKING_DIRECTORY with Visual Studio
			// generators) is set.
			std::filesystem::path workingDirectory;

			constexpr bool operator==(const Debugger&) const = default;
		};

		// The logical name of the target.
		std::string name;

		// Uniquely identifies the target. The format is unspecified and must not be interpreted.
		std::string id;

		TargetType type;

		Paths paths;

		// File name of the primary artifact for executables and libraries that are linked or archived
		// into a single primary artifact.
		std::optional<std::string> nameOnDisk;

		// Artifacts meant for consumption by dependents, for executables and libraries.
		std::optional<std::vector<Artifact>> artifacts;

		// Present when the target has any debugger specific values set.
		std::optional<Debugger> debugger;

		constexpr bool operator==(const Target&) const = default;
	};

}
