#pragma once

#include <kad/lib/data.hpp>

#include <kad/common/bit_flag.hpp>
#include <kad/common/no_copy_or_move.hpp>

#include <kad/model/reply/target.hpp>

#include <filesystem>
#include <string>

namespace kad::lib
{
	constexpr auto TARGET_CONFIG_EXTENSION = ".json";

	class Preset;

	class Target : public common::NoCopyOrMove
	{
	public:
		Target(Preset& preset, const std::string& name, const std::filesystem::path& path_target_json);
		~Target();

		[[nodiscard]] Preset& config() { return preset_; }
		[[nodiscard]] const Preset& config() const { return preset_; }

		[[nodiscard]] const std::filesystem::path& path_target_json() const { return path_target_json_; }
		[[nodiscard]] const std::filesystem::path& path_target_config() const { return path_target_config_; }

	private:
		Preset& preset_;

		std::filesystem::path path_target_json_;
		std::filesystem::path path_target_config_;

	};
}
