#pragma once

#include <kad/model/shared/common.hpp>

#include <string>
#include <vector>

namespace kad::model::query
{
	struct Query
	{
		struct Request
		{
			// Specifies one of the Object Kinds to be included in the reply.
			std::string kind;

			// a JSON object containing major and (optionally) minor members specifying non-negative
			// integer version components
			Version version;

			// Optional member reserved for use by the client. This value is preserved in the reply written
			// for the client in the v1 Reply Index File but is otherwise ignored. Clients may use this to
			// pass custom information with a request through to its reply.
			std::string client;

			constexpr bool operator==(const Request&) const = default;
		};

		// A JSON array containing zero or more requests.
		std::vector<Request> requests;

		// Optional member reserved for use by the client. This value is preserved in the reply written for
		// the client in the v1 Reply Index File but is otherwise ignored. Clients may use this to pass
		// custom information with a query through to its reply
		std::string client;

		constexpr bool operator==(const Query&) const = default;
	};
}
