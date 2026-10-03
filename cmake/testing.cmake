# Add our testing framework
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../thirdparty/Catch2)

include(CTest)
include(Catch)

enable_testing()
