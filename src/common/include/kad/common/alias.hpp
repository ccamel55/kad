#pragma once

#include <expected>
#include <string>

template <typename Type>
using ResultStr = std::expected<Type, std::string>;
