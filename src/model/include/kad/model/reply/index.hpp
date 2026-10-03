#pragma once

#include <map>
#include <string>
#include <vector>

#include <kad/model/shared/common.hpp>

namespace kad::model::reply
{
	// NOTE: this is not complete and only contains values we care about
	struct Index
	{
		struct CMake
		{
			struct Version
			{
				int major = 0;
				int minor = 0;
				int patch = 0;

				// Set for development builds and release candidates (e.g. "rc1" or "g0abc3"). The member is
				// optional, so treat a missing value like an empty one.
				std::string suffix;

				// The whole version as text: <major>.<minor>.<patch>[-<suffix>].
				std::string string;

				// True if CMake was built from a version-controlled checkout that had local changes.
				bool isDirty = false;

				constexpr bool operator==(const Version&) const = default;
			};

			struct Paths
			{
				std::string cmake;
				std::string ctest;
				std::string cpack;

				// CMake's resource directory, the one containing Modules/ (i.e. CMAKE_ROOT).
				std::string root;

				constexpr bool operator==(const Paths&) const = default;
			};

			struct Generator
			{
				// True for multi-configuration generators such as Visual Studio, Xcode and Ninja Multi-Config.
				bool multiConfig = false;

				// Generator name, e.g. "Ninja" or "Unix Makefiles".
				std::string name;

				// Only written by generators that support CMAKE_GENERATOR_PLATFORM.
				std::optional<std::string> platform;

				constexpr bool operator==(const Generator&) const = default;
			};

			// CMake's own version, which has more fields than the object-kind Version.
			Version version;

			// Absolute paths, with forward slashes, to the tools and data installed with CMake.
			Paths paths;

			// The generator that produced the build system.
			Generator generator;

			constexpr bool operator==(const CMake&) const = default;
		};

		struct Reply
		{
			struct Client
			{
				struct QueryJson
				{
					using ReplyReferenceOrError = MaybeError<ReplyReference>;
					using ReplyReferenceOrErrorArrayOrError = MaybeError<std::vector<ReplyReferenceOrError>>;

					//  A copy of the query.json file client member, if it exists.
					std::string client;

					// Present only if the client has a query.json. An Error if that file could not be read or
					// did not parse as a JSON object.
					ReplyReferenceOrErrorArrayOrError responses{ };

					constexpr bool operator==(const QueryJson&) const = default;
				};

				struct Name
				{
					static constexpr auto QUERY_JSON = "query.json";
				};

				// Present only if the client has a query.json. An Error if that file could not be read or
				// did not parse as a JSON object.
				MaybeError<QueryJson> queryJson;

				constexpr bool operator==(const Client&) const = default;
			};

			// One entry per query/client-<client>/ directory, keyed by <client>, i.e. the member name
			// without CLIENT_PREFIX.
			std::map<std::string, Client> clients;

			constexpr bool operator==(const Reply&) const = default;
		};

		// A JSON object containing information about the instance of CMake that generated the reply.
		CMake cmake;

		// A JSON object mirroring the content of the query/ directory that CMake loaded to produce the reply.
		Reply reply;

		constexpr bool operator==(const Index&) const = default;
	};
}
