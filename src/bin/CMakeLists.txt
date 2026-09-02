project(kad)

add_executable(
	${PROJECT_NAME}
		src/main.cpp
		src/cli/cli.cpp
		src/cli/init.cpp
		src/cli/preset.cpp
		src/cli/target.cpp
		src/tui/tui.cpp
		src/context/config.cpp
		src/context/context.cpp
)

target_include_directories(
	${PROJECT_NAME}
		PRIVATE
			include
)

target_link_libraries(
	${PROJECT_NAME}
		PUBLIC
			CLI11::CLI11
			ftxui::ftxui
			rapidfuzz::rapidfuzz
			reproc::reproc
			spdlog::spdlog
			kad::common
			kad::model
)
