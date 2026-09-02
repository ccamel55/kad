#pragma once

#include <kad/model/shared/common.hpp>

namespace kad::model::reply
{
	struct ReplyReference
	{
		struct Name
		{
			static constexpr auto KIND		= "kind";
			static constexpr auto VERSION	= "version";
			static constexpr auto JSON_FILE = "jsonFile";
		};

		// A string specifying one of the Object Kinds.
		std::string kind;

		// A JSON object with members major and minor specifying integer version components of the object kind.
		Version version;

		// A JSON string specifying a path relative to the reply index file to another JSON file containing the object.
		std::filesystem::path json_file;
	};

	// Index files are for success and also errors
	// Sucess files will be named `index-{}.json`
	// Error files will be named `error-{}.json`
	struct Index
	{
		struct Name
		{
			static constexpr auto CMAKE		= "cmake";
			static constexpr auto OBJECTS	= "objects";
			static constexpr auto REPLY		= "reply";
		};

		struct CMake
		{
			struct Name
			{
				static constexpr auto VERSION	= "version";
				static constexpr auto PATHS		= "paths";
				static constexpr auto GENERATOR = "generator";
			};

			struct Version
			{
				struct Name
				{
					static constexpr auto MAJOR 	= "major";
					static constexpr auto MINOR		= "minor";
					static constexpr auto PATCH		= "patch";
					static constexpr auto SUFFIX	= "suffix";
					static constexpr auto STRING	= "string";
					static constexpr auto IS_DIRTY	= "isDirty";
				};
			};

			struct Generator
			{
				struct Name
				{
					static constexpr auto MULTI_CONFIG	= "multiConfig";
					static constexpr auto NAME			= "name";
					static constexpr auto PLATFORM		= "platform";
				};
			};
		};
	};

}
