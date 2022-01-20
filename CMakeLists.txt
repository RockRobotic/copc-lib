cmake_minimum_required(VERSION 3.15)

file(READ "version" ver)
project(COPCLIB
        LANGUAGES CXX C
        VERSION ${ver})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Find required packages
find_package(pybind11 CONFIG REQUIRED)

set(LIBRARY_TARGET_NAME copc-lib)
# Required C++ versioning
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

add_subdirectory(libs/laz-perf)
include_directories(libs/laz-perf/cpp)
add_subdirectory(cpp)

set(COPC_PYTHON_LIB copclib)

pybind11_add_module(_core python/bindings.cpp)
target_link_libraries(_core PRIVATE copc-lib-s)
install(TARGETS _core DESTINATION .)

    set_target_properties(_core PROPERTIES
                                                        # have the python lib output to the python build dir
                                                        # not the /lib dir
                                                        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                                                        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
enable_testing()
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