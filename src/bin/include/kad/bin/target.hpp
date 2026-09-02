#pragma once

#include <string>

namespace kad
{
	enum TargetType
	{
		TARGET,
		ABSTRACT_TARGET
	};

	struct Target
	{
		std::string name;
		TargetType	type;
	};
}
