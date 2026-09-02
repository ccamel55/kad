#pragma once

#include <cstddef>
#include <type_traits>

namespace kad::common
{
	//
	// Checks if a given type is an instance of another template type.
	//

	template<typename Type, template<typename...> typename TemplateType>
	inline constexpr bool is_instance_of_v = std::false_type{ };

	template<template<typename...> typename TemplateType, typename... ArgsT>
	inline constexpr bool is_instance_of_v<TemplateType<ArgsT...>, TemplateType> = std::true_type{ };

	//
	// Can container resize
	//

	template <typename Type>
	concept CanResize = requires(Type& t)
	{
		{ t.resize(size_t{}) };
		{ t.clear() };
	};
}
