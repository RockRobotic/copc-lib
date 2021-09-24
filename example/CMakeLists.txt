find_package(lazperf REQUIRED)
find_package(copc-lib QUIET)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/helpers.cmake NO_POLICY_SCOPE)
download_test_files("${CMAKE_CURRENT_BINARY_DIR}/autzen-classified.copc.laz")

add_executable(example_reader "example-reader.cpp")
target_link_libraries(example_reader copc-lib LAZPERF::lazperf)
add_executable(example_writer "example-writer.cpp")
target_link_libraries(example_writer copc-lib LAZPERF::lazperf)