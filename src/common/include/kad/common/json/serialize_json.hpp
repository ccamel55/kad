#pragma once

#include <kad/common/json/recursive_filter.hpp>

#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>

#define JSON_SERIALIZE_ENUM(EnumType)															\
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

#define JSON_SERIALIZE_STRUCT(StructType)																					\
	template <>																												\
	struct nlohmann::adl_serializer<StructType>																				\
	{																														\
		using Type = StructType;																							\
		inline const static kad::common::json::RecursiveFilter Filter = kad::common::json::detail::CreateFilter<Type>();	\
																															\
		static void from_json(const json& json, Type& object)																\
		{																													\
			visit_struct::visit_pointers<Type>([&]<typename FieldType>(const char* name, FieldType Type::* ptr)				\
			{																												\
				object.*ptr = json[name].get<FieldType>();																	\
			});																												\
		}																													\
																															\
		static void to_json(json& json, const Type& object)																	\
		{																													\
			visit_struct::visit_pointers<Type>([&]<typename FieldType>(const char* name, FieldType Type::* ptr)				\
			{																												\
				json[name] = object.*ptr;																					\
			});																												\
		}																													\
	};
