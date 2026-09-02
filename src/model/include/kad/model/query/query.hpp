#pragma once

#include <kad/model/shared/common.hpp>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace kad::model::query
{
	struct Query
	{
		struct Name
		{
			static constexpr auto CLIENT	= "client";
			static constexpr auto REQUESTS	= "requests";
		};

		struct Request
		{
			struct Name
			{
				static constexpr auto KIND		= "kind";
				static constexpr auto VERSION	= "version";
				static constexpr auto CLIENT	= "client";
			};

			// Specifies one of the Object Kinds to be included in the reply.
			std::string kind;

			// a JSON object containing major and (optionally) minor members specifying non-negative
			// integer version components
			Version version;

			// Optional member reserved for use by the client. This value is preserved in the reply written
			// for the client in the v1 Reply Index File but is otherwise ignored. Clients may use this to
			// pass custom information with a request through to its reply.
			nlohmann::json client;
		};

		// A JSON array containing zero or more requests.
		std::vector<Request> requests;

		// Optional member reserved for use by the client. This value is preserved in the reply written for
		// the client in the v1 Reply Index File but is otherwise ignored. Clients may use this to pass
		// custom information with a query through to its reply
		nlohmann::json client;
	};
}
