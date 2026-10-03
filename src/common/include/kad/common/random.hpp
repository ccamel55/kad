#pragma once

#include <random>
#include <string>

namespace kad::common
{
	namespace detail
	{
		const std::string ALPHANUMERIC	= "abcdefghijklmnopqrstuvwxyz0123456789";
		const std::string HEX_CHARS		= "0123456789abcdef";
	}

	[[nodiscard]] inline std::string RandomString(size_t length, const std::string& character_set = detail::ALPHANUMERIC)
	{
		std::random_device rd;

		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distrib(0, character_set.length() - 1);

		std::string result;
		result.reserve(length);

		for (size_t i = 0; i < length; ++i)
		{
			result += character_set[distrib(gen)];
		}

		return result;
	}
}
