project(model)

add_library(
	${PROJECT_NAME}
		src/model.cpp
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
			kad::common
			simdjson::simdjson
)

if(BUILD_TESTS)
		add_executable(
			${PROJECT_NAME}-test
				test/test_model_query.cpp
				test/test_model_reply.cpp
				test/test_model_shared.cpp
		)

		target_link_libraries(
			${PROJECT_NAME}-test
				PRIVATE
					${PROJECT_NAME}
					Catch2::Catch2WithMain
		)

		catch_discover_tests(${PROJECT_NAME}-test)
endif()
