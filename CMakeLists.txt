cmake_minimum_required(VERSION 3.0.0)
project(copc-lib VERSION 1.2.0)

find_package(lazperf REQUIRED)

set (COPC_SHARED_LIB copc-lib)
set (COPC_STATIC_LIB copc-lib-s)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Path to additional CMake modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

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
    include(${CMAKE_SOURCE_DIR}/cmake/helpers.cmake NO_POLICY_SCOPE)

    download_test_files("${CMAKE_BINARY_DIR}/autzen-classified.copc.laz")
    set(BaseAutzenFilePath "${CMAKE_BINARY_DIR}/autzen-classified.copc.laz")
endif()

if(WITH_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if(WITH_EXAMPLES OR WITH_TESTS)
    add_subdirectory(example)
    if(WITH_TESTS)
        add_test(NAME example_reader COMMAND example_reader)
        add_test(NAME example_writer COMMAND example_writer)
    endif()
endif()

if(WITH_PYTHON)
    add_subdirectory(python)
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
