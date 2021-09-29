set(LIBRARY_TARGET_NAME copc-lib)

# Only public header files go here.
set(${LIBRARY_TARGET_NAME}_HDR
        include/${LIBRARY_TARGET_NAME}/copc/file.hpp
        include/${LIBRARY_TARGET_NAME}/hierarchy/entry.hpp
        include/${LIBRARY_TARGET_NAME}/hierarchy/key.hpp
        include/${LIBRARY_TARGET_NAME}/hierarchy/node.hpp
        include/${LIBRARY_TARGET_NAME}/hierarchy/page.hpp
        include/${LIBRARY_TARGET_NAME}/io/base_io.hpp
        include/${LIBRARY_TARGET_NAME}/io/reader.hpp
        include/${LIBRARY_TARGET_NAME}/io/writer.hpp
        include/${LIBRARY_TARGET_NAME}/las/file.hpp
        include/${LIBRARY_TARGET_NAME}/las/header.hpp
        include/${LIBRARY_TARGET_NAME}/las/point.hpp
        include/${LIBRARY_TARGET_NAME}/las/points.hpp
        include/${LIBRARY_TARGET_NAME}/las/utils.hpp
        include/${LIBRARY_TARGET_NAME}/las/vlr.hpp
        include/${LIBRARY_TARGET_NAME}/laz/compressor.hpp
        include/${LIBRARY_TARGET_NAME}/laz/decompressor.hpp
)

# All source files and private header files go here.
set(${LIBRARY_TARGET_NAME}_SRC
        include/${LIBRARY_TARGET_NAME}/hierarchy/internal/page.hpp
        include/${LIBRARY_TARGET_NAME}/hierarchy/internal/hierarchy.hpp
        include/${LIBRARY_TARGET_NAME}/io/internal/writer_internal.hpp
        src/hierarchy/key.cpp
        src/hierarchy/page.cpp
        src/io/base_io.cpp
        src/io/reader.cpp
        src/io/writer_internal.cpp
        src/io/writer_public.cpp
        src/las/header.cpp
        src/las/point.cpp
        src/las/points.cpp
        src/las/utils.cpp
)

add_library(${LIBRARY_TARGET_NAME} ${${LIBRARY_TARGET_NAME}_SRC} ${${LIBRARY_TARGET_NAME}_HDR})
add_library(${PROJECT_NAME}::${LIBRARY_TARGET_NAME} ALIAS ${LIBRARY_TARGET_NAME})

set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES VERSION       ${${PROJECT_NAME}_VERSION})
                                                        #PUBLIC_HEADER "${${LIBRARY_TARGET_NAME}_HDR}")

# Specify include directories for both compilation and installation process.
# The $<INSTALL_PREFIX> generator expression is useful to ensure to create
# relocatable configuration files, see https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#creating-relocatable-packages
target_include_directories(${LIBRARY_TARGET_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                                         "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC LAZPERF::lazperf)

# Specify installation targets, typology and destination folders.
install(TARGETS ${LIBRARY_TARGET_NAME}
        EXPORT  ${PROJECT_NAME}
        LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                            COMPONENT shlib
        ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                            COMPONENT lib
        RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}"                            COMPONENT bin)
        # PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_TARGET_NAME}" COMPONENT dev)

# Keep header file hierarchy
install(DIRECTORY "include/${LIBRARY_TARGET_NAME}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

message(STATUS "Created target ${LIBRARY_TARGET_NAME} for export ${PROJECT_NAME}.")