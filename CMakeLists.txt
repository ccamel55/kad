cmake_minimum_required(VERSION 3.26)

project(kad-cli LANGUAGES CXX)

# Needed for Clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
		add_subdirectory(thirdparty/Catch2)
		include(CTest)
		include(Catch)
		enable_testing()
endif()

add_subdirectory(src)
add_subdirectory(thirdparty)
