if(SKBUILD)
    # Scikit-Build does not add your site-packages to the search path
    # automatically, so we need to add it _or_ the pybind11 specific directory
    # here.
    execute_process(
        COMMAND "${PYTHON_EXECUTABLE}" -c
                "import pybind11; print(pybind11.get_cmake_dir())"
        OUTPUT_VARIABLE _tmp_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT)
    list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
#include(BuildRequires)

set(COPC_PYTHON_LIB _core)
find_package(pybind11 CONFIG REQUIRED)

# add the python lib
pybind11_add_module(${COPC_PYTHON_LIB} bindings.cpp)
if (LAZPERF_IS_FOUND)
    target_link_libraries(${COPC_PYTHON_LIB} PRIVATE lazperf_s copc-lib-s)
else()
    target_link_libraries(${COPC_PYTHON_LIB} PRIVATE ${LAZPERF_LIB_NAME} copc-lib-s)
endif()

install(TARGETS ${COPC_PYTHON_LIB} DESTINATION .)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/copclib DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(${COPC_PYTHON_LIB} PROPERTIES
                        # have the python lib output to the python build dir
                        # not the /lib dir
                        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/copclib
                        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/copclib)

if(UNIX AND NOT APPLE)
    add_custom_command(TARGET ${COPC_PYTHON_LIB} POST_BUILD
        COMMAND stubgen -m _core -o .
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/copclib
        COMMENT "Generating stubs for pybind"
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/copclib/_core.pyi DESTINATION .)
endif()

if(WITH_TESTS)
    include(DownloadExampleFiles)
    download_test_files("${CMAKE_CURRENT_BINARY_DIR}/data/autzen-classified.copc.laz")

    # PY_SOURCE_DIR is the folder we'll copy the python scripts into
    set(PY_SOURCE_DIR ${CMAKE_SOURCE_DIR}/test)
    set(PY_EXAMPLE_DIR ${CMAKE_SOURCE_DIR}/example)

    # add the tests
    add_test(NAME python-tests
             COMMAND ${PYTHON_EXECUTABLE} -m pytest "${PY_SOURCE_DIR}"
             WORKING_DIRECTORY ${PY_SOURCE_DIR}
            )
    add_test(NAME example_reader_py
             COMMAND ${PYTHON_EXECUTABLE} "${PY_EXAMPLE_DIR}/example_reader.py"
             WORKING_DIRECTORY ${PY_EXAMPLE_DIR}
            )
    add_test(NAME example_writer_py
             COMMAND ${PYTHON_EXECUTABLE} "${PY_EXAMPLE_DIR}/example_writer.py"
             WORKING_DIRECTORY ${PY_EXAMPLE_DIR}
            )
    add_test(NAME example_laz_writer_py
                COMMAND ${PYTHON_EXECUTABLE} "${PY_EXAMPLE_DIR}/example_laz_writer.py"
                WORKING_DIRECTORY ${PY_EXAMPLE_DIR}
            )

    set_tests_properties(python-tests example_reader_py example_writer_py example_laz_writer_py
        PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}:$ENV{PYTHONPATH}")
endif()
