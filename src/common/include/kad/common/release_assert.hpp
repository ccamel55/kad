#pragma once

#include <iostream>

#define release_assert(assert, str) 	\
	if (!(assert)) [[unlikely]]			\
	{									\
		std::cerr << str << std::endl;	\
		std::abort();					\
	}
