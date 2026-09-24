#pragma once

#include <kad/lib/preset.hpp>
#include <kad/common/no_copy_or_move.hpp>

#include <filesystem>
#include <map>

namespace kad::lib
{
	namespace config
	{
		static constexpr uint16_t REVISION = 1;

		struct Config
		{
			struct Name
			{
				static constexpr auto REVISION = "revision";
				static constexpr auto ACTIVE_PRESET = "active_preset";
			};

			uint16_t revision{ REVISION };
			std::string active_preset{ };
		};
	}

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

		[[nodiscard]] config::Config& data() { return config_; }
		[[nodiscard]] const config::Config& data() const { return config_; }

		[[nodiscard]] PresetMap& presets() { return presets_; }
		[[nodiscard]] const PresetMap& presets() const { return presets_; }

		[[nodiscard]] Preset* FindPreset(const std::string& name);
		[[nodiscard]] const Preset* FindPreset(const std::string& name) const;

		Preset& CreatePreset(const std::string& name, const std::filesystem::path& build_directory);
		void RemovePreset(const std::string& name);
		void RemovePreset(PresetMap::iterator it);

	private:
		Context& context_;

		bool dirty_{ false };

		std::filesystem::path path_config_;
		std::filesystem::path path_config_presets_;

		mutable config::Config config_;

		PresetMap presets_;
	};
}
