cmake_minimum_required(VERSION 3.26)

project(kad-cli LANGUAGES CXX)

include(cmake/main.cmake)

add_subdirectory(src)
add_subdirectory(thirdparty)
