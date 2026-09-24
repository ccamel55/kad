#pragma once

#include <string>

namespace kad::common::string
{
	namespace detail
	{
		inline const char* WHITESPACE = " \t\n\r\f\v";
	}

	std::string& rtrim(std::string& s, const char* t = detail::WHITESPACE);
	std::string& ltrim(std::string& s, const char* t = detail::WHITESPACE);

	std::string& trim(std::string& s, const char* t = detail::WHITESPACE);
}
