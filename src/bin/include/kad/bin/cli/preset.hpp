#pragma once

#include <kad/bin/cli/base.hpp>
#include <kad/common/lazy.hpp>
#include <kad/lib/context.hpp>


namespace kad::cli
{
	class CommandPreset final : public CommandBase
	{
	public:
		struct Data : public CommandData
		{
			kad::common::Lazy<lib::Context> context;

			std::string preset{ };
		};

		explicit CommandPreset(CLI::App* parent);

		CommandData* Data() override { return &data_; }
		const CommandData* Data() const override { return &data_; }

	protected:
		void HandleCommandImpl() override;

	private:
		struct Data data_;

		std::unique_ptr<CommandBase> cmd_active;
		std::unique_ptr<CommandBase> cmd_configure;
		std::unique_ptr<CommandBase> cmd_add;
		std::unique_ptr<CommandBase> cmd_remove;

	};
}
