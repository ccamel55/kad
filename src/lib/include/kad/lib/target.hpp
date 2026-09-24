#pragma once

#include <kad/common/bit_flag.hpp>
#include <kad/common/no_copy_or_move.hpp>

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace kad::lib
{
	constexpr auto TARGET_CONFIG_EXTENSION = ".json";

	namespace config
	{
		static constexpr uint16_t REVISION_TARGET = 1;

		struct Target
		{
			struct Name
			{
				static constexpr auto REVISION = "revision";
				static constexpr auto ENVIRONMENT_VARIABLES = "environment_variables";
				static constexpr auto ARGUMENTS = "arguments";
				static constexpr auto WORKING_DIRECTORY = "workding_directory";
			};

			uint16_t revision{ REVISION_TARGET };
			std::map<std::string, std::string> environment_variables;
			std::vector<std::string> arguments;
			std::filesystem::path workding_directory;
		};
	}

	enum class TargetType
	{
		TARGET,
		ABSTRACT_TARGET,
	};

	class Preset;

	class Target : public common::NoCopyOrMove
	{
	public:
		using TargetMap = std::map<std::string, Target>;

		Target(Preset& preset, const std::string& name, const std::filesystem::path& path_target_json, TargetType type);
		~Target();

		[[nodiscard]] Preset& config() { return preset_; }
		[[nodiscard]] const Preset& config() const { return preset_; }

		[[nodiscard]] const std::filesystem::path& path_target_json() const { return path_target_json_; }
		[[nodiscard]] const std::filesystem::path& path_target_config() const { return path_target_config_; }

		[[nodiscard]] TargetType type() const { return type_; }

	private:
		Preset& preset_;

		std::filesystem::path path_target_json_;
		std::filesystem::path path_target_config_;

		TargetType type_;

	};
}
