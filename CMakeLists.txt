cmake_minimum_required(VERSION 3.15)

# ref https://github.com/robotology/how-to-export-cpp-library/blob/master/CMakeLists.txt
project(COPCLIB
        LANGUAGES CXX C
        VERSION 1.3.0)

# Defines the CMAKE_INSTALL_LIBDIR, CMAKE_INSTALL_BINDIR and many other useful macros.
# See https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html
include(GNUInstallDirs)

### Options
# Shared/Dynamic or Static library?
option(BUILD_SHARED_LIBS "Build libraries as shared as opposed to static" ON)

option(WITH_TESTS_AND_EXAMPLES "Build test and example files." OFF)
option(WITH_PYTHON "Build python bindings." OFF)
option(ONLY_PYTHON "Build only python bindings, for setup.py env" OFF)

if (ONLY_PYTHON)
    set(WITH_PYTHON ON)
    set(WITH_TESTS_AND_EXAMPLES OFF)
    set(BUILD_SHARED_LIBS ON)
endif()

# Control where libraries and executables are placed during the build.
# With the following settings executables are placed in <the top level of the
# build tree>/bin and libraries/archives in <top level of the build tree>/lib.
# This is particularly useful to run ctests on libraries built on Windows
# machines: tests, which are executables, are placed in the same folders of
# dlls, which are treated as executables as well, so that they can properly
# find the libraries to run. This is a because of missing RPATH on Windows.

# setup.py already sets CMAKE_LIBRARY_OUTPUT_DIRECTORY, we don't want to override it
if (NOT ONLY_PYTHON)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

    # For windows, remove the Release/ and Debug/ pathes
    foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
        string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
        set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
        set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )
        set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
    endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
endif()

# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
# See https://cmake.org/cmake/help/v3.4/variable/CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS.html
# See https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Under MSVC, we set CMAKE_DEBUG_POSTFIX to "d" to add a trailing "d" to library
# built in debug mode. In this Windows user can compile, build and install the
# library in both Release and Debug configuration avoiding naming clashes in the
# installation directories.
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

# Required C++ versioning
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# We use
# - InstallBasicPackageFiles (http://robotology.github.io/ycm/gh-pages/v0.8/module/InstallBasicPackageFiles.html)
# - AddUninstallTarget (http://robotology.github.io/ycm/gh-pages/v0.8/module/AddUninstallTarget.html)
# - AddInstallRPATHSupport (http://robotology.github.io/ycm/gh-pages/v0.8/module/AddInstallRPATHSupport.html)
# from YCM. Files are under the cmake subdirectory.
# See https://github.com/robotology/ycm/
# If you don't want to ship these files with your code (recommended), you can
# instead depend on YCM.
# In this case replace the following line with
# find_package(YCM REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(WITH_TESTS_AND_EXAMPLES)
    enable_testing()
endif()

# Find required packages
find_package(LAZPERF 2.1.0 REQUIRED)

# Enable RPATH support for installed binaries and libraries
include(AddInstallRPATHSupport)
add_install_rpath_support(BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"
                          LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"
                          INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
                          USE_LINK_PATH)

### Compile- and install-related commands.
add_subdirectory(cpp)

# Create and install CMake configuration files for your project that are
# necessary to for other projects to call find_package(LibTemplateCMake).
#
# Note that it is extremely important to use exactly the project name while
# installing configuration files (you can use PROJECT_NAME variable to avoid
# any possible error). This is required to allow find_package() to properly
# look for the installed library in system path, in particular in Windows when
# the installation is performed in the default path.
#
# install_basic_package_files() comes with many input parameters to customize
# the configuration files. The parameters used in the following call provide
# basic versions of CMake configuration files.
# See install_basic_package_files() documentation found in ./cmake folder.
#
# Note that if your library depends from other libraries, you are probably
# required to used the install_basic_package_files() DEPENDENCIES option.
include(InstallBasicPackageFiles)
install_basic_package_files(${PROJECT_NAME}
                            VERSION ${${PROJECT_NAME}_VERSION}
                            COMPATIBILITY AnyNewerVersion
                            VARS_PREFIX ${PROJECT_NAME}
                            DEPENDENCIES "LAZPERF 2.1.0 REQUIRED"
                            FIRST_TARGET copc-lib
                            NO_CHECK_REQUIRED_COMPONENTS_MACRO)
# Add the uninstall target
include(AddUninstallTarget)

# Build Submodules
if (WITH_TESTS_AND_EXAMPLES)
    add_subdirectory(test)
    add_subdirectory(example)
endif()

if(WITH_PYTHON)
    add_subdirectory(python)
endif()
