set (COPC_PYTHON_LIB copclib)

find_package(lazperf REQUIRED)
find_package(copc-lib REQUIRED)
find_package(pybind11 CONFIG)

include(${CMAKE_SOURCE_DIR}/cmake/helpers.cmake NO_POLICY_SCOPE)
download_test_files("${CMAKE_SOURCE_DIR}/test/data/autzen-classified.copc.laz")

pybind11_add_module(${COPC_PYTHON_LIB} ${SRCS} bindings.cpp)
target_link_libraries(${COPC_PYTHON_LIB} PRIVATE ${COPC_STATIC_LIB})
target_compile_definitions(${COPC_PYTHON_LIB} PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})

if(WITH_TESTS)
    add_test(NAME python-tests
            COMMAND ${PYTHON_EXECUTABLE} -m pytest "${CMAKE_SOURCE_DIR}/test"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test"
            )
    add_test(NAME example_reader_py
            COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/example/example-reader.py"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/example"
            )
    add_test(NAME example_writer_py
            COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/example/example-writer.py"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/example"
            )

    set_tests_properties(python-tests example_reader_py example_writer_py
        PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}:$ENV{PYTHONPATH}")
endif()