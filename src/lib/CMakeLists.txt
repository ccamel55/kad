project(lib)

add_library(
	${PROJECT_NAME}
		src/config.cpp
		src/context.cpp
		src/preset.cpp
		src/target.cpp
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
			rapidfuzz::rapidfuzz
			kad::common
			kad::model
)

if(BUILD_TESTS)
		add_executable(
			${PROJECT_NAME}-test
				test/test_config.cpp
				test/test_context.cpp
				test/test_preset.cpp
				test/test_target.cpp
		)

		target_link_libraries(
			${PROJECT_NAME}-test
				PRIVATE
					${PROJECT_NAME}
					Catch2::Catch2WithMain
		)

		catch_discover_tests(${PROJECT_NAME}-test)
endif()
