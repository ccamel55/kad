project(common)

add_library(
	${PROJECT_NAME}
		src/common.cpp
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
)

if(BUILD_TESTS)
		add_executable(
			${PROJECT_NAME}-test
				test/test_bitflag.cpp
				test/test_scope_exit.cpp
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
