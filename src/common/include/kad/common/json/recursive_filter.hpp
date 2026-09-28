#pragma once

#include <kad/common/no_copy_or_move.hpp>

#include <nlohmann/json.hpp>
#include <visit_struct/visit_struct.hpp>

#include <map>
#include <print>

namespace kad::common::json
{
	/// Recursively owning filter.
	/// Each instance can hold references to other static instances of recursive filter.
	struct RecursiveFilter : public kad::common::NoCopy
	{
		using Filter = std::map<std::string_view, const RecursiveFilter*>;
		Filter data;

		void Log(int depth = 0) const
		{
			const auto indent = std::format("{:\t>{}}", "", depth);
			for (const auto& [k, v]: data)
			{
				std::println("{}{}: {}", indent, k, v ? reinterpret_cast<uintptr_t>(v) : 0);
				if (v && !v->data.empty())
				{
					v->Log(depth + 1);
				}
			}
		}
	};

	namespace detail
	{
		template <typename Type>
		concept VisitableType = visit_struct::traits::visitable<Type>::value;

		template <typename Type>
		concept SerializeWithFilterType = requires { { nlohmann::adl_serializer<Type>::Filter } -> std::same_as<const RecursiveFilter&>; };

		/// Build a static instance of object filter.
		/// This should only be called to construct the filter, getting filters should be done via `GetFilter`.
		template <VisitableType Type>
		RecursiveFilter CreateFilter()
		{
			RecursiveFilter filter;
			visit_struct::visit_types<Type>([&]<typename FieldType>(const char* name, visit_struct::type_c<FieldType>)
			{
				if constexpr (VisitableType<FieldType>)
				{
					static_assert(SerializeWithFilterType<FieldType>, "custom type must have filter");
					filter.data.emplace(name, std::addressof(nlohmann::adl_serializer<FieldType>::Filter));
				}
				else
				{
					filter.data.emplace(name, nullptr);
				}
			});
			return filter;
		}
	}

	/// Get reference to static filter object.
	template <detail::SerializeWithFilterType Type>
	const RecursiveFilter& GetFilter()
	{
		return nlohmann::adl_serializer<Type>::Filter;
	}
}
