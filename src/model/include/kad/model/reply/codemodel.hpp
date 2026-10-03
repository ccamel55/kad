#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <kad/model/shared/common.hpp>

namespace kad::model::reply
{
	// NOTE: this is missing some fields that we don't really care about
	struct CodeModel
	{
		struct Configurations
		{
			struct Target
			{
				// A string specifying the target name.
				std::string name;

				// A string uniquely identifying the target. This matches the id field in the file referenced by jsonFile.
				std::string id;

				// An unsigned integer 0-based index into the main directories array indicating the build system directory
				// in which the target is defined.
				uint32_t directoryIndex;

				// An unsigned integer 0-based index into the main projects array indicating the build system project
				// in which the target is defined.
				uint32_t projectIndex;

				// A JSON string specifying a path relative to the codemodel file to another JSON file containing a
				// "codemodel" version 2 "target" object.
				std::filesystem::path jsonFile;

				constexpr bool operator==(const Target&) const = default;
			};

			// A string specifying the name of the configuration.
			std::string name;

			// A JSON array of entries corresponding to the build system targets. Such targets are created by calls
			// to add_executable(), add_library(), and add_custom_target(), excluding imported targets and interface
			// libraries that do not generate any build rules.
			std::vector<Target> targets;

			// Only exists for V2.9 and later
			// A JSON array of entries corresponding to targets that are not present in the build system.
			// These are imported targets or interface libraries created by calls to add_executable() or add_library().
			// In the case of interface libraries, only those that are not part of the build system are included in this array.
			// Interface libraries that do participate in the build system will be included in the targets array instead.
			std::vector<Target> abstractTargets;

			constexpr bool operator==(const Configurations&) const = default;
		};

		// Will never change.
		inline static constexpr auto kind = "codemodel";

		kad::model::Path paths;
		kad::model::Version version;

		// A JSON array of entries corresponding to available build configurations. On single-configuration generators
		// there is one entry for the value of the CMAKE_BUILD_TYPE variable. For multi-configuration generators there
		// is an entry for each configuration listed in the CMAKE_CONFIGURATION_TYPES variable
		std::vector<Configurations> configurations;

		constexpr bool operator==(const CodeModel&) const = default;
	};
}
