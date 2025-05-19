cmake_minimum_required(VERSION 3.15) # conda uses this cmakelist as the top-level for testing
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        project(COPCLIB-EXAMPLES LANGUAGES CXX C)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
include(BuildRequires)

add_executable(example_reader "example-reader.cpp")
target_link_libraries(example_reader COPCLIB::copc-lib)
add_executable(example_writer "example-writer.cpp")
target_link_libraries(example_writer COPCLIB::copc-lib)
add_executable(example_laz_writer "example-laz-writer.cpp")
target_link_libraries(example_laz_writer COPCLIB::copc-lib)

enable_testing()
add_test(NAME example_reader COMMAND example_reader WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_test(NAME example_writer COMMAND example_writer WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_test(NAME example_laz_writer COMMAND example_laz_writer WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

include(DownloadExampleFiles)
download_test_files("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/autzen-classified.copc.laz")
