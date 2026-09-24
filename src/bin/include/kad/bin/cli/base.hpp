#pragma once

#include <kad/lib/context.hpp>

#include <CLI/CLI.hpp>

namespace kad::cli
{
	/// Recursively search for root folder and abort if it doesn't exist.
	[[nodiscard]] inline std::filesystem::path FindRootFolderOrThrow(
		const std::filesystem::path& cwd = std::filesystem::current_path(),
		size_t max_depth = 100
	)
	{
		const auto path = lib::FindRootDirectory(cwd, max_depth);
		if (!path.has_value()) [[unlikely]]
		{
			throw CLI::ValidationError("Could not find root folder for current project");
		}
		return path.value();
	}

	[[nodiscard]] inline bool AppHasSubcommand(const CLI::App* app)
	{
		const auto subcommands = app->get_subcommands();
		const bool has_subcommand = std::ranges::find_if(subcommands, [](const CLI::App* subcommand)
		{
			return subcommand && subcommand->parsed();
		}) != subcommands.end();

		return has_subcommand;
	}

	// TODO(ALLAN): move this into common
	template <typename Type>
	class Lazy
	{
	public:
		constexpr Lazy(std::function<void(std::optional<Type>&)> init)
			: init_{ std::move(init) }
		{ }

		constexpr Type* operator->() { TryInit(); return  get(); }
		constexpr const Type* operator->() const { TryInit(); return get(); }

		constexpr Type& operator*() { TryInit(); return value_.value(); }
		constexpr const Type& operator*() const { TryInit(); return value_.value(); }

		constexpr Type* get() { TryInit(); return std::addressof(value_.value()); }
		constexpr const Type* get() const { TryInit(); return std::addressof(value_.value()); }

	private:
		void TryInit() const
		{
			if (value_.has_value()) [[likely]]
			{
				return;
			}

			init_(value_);
		}

	private:
		mutable std::function<void(std::optional<Type>&)>	init_;
		mutable std::optional<Type>							value_;

	};

	struct CommandData { };

	class CommandBase : public common::NoCopyOrMove
	{
	public:
		CommandBase(CommandBase* parent, CLI::App* command)
			: parent_{ parent }
			, command_{ command }
		{
			command_->callback([this]()
			{
				if (AppHasSubcommand(command_))
				{
					return;
				}
				this->HandleCommandImpl();
			});
		}

		virtual ~CommandBase() = default;

		CLI::App* Command() { return command_; }
		const CLI::App* Command() const { return command_; }

		CommandBase* Parent() { return parent_; }
		const CommandBase* Parent() const { return parent_; }

		virtual CommandData* Data() { return nullptr; }
		virtual const CommandData* Data() const { return nullptr; }

		template <typename DerivedType> requires std::derived_from<DerivedType, CommandData>
		DerivedType* DataAs() { return Data() ? static_cast<DerivedType*>(Data()): nullptr; }

		template <typename DerivedType> requires std::derived_from<DerivedType, CommandData>
		const DerivedType* DataAs() const { return Data() ? static_cast<const DerivedType*>(Data()): nullptr; }

	protected:
		virtual void HandleCommandImpl() = 0;

	private:
		CommandBase* parent_;
		CLI::App* command_;

	};
}
