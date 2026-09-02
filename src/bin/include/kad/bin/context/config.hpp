#pragma once

#include <kad/common/no_copy_or_move.hpp>

#include <filesystem>
#include <set>

namespace kad::context
{
	namespace config
	{
		static constexpr uint16_t REVISION = 1;
		static constexpr uint16_t REVISION_PRESET = 1;

		struct Data
		{
			struct Name
			{
				static constexpr auto REVISION = "revision";
				static constexpr auto ACTIVE_PRESET = "active_preset";
			};

			uint16_t revision;
			std::string active_preset;
		};

		struct Preset
		{
			struct Name
			{

			};
		};
	}

	class Config : public common::NoCopy
	{
	public:
		explicit Config(const std::filesystem::path& path_kad);
		~Config();

	private:
		std::filesystem::path path_config_;
		std::filesystem::path path_config_presets_;

		config::Data data_;
		std::set<std::string> presets_;
	};
}
