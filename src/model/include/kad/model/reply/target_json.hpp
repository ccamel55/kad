#pragma once

#include <kad/common/json/serialize_json.hpp>
#include <kad/model/reply/target.hpp>

#include <nlohmann/json.hpp>

JSON_SERIALIZE_ENUM(kad::model::reply::TargetType);
