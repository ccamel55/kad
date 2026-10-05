#pragma once

#include <cstddef>
#include <flat_map>
#include <flat_set>
#include <type_traits>

namespace kad::common
{
	//
	// Checks if a given type is an instance of another template type.
	//

	template <typename Type, template<typename...> typename TemplateType>
	inline constexpr bool is_instance_of_v = std::false_type{ };

	template <template<typename...> typename TemplateType, typename... ArgsT>
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

	template <typename...Args>
	std::flat_map<Args...> FlatMapReserved(std::size_t new_cap)
	{
		using FlatMap =  std::flat_map<Args...>;

		using KeyContainer		= typename FlatMap::key_container_type;
		using ValueContainer	= typename FlatMap::mapped_container_type;

		KeyContainer keys;
		if constexpr(requires { keys.reserve(new_cap); })
		{
			keys.reserve(new_cap);
		}

		ValueContainer values;
		if constexpr(requires { values.reserve(new_cap); })
		{
			values.reserve(new_cap);
		}

		FlatMap map;
		map.replace(std::move(keys), std::move(values));

		return map;
	};

	template <typename...Args>
	std::flat_set<Args...> FlatSetReserved(std::size_t new_cap)
	{
		using FlatSet	=  std::flat_set<Args...>;
		using Container	= typename FlatSet::container_type;

		Container values;
		if constexpr(requires { values.reserve(new_cap); })
		{
			values.reserve(new_cap);
		}

		FlatSet set;
		set.replace(std::move(values));

		return set;
	};
}
