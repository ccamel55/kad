#pragma once

#include <nlohmann/json.hpp>
#include <kad/model/shared/common.hpp>

#include <map>
#include <string>
#include <variant>
#include <vector>

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

	// Stands in for a ReplyReference when CMake cannot satisfy a query (unknown query file, bad or
	// unsupported request, or a kind that an error index does not provide).
	struct ReplyError
	{
		struct Name
		{
			// Deliberately not ERROR, which <windows.h> defines as a macro.
			static constexpr auto ERROR = "error";
		};

		// Human-readable message, e.g. "unknown query file".
		std::string error;
	};

	// Most values under "reply" can be either one; an "error" member means it is an Error.
	using ReferenceOrError = std::variant<ReplyReference, ReplyError>;

	// NOTE: this is missing some fields that we don't really care about
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
				static constexpr auto GENERATOR	= "generator";
			};

			struct Version
			{
				struct Name
				{
					static constexpr auto MAJOR		= "major";
					static constexpr auto MINOR		= "minor";
					static constexpr auto PATCH		= "patch";
					static constexpr auto SUFFIX	= "suffix";
					static constexpr auto STRING	= "string";
					static constexpr auto IS_DIRTY	= "isDirty";
				};

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
			};

			struct Paths
			{
				struct Name
				{
					static constexpr auto CMAKE	= "cmake";
					static constexpr auto CTEST	= "ctest";
					static constexpr auto CPACK	= "cpack";
					static constexpr auto ROOT	= "root";
				};

				std::string cmake;
				std::string ctest;
				std::string cpack;

				// CMake's resource directory, the one containing Modules/ (i.e. CMAKE_ROOT).
				std::string root;
			};

			struct Generator
			{
				struct Name
				{
					static constexpr auto MULTI_CONFIG	= "multiConfig";
					static constexpr auto NAME			= "name";
					static constexpr auto PLATFORM		= "platform";
				};

				// True for multi-configuration generators such as Visual Studio, Xcode and Ninja Multi-Config.
				bool multiConfig = false;

				// Generator name, e.g. "Ninja" or "Unix Makefiles".
				std::string name;

				// Only written by generators that support CMAKE_GENERATOR_PLATFORM.
				std::optional<std::string> platform;
			};

			// CMake's own version, which has more fields than the object-kind Version.
			Version version;

			// Absolute paths, with forward slashes, to the tools and data installed with CMake.
			Paths paths;

			// The generator that produced the build system.
			Generator generator;
		};

		struct Reply
		{
			struct Name
			{
				// Members whose names start with this describe a query/client-<client>/ directory; every
				// other member describes a shared stateless query file.
				static constexpr auto CLIENT_PREFIX	= "client-";
			};

			struct QueryJson
			{
				struct Name
				{
					static constexpr auto CLIENT	= "client";
					static constexpr auto REQUESTS	= "requests";
					static constexpr auto RESPONSES	= "responses";
				};

				// Echo of Query::client; null if the query had none.
				nlohmann::json client;

				// Echo of the raw Query::requests value; null if the query had none. Kept as JSON because
				// CMake copies it back even when it is malformed.
				nlohmann::json requests;

				// An Error if requests was missing or invalid. Otherwise one entry per request, in the same
				// order as Query::requests.
				std::variant<std::vector<ReferenceOrError>, ReplyError> responses;
			};

			struct Client
			{
				struct Name
				{
					static constexpr auto QUERY_JSON = "query.json";
				};

				// The client's stateless query files, keyed by file name. A recognized "<kind>-v<major>" maps
				// to a FileReference (or to an Error in an error index); an unrecognized name maps to an Error.
				std::map<std::string, ReferenceOrError> stateless_queries;

				// Present only if the client has a query.json. An Error if that file could not be read or
				// did not parse as a JSON object.
				std::optional<std::variant<QueryJson, ReplyError>> query_json;
			};

			// The shared stateless query files, keyed by file name. A recognized "<kind>-v<major>" maps to
			// a FileReference (or to an Error in an error index); an unrecognized name maps to an Error.
			std::map<std::string, ReferenceOrError> stateless_queries;

			// One entry per query/client-<client>/ directory, keyed by <client>, i.e. the member name
			// without CLIENT_PREFIX.
			std::map<std::string, Client> clients;
		};

		// Version, install paths and generator of the CMake that wrote this reply.
		CMake cmake;

		// References to all object files CMake produced for this reply, across every kind and version.
		std::vector<ReplyReference> objects;

		// Per-query results, organized the way CMake found the query/ directory: one entry per query
		// file or client directory.
		Reply reply;
	};
}
