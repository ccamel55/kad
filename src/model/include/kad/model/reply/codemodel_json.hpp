#pragma once

#include <kad/model/shared/common_json.hpp>
#include <kad/model/reply/codemodel.hpp>

VISITABLE_STRUCT(
	kad::model::reply::CodeModel::Configurations::Target,
	name,
	id,
	directoryIndex,
	projectIndex,
	jsonFile
);

VISITABLE_STRUCT(
	kad::model::reply::CodeModel::Configurations,
	name,
	targets,
	abstractTargets
);

VISITABLE_STRUCT(
	kad::model::reply::CodeModel,
	paths,
	version,
	configurations
);

JSON_SERIALIZE_STRUCT(kad::model::reply::CodeModel::Configurations::Target);
JSON_SERIALIZE_STRUCT(kad::model::reply::CodeModel::Configurations);
JSON_SERIALIZE_STRUCT(kad::model::reply::CodeModel);
