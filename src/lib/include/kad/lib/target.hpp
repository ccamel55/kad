#pragma once

#include <kad/lib/data.hpp>

#include <kad/common/bit_flag.hpp>
#include <kad/common/no_copy_or_move.hpp>

#include <kad/model/reply/codemodel.hpp>
#include <kad/model/reply/target.hpp>

#include <spdlog/spdlog.h>

#include <filesystem>
#include <flat_map>
#include <string>

namespace kad::lib
{
	constexpr auto TARGET_CONFIG_EXTENSION = ".json";

	class Preset;

	class Target : public common::NoCopy
	{
	public:

	};

	[[nodiscard]] size_t GetNumTargets(const std::vector<kad::model::reply::CodeModel::Configurations>& configurations);

	class Targets : public common::NoCopy
	{
	public:
		using TargetMap = std::flat_map<std::string, Target>;

		Targets(Preset* preset, const model::reply::CodeModel& codemodel);
		~Targets();

		[[nodiscard]] spdlog::logger* logger() const;

		[[nodiscard]] Preset& preset() { return *preset_; }
		[[nodiscard]] const Preset& preset() const { return *preset_; }

		[[nodiscard]] const std::filesystem::path& path_target_folder() const { return path_target_folder_; }

		[[nodiscard]] TargetMap& targets() { return targets_; }
		[[nodiscard]] const TargetMap& targets() const { return targets_; }

		[[nodiscard]] Target* FindTarget(const std::string& name);
		[[nodiscard]] const Target* FindTarget(const std::string& name) const;

	private:
		Preset* preset_;

		std::filesystem::path path_target_folder_;

		TargetMap targets_;
	};
}
