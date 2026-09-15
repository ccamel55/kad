#pragma once

#include <kad/bin/cli/base.hpp>
#include <kad/bin/context/context.hpp>

namespace kad::cli
{
	class CommandTarget final : public CommandBase
	{
	public:
		struct Data : public CommandData
		{
			Lazy<context::Context> context;

			std::string preset{ };
			std::string target{ };
		};

		explicit CommandTarget(CLI::App* parent);

		CommandData* Data() override { return &data_; }
		const CommandData* Data() const override { return &data_; }

	protected:
		void HandleCommandImpl() override;

	private:
		struct Data data_;

		std::unique_ptr<CommandBase> cmd_build;
		std::unique_ptr<CommandBase> cmd_build_and_run;
	};
}
