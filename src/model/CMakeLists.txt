interface(
	NAME
		model

	DEPENDENCIES
		kad::common
		simdjson::simdjson
)

test(
	NAME
		model-query

	SOURCES
		./test/test_model_query.cpp

	DEPENDENCIES
		kad::model
)

test(
	NAME
		model-reply

	SOURCES
		./test/test_model_reply.cpp

	DEPENDENCIES
		kad::model
)

test(
	NAME
		model-shared

	SOURCES
		./test/test_model_shared.cpp

	DEPENDENCIES
		kad::model
)
