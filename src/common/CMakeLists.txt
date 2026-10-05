library(
	NAME
		common

	DEPENDENCIES
		magic_enum::magic_enum
		spdlog::spdlog
		simdjson::simdjson
		subprocess::subprocess
		visit_struct::visit_struct
)

test(
	NAME
		common-test

	SOURCES
		./test/test_bitflag.cpp
		./test/test_file.cpp
		./test/test_json.cpp
		./test/test_lazy.cpp
		./test/test_process.cpp
		./test/test_scope_exit.cpp
		./test/test_string.cpp
		./test/test_tracked.cpp
		./test/test_version.cpp

	DEPENDENCIES
		kad::common
)
