#pragma once

#include <kad/model/query/query.hpp>
#include <kad/model/shared/common_json.hpp>

VISITABLE_STRUCT(
	kad::model::query::Query::Request,
	kind,
	version,
	client
);

VISITABLE_STRUCT(
	kad::model::query::Query,
	requests,
	client
);

JSON_SERIALIZE_STRUCT(kad::model::query::Query);
JSON_SERIALIZE_STRUCT(kad::model::query::Query::Request);
