#pragma once

#include <kad/common/process.hpp>

#include <filesystem>
#include <string>

namespace kad::process
{
	class CMake
	{
	public:
		struct ParamsConfigure
		{
			std::filesystem::path cmake_root;
			std::filesystem::path build_directory;

			std::string preset;
		};

		struct ParamsBuild
		{

		};

		explicit CMake(const std::filesystem::path& cmake = "cmake")
			: cmake_{ cmake }
		{ }

		[[nodiscard]] common::CreateProcessResult Configue(const ParamsConfigure& params) const;
		[[nodiscard]] common::CreateProcessResult Build(const ParamsBuild& params) const;

	private:
		std::filesystem::path cmake_;

	};
}
