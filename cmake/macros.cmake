macro(library)
	# Arguments
	set(_ARGS_OPT "")
	set(_ARGS_SINGLE NAME)
	set(_ARGS_MULTIPLE DEPENDENCIES)

	cmake_parse_arguments(
		LIB
			"${_ARGS_OPT}"
			"${_ARGS_SINGLE}"
			"${_ARGS_MULTIPLE}"
		${ARGN}
	)

	project(${LIB_NAME})

	message(STATUS "Library: ${PROJECT_NAME}")
	message(STATUS "	Dependencies: ${LIB_DEPENDENCIES}")

	# Find source files.
	file(
		GLOB_RECURSE
			LIB_SOURCE_FILES
		RELATIVE
			${CMAKE_CURRENT_LIST_DIR}
		${CMAKE_CURRENT_LIST_DIR}/src/**.c*
	)

	# Create library
	add_library(${PROJECT_NAME} ${LIB_SOURCE_FILES})
	add_library(kad::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

	target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
	target_link_libraries(${PROJECT_NAME} PUBLIC ${LIB_DEPENDENCIES})
endmacro()

macro(interface)
	# Arguments
	set(_ARGS_OPT "")
	set(_ARGS_SINGLE NAME)
	set(_ARGS_MULTIPLE DEPENDENCIES)

	cmake_parse_arguments(
		LIB
			"${_ARGS_OPT}"
			"${_ARGS_SINGLE}"
			"${_ARGS_MULTIPLE}"
		${ARGN}
	)

	project(${LIB_NAME})

	message(STATUS "Interface: ${PROJECT_NAME}")
	message(STATUS "	Dependencies: ${LIB_DEPENDENCIES}")

	# Create interface
	add_library(${PROJECT_NAME} INTERFACE)
	add_library(kad::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

	target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
	target_link_libraries(${PROJECT_NAME} INTERFACE ${LIB_DEPENDENCIES})
endmacro()

macro(test)
	# Arguments
	set(_ARGS_OPT "")
	set(_ARGS_SINGLE NAME)
	set(_ARGS_MULTIPLE DEPENDENCIES SOURCES)

	cmake_parse_arguments(
		LIB
		"${_ARGS_OPT}"
		"${_ARGS_SINGLE}"
		"${_ARGS_MULTIPLE}"
		${ARGN}
	)

	project(${LIB_NAME})

	message(STATUS "Test: ${PROJECT_NAME}")
	message(STATUS "	Dependencies: ${LIB_DEPENDENCIES}")

	# Create test
	add_executable(${PROJECT_NAME} ${LIB_SOURCES})

	target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
	target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain ${LIB_DEPENDENCIES})

	catch_discover_tests(${PROJECT_NAME})
endmacro()

macro(binary)
	# Arguments
	set(_ARGS_OPT "")
	set(_ARGS_SINGLE NAME)
	set(_ARGS_MULTIPLE DEPENDENCIES)

	cmake_parse_arguments(
		LIB
		"${_ARGS_OPT}"
		"${_ARGS_SINGLE}"
		"${_ARGS_MULTIPLE}"
		${ARGN}
	)

	project(${LIB_NAME})

	message(STATUS "Binary: ${PROJECT_NAME}")
	message(STATUS "	Dependencies: ${LIB_DEPENDENCIES}")

	# Find source files.
	file(
		GLOB_RECURSE
			LIB_SOURCE_FILES
		RELATIVE
			${CMAKE_CURRENT_LIST_DIR}
		${CMAKE_CURRENT_LIST_DIR}/src/**.c*
	)

	# Create binary
	add_executable(${PROJECT_NAME} ${LIB_SOURCE_FILES})

	target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
	target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_DEPENDENCIES})
endmacro()
