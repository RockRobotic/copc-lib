set(LIBRARY_TARGET_NAME copc-lib)

# Only public header files go here.
set(${LIBRARY_TARGET_NAME}_HDR
        include/${LIBRARY_TARGET_NAME}/copc/info.hpp
        include/${LIBRARY_TARGET_NAME}/copc/extents.hpp
        include/${LIBRARY_TARGET_NAME}/copc/copc_config.hpp
        include/${LIBRARY_TARGET_NAME}/geometry/box.hpp
        include/${LIBRARY_TARGET_NAME}/geometry/vector3.hpp
        include/${LIBRARY_TARGET_NAME}/hierarchy/entry.hpp
        include/${LIBRARY_TARGET_NAME}/hierarchy/key.hpp
        include/${LIBRARY_TARGET_NAME}/hierarchy/node.hpp
        include/${LIBRARY_TARGET_NAME}/hierarchy/page.hpp
        include/${LIBRARY_TARGET_NAME}/io/base_reader.hpp
        include/${LIBRARY_TARGET_NAME}/io/copc_base_io.hpp
        include/${LIBRARY_TARGET_NAME}/io/copc_reader.hpp
        include/${LIBRARY_TARGET_NAME}/io/copc_writer.hpp
        include/${LIBRARY_TARGET_NAME}/io/laz_writer.hpp
        include/${LIBRARY_TARGET_NAME}/io/laz_reader.hpp
        include/${LIBRARY_TARGET_NAME}/las/header.hpp
        include/${LIBRARY_TARGET_NAME}/io/laz_base_writer.hpp
        include/${LIBRARY_TARGET_NAME}/las/point.hpp
        include/${LIBRARY_TARGET_NAME}/las/points.hpp
        include/${LIBRARY_TARGET_NAME}/las/utils.hpp
        include/${LIBRARY_TARGET_NAME}/las/vlr.hpp
        include/${LIBRARY_TARGET_NAME}/las/laz_config.hpp
        include/${LIBRARY_TARGET_NAME}/laz/compressor.hpp
        include/${LIBRARY_TARGET_NAME}/laz/decompressor.hpp
)

# All source files and private header files go here.
set(${LIBRARY_TARGET_NAME}_SRC
        include/${LIBRARY_TARGET_NAME}/hierarchy/internal/page.hpp
        include/${LIBRARY_TARGET_NAME}/hierarchy/internal/hierarchy.hpp
        include/${LIBRARY_TARGET_NAME}/io/internal/copc_writer_internal.hpp
        src/copc/info.cpp
        src/copc/extents.cpp
        src/copc/copc_config.cpp
        src/geometry/box.cpp
        src/hierarchy/key.cpp
        src/hierarchy/page.cpp
        src/io/base_reader.cpp
        src/io/copc_base_io.cpp
        src/io/copc_reader.cpp
        src/io/copc_writer_internal.cpp
        src/io/copc_writer_public.cpp
        src/io/laz_base_writer.cpp
        src/io/laz_writer.cpp
        src/io/laz_reader.cpp
        src/las/header.cpp
        src/las/point.cpp
        src/las/points.cpp
        src/las/utils.cpp
        src/las/vlr.cpp
        src/las/laz_config.cpp
)

# Compile static library for pip wheels
if (WITH_PYTHON)
    add_library(${LIBRARY_TARGET_NAME}-s STATIC ${${LIBRARY_TARGET_NAME}_SRC} ${${LIBRARY_TARGET_NAME}_HDR})
    set_target_properties(${LIBRARY_TARGET_NAME}-s PROPERTIES VERSION       ${${PROJECT_NAME}_VERSION})
    target_include_directories(${LIBRARY_TARGET_NAME}-s PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                                         "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")
    if (LAZPERF_IS_FOUND)
        target_link_libraries(${LIBRARY_TARGET_NAME}-s PRIVATE lazperf_s)
    else()
        target_link_libraries(${LIBRARY_TARGET_NAME}-s PRIVATE ${LAZPERF_LIB_NAME})
    endif()
    message(STATUS "Created target ${LIBRARY_TARGET_NAME}-s for export ${PROJECT_NAME}.")
endif()

if (NOT SKBUILD)
    add_library(${LIBRARY_TARGET_NAME} ${${LIBRARY_TARGET_NAME}_SRC} ${${LIBRARY_TARGET_NAME}_HDR})
    add_library(${PROJECT_NAME}::${LIBRARY_TARGET_NAME} ALIAS ${LIBRARY_TARGET_NAME})

    set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES VERSION       ${${PROJECT_NAME}_VERSION})

    # Specify include directories for both compilation and installation process.
    # The $<INSTALL_PREFIX> generator expression is useful to ensure to create
    # relocatable configuration files, see https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#creating-relocatable-packages
    target_include_directories(${LIBRARY_TARGET_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                                                "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")

    target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC ${LAZPERF_LIB_NAME})

    # Specify installation targets, typology and destination folders.
    install(TARGETS ${LIBRARY_TARGET_NAME} ${EXTRA_EXPORT_TARGETS}
            EXPORT  ${PROJECT_NAME}
            LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                            COMPONENT shlib
            ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                            COMPONENT lib
            RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}"                            COMPONENT bin)
            # PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_TARGET_NAME}" COMPONENT dev)

    # Keep header file hierarchy
    install(DIRECTORY "include/${LIBRARY_TARGET_NAME}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    message(STATUS "Created target ${LIBRARY_TARGET_NAME} for export ${PROJECT_NAME}.")
endif()
