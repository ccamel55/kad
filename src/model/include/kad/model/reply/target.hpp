#pragma once

#include <kad/model/shared/common.hpp>

namespace kad::model::reply
{
	enum class TargetType
	{
		EXECUTABLE,
		STATIC_LIBRARY,
		SHARED_LIBRARY,
		MODULE_LIBRARY,
		OBJECT_LIBRARY,
		INTERFACE_LIBRARY,
		UTILITY,
	};

}
