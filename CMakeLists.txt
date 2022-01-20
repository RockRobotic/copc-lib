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