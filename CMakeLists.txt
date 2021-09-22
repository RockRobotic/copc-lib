cmake_minimum_required(VERSION 3.0.0)
project(copc-lib VERSION 1.2.0)

find_package(lazperf REQUIRED)

set (COPC_SHARED_LIB copc-lib)
set (COPC_STATIC_LIB copc-lib-s)
set (COPC_PYTHON_LIB copclib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

file(GLOB_RECURSE SRCS
        src/*.cpp
        )

if (NOT EMSCRIPTEN)
    add_library(${COPC_SHARED_LIB} SHARED ${SRCS})
    target_link_libraries(${COPC_SHARED_LIB} LAZPERF::lazperf)
    target_include_directories(${COPC_SHARED_LIB} INTERFACE $<INSTALL_INTERFACE:include>)
endif()

add_library(${COPC_STATIC_LIB} STATIC ${SRCS})
target_link_libraries(${COPC_STATIC_LIB} LAZPERF::lazperf)

# Choose package components
set(WITH_TESTS TRUE CACHE BOOL "Choose if unit tests should be built")
set(WITH_EXAMPLES TRUE CACHE BOOL "Choose if examples should be built")
set(WITH_PYTHON TRUE CACHE BOOL "Choose if python bindings should be built")

if (WITH_TESTS OR WITH_EXAMPLES OR WITH_PYTHON)
    set(AutzenFilePath "${CMAKE_BINARY_DIR}/test/data/autzen-classified.copc.laz")
    if(NOT EXISTS ${AutzenFilePath})
        message(STATUS "Downloading test files")
        file (DOWNLOAD
        "https://github.com/PDAL/data/raw/62e514b6484ec59cd48bb48d5c6fe8a00216a6ac/autzen/autzen-classified.copc.laz"
        ${AutzenFilePath}
        )
    endif()

    add_executable(example_reader "example/example-reader.cpp")
    target_link_libraries(example_reader ${COPC_SHARED_LIB})
    add_executable(example_writer "example/example-writer.cpp")
    target_link_libraries(example_writer ${COPC_SHARED_LIB})
endif()

if(WITH_TESTS)
    add_subdirectory(lib/Catch2)

    file(GLOB TEST_SRCS
            test/*.cpp
            )

    add_executable(tests ${TEST_SRCS})
    target_link_libraries(tests ${COPC_SHARED_LIB} Catch2::Catch2)

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/lib/Catch2/contrib")
    include(CTest)
    include(Catch)
    catch_discover_tests(tests)

    add_test(NAME example_reader COMMAND example_reader)
    add_test(NAME example_writer COMMAND example_writer)
endif()

if(WITH_PYTHON)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/test/data/autzen-classified.copc.laz")
        message(STATUS "Copying test files")
        file(COPY "${AutzenFilePath}" 
            DESTINATION "${CMAKE_SOURCE_DIR}/test/data")
    endif()

    add_subdirectory(lib/pybind11)
    pybind11_add_module(${COPC_PYTHON_LIB} ${SRCS} python/bindings.cpp)
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
    endif()
endif()

# Install
set(include_dest "include/copc-lib")
set(lib_dest "lib/cmake/copc-lib")

if (NOT EMSCRIPTEN)

    ## Targets
    install(TARGETS ${COPC_SHARED_LIB} EXPORT ${COPC_SHARED_LIB} LIBRARY DESTINATION lib)
    ## Export
    install(FILES cmake/copc-lib-config.cmake DESTINATION ${lib_dest})
    install(EXPORT ${COPC_SHARED_LIB} DESTINATION "${lib_dest}")
    ## Headers
    file(GLOB_RECURSE HDRS
            include/*.hpp
            )
    ### Reproduce the folder hierarchy of include/* in destination
    foreach(HDR IN LISTS HDRS)
        get_filename_component(FILE_DIR ${HDR} DIRECTORY)
        file(RELATIVE_PATH REL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include" "${FILE_DIR}")
        install(FILES ${HDR} DESTINATION "include/${REL_PATH}")
    endforeach()

endif()
