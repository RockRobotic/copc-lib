find_package(lazperf REQUIRED)
find_package(copc-lib REQUIRED)
find_package(Catch2 REQUIRED)

include(${CMAKE_SOURCE_DIR}/cmake/helpers.cmake NO_POLICY_SCOPE)
download_test_files("${CMAKE_CURRENT_BINARY_DIR}/autzen-classified.copc.laz")

file(GLOB TEST_SRCS
        *.cpp
        )

add_executable(tests ${TEST_SRCS})
target_link_libraries(tests copc-lib LAZPERF::lazperf Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(tests)