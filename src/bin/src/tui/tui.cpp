#include <kad/bin/tui/tui.hpp>

#include <ftxui/ftxui.hpp>
#include <rapidfuzz/fuzz.hpp>

#include <ranges>

using namespace kad::tui;

Tui::Tui(const TuiData& data)
	: data_{ data }
{

}

void Tui::RunBlocking()
{
	auto screen = ftxui::App::Fullscreen();

	auto entries = data_.targets
		| std::ranges::views::transform([](const auto& target){ return target.name; })
		| std::ranges::to<std::vector>();

	std::string search;
	std::optional<rapidfuzz::fuzz::CachedRatio<std::string::value_type>> scorer;
	auto option_input = ftxui::InputOption::Default();
	{
		option_input.insert = true;
		option_input.multiline = false;
		option_input.transform = [](ftxui::InputState state)
		{
			if (state.focused)
			{
				return state.element | ftxui::bgcolor(ftxui::Color::GrayDark);
			}
			else
			{
				return state.element;
			}
		};

		constexpr double SCORE_CUTOFF = 25.0;

		option_input.on_enter = [&]() { };
		option_input.on_change = [&]()
		{
			if (search.empty())
			{
				scorer.reset();
				entries = data_.targets
					| std::ranges::views::transform([](const auto& target){ return target.name; })
					| std::ranges::to<std::vector>();
			}
			else
			{
				scorer.emplace(search | std::ranges::views::transform([](char c){ return std::tolower(c); }));
				entries.clear();

				std::vector<std::pair<std::string, double>> results;
				for (const auto& target: data_.targets)
				{
					const auto name_lower = target.name
						| std::ranges::views::transform([](char c){ return std::tolower(c); })
						| std::ranges::to<std::string>();

					const auto score = scorer->similarity(name_lower, SCORE_CUTOFF);
					if (score < SCORE_CUTOFF)
					{
						continue;
					}

					auto entry = std::make_pair(target.name, score);
					results.insert(
						std::ranges::lower_bound(results, entry, [](const auto& a, const auto& b){ return a.second > b.second; }),
						std::move(entry)
					);
				}

				entries = results
					| std::ranges::views::transform([](const auto& target){ return target.first; })
					| std::ranges::to<std::vector>();
			}
		};
	}

	auto selected =  0;
	auto option_menu = ftxui::MenuOption::Vertical();
	{
		option_menu.underline.enabled = false;
		option_menu.entries_option.transform = [](const ftxui::EntryState& state)
		{
			if (state.focused)
			{
				return ftxui::text(std::format("· {}", state.label)) | ftxui::bgcolor(ftxui::Color::GrayDark);
			}
			else
			{
				return ftxui::text(std::format("  {}", state.label));
			}
		};

		option_menu.on_enter = [&]() { };
		option_menu.on_change = [&]() { };
	}

	auto input = ftxui::Input(&search, "search", option_input);
	auto menu = ftxui::Menu(&entries, &selected, option_menu);

	auto footer = ftxui::Renderer([&](){
		if (input->Focused())
		{
			return ftxui::text(std::format("{} results", entries.size()));
		}

		return ftxui::hbox({
			ftxui::hbox({
				ftxui::text("b") | ftxui::underlined,
				ftxui::text("uild"),
				ftxui::text(" | "),
				ftxui::text("build and "),
				ftxui::text("r") | ftxui::underlined,
				ftxui::text("un"),
			}),
			ftxui::text("") | ftxui::xflex_grow,
			ftxui::hbox({
				ftxui::text("p") | ftxui::underlined | ftxui::dim,
				ftxui::text("resets") | ftxui::dim,
				ftxui::text(" | "),
				ftxui::text("t") | ftxui::underlined,
				ftxui::text("argets"),
				ftxui::text(" "),
				ftxui::text("  debug  ") | ftxui::bgcolor(ftxui::Color::GrayDark),
			}) | ftxui::align_right,
		});
	});

	auto component_tree = ftxui::Container::Vertical({
		input,
		menu
	});

	auto component = ftxui::Renderer(component_tree, [&]
	{
		return ftxui::vbox({
			input->Render(),
			ftxui::separator(),
			menu->Render() | ftxui::frame | ftxui::flex,
			ftxui::separator(),
			footer->Render()
		});
	});

	bool force_quit = false;
	component |= ftxui::CatchEvent([&](ftxui::Event event)
	{
		if (event == ftxui::Event::Character('q'))
		{
			force_quit = true;
			return true;
		}

		if (input->Focused())
		{
			if (event == ftxui::Event::Escape || event == ftxui::Event::Return)
			{
				menu->TakeFocus();
				return true;
			}
		}
		else
		{
			if (event == ftxui::Event::Character('/'))
			{
				input->TakeFocus();
				return true;
			}
		}

		return false;
	});

	menu->TakeFocus();
	ftxui::Loop loop{ &screen, component };

	while (!loop.HasQuitted() && !force_quit)
	{
		loop.RunOnceBlocking();
	}
}
