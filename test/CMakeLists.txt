cmake_minimum_required(VERSION 3.15) # conda uses this cmakelist as the top-level for testing
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        project(COPCLIB-TEST LANGUAGES CXX C)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
include(BuildRequires)

find_package(Catch2 REQUIRED)
set(TEST_TARGET_NAME unit_tests)

file(GLOB ${TEST_TARGET_NAME}_SRC
        *.cpp
)

add_executable(${TEST_TARGET_NAME} ${${TEST_TARGET_NAME}_SRC})
target_link_libraries(${TEST_TARGET_NAME} COPCLIB::copc-lib Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(${TEST_TARGET_NAME} WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

include(DownloadExampleFiles)
download_test_files("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/autzen-classified.copc.laz")
download_test_files("${CMAKE_CURRENT_SOURCE_DIR}/data/autzen-classified.copc.laz")
