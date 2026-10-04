library(
	NAME
		lib

	DEPENDENCIES
		kad::common
		kad::model
		rapidfuzz::rapidfuzz
		spdlog::spdlog
)

test(
	NAME
		lib-config

	SOURCES
		./test/test_config.cpp

	DEPENDENCIES
		kad::lib
)

test(
	NAME
		lib-context

	SOURCES
		./test/test_context.cpp

	DEPENDENCIES
		kad::lib
)

test(
	NAME
		lib-preset

	SOURCES
		./test/test_preset.cpp

	DEPENDENCIES
		kad::lib
)

test(
	NAME
		lib-target

	SOURCES
		./test/test_target.cpp

	DEPENDENCIES
		kad::lib
)
