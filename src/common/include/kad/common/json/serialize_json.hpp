#pragma once

#include <filesystem>

#include <simdjson.h>

#include <magic_enum/magic_enum.hpp>
#include <visit_struct/visit_struct.hpp>

#include <kad/common/traits.hpp>

#define JSON_SERIALIZE_ENUM(EnumType)												\
	namespace simdjson																\
	{																				\
		template <typename Json>													\
		auto tag_invoke(serialize_tag, Json& object, const EnumType& value)			\
		{																			\
			object.append(magic_enum::enum_name<EnumType>(value));					\
		}																			\
		template <typename Json>													\
		auto tag_invoke(deserialize_tag, Json& object, EnumType& value)				\
		{																			\
			std::string enum_str;													\
			if (auto err = object.get_string().get(enum_str))						\
			{																		\
				return err;															\
			}																		\
			value = magic_enum::enum_cast<EnumType>(enum_str).value();				\
			return SUCCESS;															\
		}																			\
	}

#define JSON_SERIALIZE_STRUCT(StructType)																					\
	namespace simdjson																										\
	{																														\
		template <typename Json>																							\
		auto tag_invoke(serialize_tag, Json& obj, const StructType& value)													\
		{																													\
			bool first = true;																								\
			obj.start_object();																								\
			visit_struct::visit_pointers<StructType>([&]<typename FieldType>(const char* name, FieldType StructType::* ptr)	\
			{																												\
				if (!first)																									\
				{																											\
					obj.append_comma();																						\
				}																											\
				obj.append_key_value(name, value.*ptr);																		\
				first = false;																								\
			});																												\
			obj.end_object();																								\
		}																													\
		template <typename Json>																							\
		auto tag_invoke(deserialize_tag, Json& obj, StructType& value)														\
		{																													\
			ondemand::object object;																						\
			auto err = obj.get_object().get(object);																		\
			visit_struct::visit_pointers<StructType>([&]<typename FieldType>(const char* name, FieldType StructType::* ptr)	\
			{																												\
				if (err)																									\
				{																											\
					return;																									\
				}																											\
				err = object[name].get(value.*ptr);																			\
				if constexpr (kad::common::is_instance_of_v<FieldType, std::optional>)										\
				{																											\
					if (err == simdjson::error_code::NO_SUCH_FIELD)															\
					{																										\
						err = simdjson::error_code::SUCCESS;																\
						value.*ptr = std::nullopt;																			\
					}																										\
				}																											\
			});																												\
			if (err)																										\
			{																												\
				return err;																									\
			}																												\
			return SUCCESS;																									\
		}																													\
	}

// std::filesystem::path is broken is current version of simdjson (at least for my build chain)
// we need to implement a custom serializer and deserializer to correct it.
namespace simdjson
{
	template <typename Json>
	auto tag_invoke(serialize_tag, Json& object, std::filesystem::path& value)
	{
		return object.append(value.c_str());
	}

	template <typename Json>
	auto tag_invoke(deserialize_tag, Json& object, std::filesystem::path& value)
	{
		std::string value_string;
		if (auto err = object.get(value_string); err)
		{
			return err;
		}
		value = std::move(value_string);
		return SUCCESS;
	}
}
