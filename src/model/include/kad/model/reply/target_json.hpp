#pragma once

#include <kad/common/json/serialize_json.hpp>
#include <kad/model/reply/target.hpp>

VISITABLE_STRUCT(
	kad::model::reply::Target::Paths,
	source,
	build
);

VISITABLE_STRUCT(
	kad::model::reply::Target::Artifact,
	path
);

VISITABLE_STRUCT(
	kad::model::reply::Target::Debugger,
	workingDirectory
);

VISITABLE_STRUCT(
	kad::model::reply::Target,
	name,
	id,
	type,
	paths,
	nameOnDisk,
	artifacts,
	debugger
);

JSON_SERIALIZE_ENUM(kad::model::reply::TargetType);

JSON_SERIALIZE_STRUCT(kad::model::reply::Target::Paths);
JSON_SERIALIZE_STRUCT(kad::model::reply::Target::Artifact);
JSON_SERIALIZE_STRUCT(kad::model::reply::Target::Debugger);
JSON_SERIALIZE_STRUCT(kad::model::reply::Target);
