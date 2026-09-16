#include <kad/bin/process/cmake.hpp>

using namespace kad::process;

kad::common::CreateProcessResult CMake::Configue(const ParamsConfigure& params) const
{
	common::Process::Params process_params
	{
		.args = { "--preset", params.preset, "-B", params.build_directory },
		.working_directory = params.cmake_root,
	};

	return common::CreateProcess(cmake_, std::move(process_params));
}

kad::common::CreateProcessResult CMake::Build(const ParamsBuild& params) const
{
	return std::unexpected{ "not implemented" };
}
