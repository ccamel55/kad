#pragma once

#include <kad/lib/target.hpp>
#include <kad/common/no_copy_or_move.hpp>
#include <kad/common/lazy.hpp>
#include <kad/common/tracked.hpp>

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
		using TargetMap = std::map<std::string, Target>;

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

		[[nodiscard]] const config::Preset& data() const { return data_.value().value(); }

		[[nodiscard]] TargetMap& targets() { return targets_; }
		[[nodiscard]] const TargetMap& targets() const { return targets_; }

		[[nodiscard]] Target* FindTarget(const std::string& name);
		[[nodiscard]] const Target* FindTarget(const std::string& name) const;

		// Get corrected build directory from config file.
		// If the config stores relative path, we will return this as absolute.
		[[nodiscard]] std::filesystem::path DataBuildDirectory() const;

	private:
		Config& config_;

		bool dirty_{ false };
		bool delete_{ false };

		std::filesystem::path path_preset_file_;
		std::filesystem::path path_preset_folder_;

		mutable common::Lazy<common::Tracked<config::Preset>> data_;

		TargetMap targets_;
	};
}
