#pragma once

#include <kad/common/json/serialize_json.hpp>
#include <kad/model/reply/index.hpp>

VISITABLE_STRUCT(
	kad::model::reply::Index::CMake::Version,
	major,
	minor,
	patch,
	suffix,
	string,
	isDirty
);

VISITABLE_STRUCT(
	kad::model::reply::Index::CMake::Paths,
	cmake,
	ctest,
	cpack,
	root
);

VISITABLE_STRUCT(
	kad::model::reply::Index::CMake::Generator,
	multiConfig,
	name,
	platform
);

VISITABLE_STRUCT(
	kad::model::reply::Index::CMake,
	version,
	paths,
	generator
);

VISITABLE_STRUCT(
	kad::model::reply::Index::Reply::Client::QueryJson,
	client,
	responses
);

VISITABLE_STRUCT(
	kad::model::reply::Index,
	cmake,
	reply
);

JSON_SERIALIZE_STRUCT(kad::model::reply::Index::CMake::Version);
JSON_SERIALIZE_STRUCT(kad::model::reply::Index::CMake::Paths);
JSON_SERIALIZE_STRUCT(kad::model::reply::Index::CMake::Generator);
JSON_SERIALIZE_STRUCT(kad::model::reply::Index::CMake);
JSON_SERIALIZE_STRUCT(kad::model::reply::Index::Reply::Client::QueryJson);

namespace simdjson
{
	template <typename Json>
	auto tag_invoke(serialize_tag, Json& obj, const kad::model::reply::Index::Reply::Client& value)
	{
		constexpr auto& name = kad::model::reply::Index::Reply::Client::Name::QUERY_JSON;

		obj.start_object();
		obj.append_key_value(name, value.queryJson);
		obj.end_object();
	}

	template <typename Json>
	auto tag_invoke(deserialize_tag, Json& obj, kad::model::reply::Index::Reply::Client& value)
	{
		constexpr auto& name = kad::model::reply::Index::Reply::Client::Name::QUERY_JSON;
		ondemand::object object;
		if (auto err = obj.get_object().get(object); err)
		{
			return err;
		}
		if (auto err = object[name].get(value.queryJson); err)
		{
			return err;
		}
		return SUCCESS;
	}
};

namespace simdjson
{
	template <typename Json>
	auto tag_invoke(serialize_tag, Json& obj, const kad::model::reply::Index::Reply& value)
	{
		obj.append(value.clients);
	}

	template <typename Json>
	auto tag_invoke(deserialize_tag, Json& obj, kad::model::reply::Index::Reply& value)
	{
		if (auto err = obj.get_object().get(value.clients); err)
		{
			return err;
		}
		return SUCCESS;
	}
};

JSON_SERIALIZE_STRUCT(kad::model::reply::Index);
