#include <kad/lib/preset.hpp>
#include <kad/lib/target.hpp>

#include <kad/lib/data.hpp>
#include <kad/lib/data_json.hpp>

#include <kad/common/traits.hpp>

#include <numeric>

using namespace kad::lib;

namespace
{
	constexpr auto KAD_TARGETS = "targets";

// 	bool CanMigrate()
// 	{
// 		return false;
// 	}
//
// 	void ApplyMigration()
// 	{
//
// 	}
}

size_t kad::lib::GetNumTargets(const std::vector<kad::model::reply::CodeModel::Configurations>& configurations)
{
	return std::accumulate(
		configurations.begin(), configurations.end(), 0,
		[](size_t count, const kad::model::reply::CodeModel::Configurations& config)
		{
			return count
				+ config.targets.size()
				+ config.abstractTargets.transform([](const auto& x){ return x.size(); }).value_or(0);
		});
}

Targets::Targets(Preset* preset, const model::reply::CodeModel& codemodel)
	: preset_{ preset }
	, path_target_folder_{ preset_->path_preset_folder() / KAD_TARGETS }
	, targets_{ common::FlatMapReserved<std::string, Target>(GetNumTargets(codemodel.configurations)) }
{
	for (const auto& configuration: codemodel.configurations)
	{
		for (const auto& target: configuration.targets)
		{
			targets_.emplace(target.name, Target{});
		}

		if (configuration.abstractTargets.has_value())
		{
			for (const auto& abstract_target: configuration.abstractTargets.value())
			{
				targets_.emplace(abstract_target.name, Target{});
			}
		}
	}
}

Targets::~Targets()
{

}

spdlog::logger* Targets::logger() const
{
	return preset().logger();
}

Target* Targets::FindTarget(const std::string& name)
{
	const auto preset = targets_.find(name);
	return preset == targets_.end()
		? nullptr
		: &preset->second;
}

const Target* Targets::FindTarget(const std::string& name) const
{
	const auto preset = targets_.find(name);
	return preset == targets_.end()
		? nullptr
		: &preset->second;
}
