#pragma once

#include <kad/bin/cli/base.hpp>

#include <filesystem>

namespace kad::cli
{
	class CommandInit final : public CommandBase
	{
	public:
		struct Data : public CommandData
		{
			std::filesystem::path root;
		};

		explicit CommandInit(CLI::App* parent);

		CommandData* Data() override { return &data_; }
		const CommandData* Data() const override { return &data_; }

	protected:
		void HandleCommandImpl() override;

	private:
		struct Data data_;

	};
}
