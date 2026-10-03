#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace kad::lib::config
{
	static constexpr uint16_t REVISION = 1;
	static constexpr uint16_t REVISION_PRESET = 1;
	static constexpr uint16_t REVISION_TARGET = 1;

	struct Config
	{
		uint16_t revision{ REVISION };
		std::string active_preset{ };
	};

	struct Preset
	{
		uint16_t revision{ REVISION_PRESET };
		std::filesystem::path build_directory{ };
	};

	struct Target
	{
		uint16_t revision{ REVISION_TARGET };
		std::map<std::string, std::string> environment_variables;
		std::vector<std::string> arguments;
		std::filesystem::path working_directory;
	};
}
