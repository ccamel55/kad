#pragma once

#include <string>

namespace kad::common::string
{
	namespace detail
	{
		inline const char* WHITESPACE = " \t\n\r\f\v";
	}

	inline std::string& rtrim(std::string& s, const char* t = detail::WHITESPACE)
	{
		s.erase(s.find_last_not_of(t) + 1);
		return s;
	}

	inline std::string& ltrim(std::string& s, const char* t = detail::WHITESPACE)
	{
		s.erase(0, s.find_first_not_of(t));
		return s;
	}

	inline std::string& trim(std::string& s, const char* t = detail::WHITESPACE)
	{
		return ltrim(rtrim(s, t), t);
	}
}
