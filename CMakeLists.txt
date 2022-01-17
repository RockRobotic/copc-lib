cmake_minimum_required(VERSION 3.15)

file(READ "version" ver)
project(COPCLIB
        LANGUAGES CXX C
        VERSION ${ver})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
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

pybind11_add_module(${COPC_PYTHON_LIB} python/bindings.cpp)
target_link_libraries(${COPC_PYTHON_LIB} PRIVATE copc-lib-s)
install(TARGETS ${COPC_PYTHON_LIB} DESTINATION .)