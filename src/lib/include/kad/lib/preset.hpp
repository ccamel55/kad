#pragma once

#include <kad/lib/target.hpp>
#include <kad/common/no_copy_or_move.hpp>

#include <filesystem>

namespace kad::lib
{
	constexpr auto PRESET_EXTENSION = ".json";

	namespace config
	{
		static constexpr uint16_t REVISION_PRESET = 1;

		struct Preset
		{
			struct Name
			{
				static constexpr auto REVISION = "revision";
				static constexpr auto BUILD_DIRECTORY = "build_directory";
			};

			uint16_t revision{ REVISION_PRESET };
			std::filesystem::path build_directory{ };
		};
	}

	/// From a directory `path`, get the latest API reply.
	std::optional<std::filesystem::path> GetApiReplyFile(const std::filesystem::path& path);

	class Config;

	class Preset : public common::NoCopyOrMove
	{
	public:
		Preset(Config& config, const std::string& name);
		Preset(Config& config, const std::string& name, const std::filesystem::path& build_directory);

		~Preset();

		/// Mark preset for deletion on destruction.
		void Delete();

		/// Create CMake file API request to retrieve required data about build.
		void CreateApiRequest();

		[[nodiscard]] bool HasApiRequest() const;
		[[nodiscard]] bool HasApiResponse() const;

		[[nodiscard]] Config& config() { return config_; }
		[[nodiscard]] const Config& config() const { return config_; }

		[[nodiscard]] const std::filesystem::path& path_preset_file() const { return path_preset_file_; }
		[[nodiscard]] const std::filesystem::path& path_preset_folder() const { return path_preset_folder_; }

		// Get corrected build directory from config file.
		// If the config stores relative path, we will return this as absolute.
		[[nodiscard]] std::filesystem::path DataBuildDirectory() const;

	private:
		void TryLoadPreset() const;

		[[nodiscard]] const config::Preset& data() const { TryLoadPreset(); return preset_.value(); }

	private:
		Config& config_;

		bool dirty_{ false };
		bool delete_{ false };

		std::filesystem::path path_preset_file_;
		std::filesystem::path path_preset_folder_;

		// Preset is not immediately loaded, instead we do lazy loading.
		mutable std::optional<config::Preset> preset_;
	};
}
