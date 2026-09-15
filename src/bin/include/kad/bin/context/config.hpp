#pragma once

#include <kad/bin/context/preset.hpp>
#include <kad/common/no_copy_or_move.hpp>

#include <filesystem>
#include <map>

namespace kad::context
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

	class Config : public common::NoCopy
	{
	public:
		explicit Config(const std::filesystem::path& path_kad);

		~Config();

		void ResolveActivePreset();
		void SetActivePreset(const std::string& name);

		[[nodiscard]] config::Config& data() { return config_; }
		[[nodiscard]] const config::Config& data() const { return config_; }

		[[nodiscard]] const std::map<std::string, Preset>& presets() const { return presets_; }

		[[nodiscard]] Preset* FindPreset(const std::string& name);
		[[nodiscard]] const Preset* FindPreset(const std::string& name) const;

		Preset& CreatePreset(const std::string& name, const std::filesystem::path& build_directory);
		void RemovePreset(const std::string& name);

	private:
		bool dirty_{ false };

		std::filesystem::path path_config_;
		std::filesystem::path path_config_presets_;

		mutable config::Config config_;
		std::map<std::string, Preset> presets_;
	};
}
