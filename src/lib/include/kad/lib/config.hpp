#pragma once

#include <kad/lib/data.hpp>
#include <kad/lib/preset.hpp>

#include <kad/common/no_copy_or_move.hpp>
#include <kad/common/tracked.hpp>

#include <filesystem>
#include <map>

namespace kad::lib
{
	class Context;

	class Config : public common::NoCopyOrMove
	{
	public:
		using PresetMap = std::map<std::string, Preset>;

		explicit Config(Context& context);

		~Config();

		void ResolveActivePreset();
		void SetActivePreset(const std::string& name);

		[[nodiscard]] Context& context() { return context_; }
		[[nodiscard]] const Context& context() const { return context_; }

		[[nodiscard]] const std::filesystem::path& path_config() const { return path_config_; }
		[[nodiscard]] const std::filesystem::path& path_config_presets() const { return path_config_presets_; }

		[[nodiscard]] const config::Config& data() const { return data_.value(); }

		[[nodiscard]] PresetMap& presets() { return presets_; }
		[[nodiscard]] const PresetMap& presets() const { return presets_; }

		[[nodiscard]] Preset* FindPreset(const std::string& name);
		[[nodiscard]] const Preset* FindPreset(const std::string& name) const;

		Preset& CreatePreset(const std::string& name, const std::filesystem::path& build_directory);
		void RemovePreset(const std::string& name);
		void RemovePreset(PresetMap::iterator it);

	private:
		Context& context_;

		std::filesystem::path path_config_;
		std::filesystem::path path_config_presets_;

		mutable common::Tracked<config::Config> data_;

		PresetMap presets_;
	};
}
