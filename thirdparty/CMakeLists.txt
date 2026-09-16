set(FTXUI_ENABLE_INSTALL OFF)
set(SPDLOG_USE_STD_FORMAT ON)
set(SPDLOG_DISABLE_DEFAULT_LOGGER ON)

add_subdirectory(CLI11)
add_subdirectory(FTXUI)
add_subdirectory(json)
add_subdirectory(magic_enum)
add_subdirectory(rapidfuzz-cpp)
add_subdirectory(spdlog)

add_library(subprocess INTERFACE)
add_library(subprocess::subprocess ALIAS subprocess)
target_include_directories(subprocess INTERFACE subprocess.h)
