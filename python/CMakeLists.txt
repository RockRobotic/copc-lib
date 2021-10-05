list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
include(BuildRequires)

set(COPC_PYTHON_LIB copclib)
find_package(pybind11 CONFIG REQUIRED)

# add the python lib
pybind11_add_module(${COPC_PYTHON_LIB} bindings.cpp)
target_link_libraries(${COPC_PYTHON_LIB} PRIVATE COPCLIB::copc-lib)

if(WITH_TESTS_AND_EXAMPLES)
    set_target_properties(${COPC_PYTHON_LIB} PROPERTIES
                                                        # have the python lib output to the python build dir
                                                        # not the /lib dir
                                                        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                                                        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    include(DownloadExampleFiles)
    download_test_files("${CMAKE_CURRENT_BINARY_DIR}/autzen-classified.copc.laz")

    # PY_SOURCE_DIR is the folder we'll copy the python scripts into
    set(PY_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR})

    # gather all the python scripts
    file(GLOB COPC_PYTHON_FILES
            ${CMAKE_SOURCE_DIR}/test/*.py
            ${CMAKE_SOURCE_DIR}/example/*.py
    )

    # copy all the python scripts into the build folder
    foreach(filename ${COPC_PYTHON_FILES})
        configure_file(${filename} ${PY_SOURCE_DIR} COPYONLY)
    endforeach(filename)

    # add the tests
    add_test(NAME python-tests
             COMMAND ${PYTHON_EXECUTABLE} -m pytest "${PY_SOURCE_DIR}"
             WORKING_DIRECTORY ${PY_SOURCE_DIR}
            )
    add_test(NAME example_reader_py
             COMMAND ${PYTHON_EXECUTABLE} "${PY_SOURCE_DIR}/example-reader.py"
             WORKING_DIRECTORY ${PY_SOURCE_DIR}
            )
    add_test(NAME example_writer_py
             COMMAND ${PYTHON_EXECUTABLE} "${PY_SOURCE_DIR}/example-writer.py"
             WORKING_DIRECTORY ${PY_SOURCE_DIR}
            )

    set_tests_properties(python-tests example_reader_py example_writer_py
        PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}:$ENV{PYTHONPATH}")
endif()
