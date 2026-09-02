set(FTXUI_ENABLE_INSTALL OFF)
set(REPROC++ ON)
set(SPDLOG_USE_STD_FORMAT ON)
set(SPDLOG_DISABLE_DEFAULT_LOGGER ON)

add_subdirectory(CLI11)
add_subdirectory(FTXUI)
add_subdirectory(json)
add_subdirectory(magic_enum)
add_subdirectory(rapidfuzz-cpp)
add_subdirectory(reproc)
add_subdirectory(spdlog)

add_library(reproc::reproc ALIAS reproc++)
