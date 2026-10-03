#pragma once

#include <kad/common/json/serialize_json.hpp>

#include <kad/model/shared/common.hpp>
#include <kad/model/shared/common_json.hpp>

VISITABLE_STRUCT(
	kad::model::Path,
	source,
	build
);

VISITABLE_STRUCT(
	kad::model::Version,
	major,
	minor
);

VISITABLE_STRUCT(
	kad::model::ReplyReference,
	kind,
	version,
	jsonFile
);

VISITABLE_STRUCT(
	kad::model::Error,
	error
);

JSON_SERIALIZE_STRUCT(kad::model::Path);
JSON_SERIALIZE_STRUCT(kad::model::Version);
JSON_SERIALIZE_STRUCT(kad::model::ReplyReference);
JSON_SERIALIZE_STRUCT(kad::model::Error);

namespace simdjson
{
	template <typename Json, typename Type>
	auto tag_invoke(serialize_tag, Json& obj, const kad::model::MaybeError<Type>& value)
	{
		value.has_value()
			? obj.append(value.value())
			: obj.append(value.error());
	}

	template <typename Json, typename Type>
	auto tag_invoke(deserialize_tag, Json& obj, kad::model::MaybeError<Type>& value)
	{
		simdjson::ondemand::json_type type;
		if (auto err = obj.type().get(type); err)
		{
			return err;
		}

		if (type == simdjson::ondemand::json_type::object)
		{
			// Note: we can't use the automatically generated implementation because we need to reset the object
			// after it's use in case it fails to parse sucesfully.
			simdjson::ondemand::object object;
			auto err = obj.get_object().get(object);

			kad::model::Error error;
			visit_struct::visit_pointers<kad::model::Error>([&](const char* name, auto kad::model::Error::* ptr)
			{
				if (err)
				{
					return;
				}
				err = object[name].get(error.*ptr);
			});

			if (!err)
			{
				value = std::unexpected{ std::move(error) };
				return SUCCESS;
			}

			// Reset iterator so we can parse the object again.
			object.reset();
		}

		Type value_type;
		if (auto err = obj.get(value_type); err)
		{
			return err;
		}

		value = std::move(value_type);
		return SUCCESS;
	}
};
