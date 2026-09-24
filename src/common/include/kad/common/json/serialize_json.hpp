#pragma once

#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>

#define JSON_SERIALIZE_ENUM(EnumType)																		\
	template <typename JsonType>																\
	void to_json(JsonType& json, const EnumType value)											\
	{																							\
		json = ::magic_enum::enum_name(value);													\
	}																							\
	template <typename JsonType>																\
	void from_json(const JsonType& json, EnumType& value)										\
	{																							\
		value = ::magic_enum::enum_cast<EnumType>(json.template get_ref<const std::string&>());	\
	}
