if (WIN32)
	message(FATAL_ERROR "Windows is not supported.")
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_EXTENSIONS OFF)

# Load it all
include(cmake/compiler_options.cmake)
include(cmake/testing.cmake)
include(cmake/macros.cmake)
