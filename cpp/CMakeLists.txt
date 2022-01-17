set(LIBRARY_TARGET_NAME copc-lib)

# Only public header files go here.
set(${LIBRARY_TARGET_NAME}_HDR
        include/${LIBRARY_TARGET_NAME}/copc/info.hpp
        include/${LIBRARY_TARGET_NAME}/copc/extents.hpp
        include/${LIBRARY_TARGET_NAME}/copc/config.hpp
        include/${LIBRARY_TARGET_NAME}/geometry/box.hpp
        include/${LIBRARY_TARGET_NAME}/geometry/vector3.hpp
        include/${LIBRARY_TARGET_NAME}/hierarchy/entry.hpp
        include/${LIBRARY_TARGET_NAME}/hierarchy/key.hpp
        include/${LIBRARY_TARGET_NAME}/hierarchy/node.hpp
        include/${LIBRARY_TARGET_NAME}/hierarchy/page.hpp
        include/${LIBRARY_TARGET_NAME}/io/base_io.hpp
        include/${LIBRARY_TARGET_NAME}/io/reader.hpp
        include/${LIBRARY_TARGET_NAME}/io/writer.hpp
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
        src/copc/info.cpp
        src/copc/extents.cpp
        src/copc/config.cpp
        src/geometry/box.cpp
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
        src/las/vlr.cpp
)

add_library(copc-lib-s STATIC ${${LIBRARY_TARGET_NAME}_SRC} ${${LIBRARY_TARGET_NAME}_HDR})
#add_library(${PROJECT_NAME}::${LIBRARY_TARGET_NAME} ALIAS ${LIBRARY_TARGET_NAME})

set_target_properties(copc-lib-s PROPERTIES VERSION       ${${PROJECT_NAME}_VERSION})
                                                        #PUBLIC_HEADER "${${LIBRARY_TARGET_NAME}_HDR}")

# Specify include directories for both compilation and installation process.
# The $<INSTALL_PREFIX> generator expression is useful to ensure to create
# relocatable configuration files, see https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#creating-relocatable-packages
target_include_directories(copc-lib-s PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                                                         "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")

target_link_libraries(copc-lib-s PRIVATE lazperf_s)
