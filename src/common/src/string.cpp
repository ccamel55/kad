#include <kad/common/string.hpp>

using namespace kad::common;

std::string& string::rtrim(std::string& s, const char* t)
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

std::string& string::ltrim(std::string& s, const char* t)
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

std::string& string::trim(std::string& s, const char* t)
{
	return ltrim(rtrim(s, t), t);
}
