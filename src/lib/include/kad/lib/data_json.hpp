#pragma once

#include <kad/common/json/serialize_json.hpp>
#include <kad/lib/data.hpp>

VISITABLE_STRUCT(
	kad::lib::config::Config,
	revision,
	active_preset
);

VISITABLE_STRUCT(
	kad::lib::config::Preset,
	revision,
	build_directory
);

VISITABLE_STRUCT(
	kad::lib::config::Target,
	revision,
	environment_variables,
	arguments,
	working_directory
);

JSON_SERIALIZE_STRUCT(kad::lib::config::Config);
JSON_SERIALIZE_STRUCT(kad::lib::config::Preset);
JSON_SERIALIZE_STRUCT(kad::lib::config::Target);
