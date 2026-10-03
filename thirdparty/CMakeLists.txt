set(FTXUI_ENABLE_INSTALL OFF)

set(SIMDJSON_DISABLE_DEPRECATED_API ON)

set(SPDLOG_USE_STD_FORMAT ON)
set(SPDLOG_DISABLE_DEFAULT_LOGGER ON)

add_subdirectory(CLI11)
add_subdirectory(FTXUI)
add_subdirectory(magic_enum)
add_subdirectory(rapidfuzz-cpp)
add_subdirectory(simdjson)
add_subdirectory(spdlog)

add_library(visit_struct INTERFACE)
add_library(visit_struct::visit_struct ALIAS visit_struct)
target_include_directories(visit_struct INTERFACE visit_struct/include)

add_library(subprocess INTERFACE)
add_library(subprocess::subprocess ALIAS subprocess)
target_include_directories(subprocess INTERFACE subprocess.h)
