#pragma once

#include <kad/lib/data.hpp>

#include <kad/common/no_copy_or_move.hpp>
#include <kad/common/lazy.hpp>
#include <kad/common/tracked.hpp>

#include <kad/model/reply/codemodel.hpp>
#include <kad/model/reply/index.hpp>

#include <filesystem>

namespace kad::lib
{
	constexpr auto PRESET_EXTENSION = ".json";

	class Config;

	class Preset : public common::NoCopyOrMove
	{
	public:
		Preset(
			Config& config,
			const std::string& name,
			const std::filesystem::path& build_directory = ""
		);

		~Preset();

		/// Mark preset for deletion on destruction.
		void Delete();

		/// Create CMake file API request to retrieve required data about build.
		void CreateApiRequest() const;

		[[nodiscard]] bool HasApiRequest() const;
		[[nodiscard]] bool HasApiResponse() const;

		using IndexOrErrorFile	= std::expected<std::filesystem::path, std::filesystem::path>;
		using IndexOrError		= std::expected<model::reply::Index, model::reply::Index>;

		[[nodiscard]] std::optional<IndexOrErrorFile> GetApiResponseFile() const;
		[[nodiscard]] std::optional<IndexOrError> GetApiResponse() const;

		[[nodiscard]] Config& config() { return config_; }
		[[nodiscard]] const Config& config() const { return config_; }

		[[nodiscard]] const std::filesystem::path& path_preset_file() const { return path_preset_file_; }
		[[nodiscard]] const std::filesystem::path& path_preset_folder() const { return path_preset_folder_; }

		void data_lazy_invalidate() { data_.invalidate(); }
		[[nodiscard]] const config::Preset& data() const { return data_.value().value(); }

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
	};
}
