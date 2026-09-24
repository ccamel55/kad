project(common)

add_library(
	${PROJECT_NAME}
		src/file.cpp
		src/process.cpp
		src/string.cpp
)

add_library(kad::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(
	${PROJECT_NAME}
		PUBLIC
			${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(
	${PROJECT_NAME}
		PUBLIC
			magic_enum::magic_enum
			nlohmann_json::nlohmann_json
			subprocess::subprocess
)

if(BUILD_TESTS)
		add_executable(
			${PROJECT_NAME}-test
				test/test_bitflag.cpp
				test/test_file.cpp
				test/test_process.cpp
				test/test_scope_exit.cpp
				test/test_string.cpp
				test/test_version.cpp
		)

		target_link_libraries(
			${PROJECT_NAME}-test
				PRIVATE
					${PROJECT_NAME}
					Catch2::Catch2WithMain
		)

		catch_discover_tests(${PROJECT_NAME}-test)
endif()
