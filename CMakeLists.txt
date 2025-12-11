cmake_minimum_required(VERSION 3.21)
project(DSRT-Engine-SDK 
    VERSION 1.0.0 
    LANGUAGES CXX
    DESCRIPTION "Professional Game Engine SDK"
)

# Options
option(DSRT_BUILD_SHARED "Build as shared library" ON)
option(DSRT_BUILD_STATIC "Build as static library" OFF)
option(DSRT_BUILD_TESTS "Build tests" ON)
option(DSRT_BUILD_EXAMPLES "Build examples" ON)
option(DSRT_BUILD_DOCS "Build documentation" ON)
option(DSRT_ENABLE_ASSERTS "Enable assertions" ON)
option(DSRT_ENABLE_LOGGING "Enable logging" ON)
option(DSRT_ENABLE_PROFILING "Enable profiling" OFF)
option(DSRT_ENABLE_PHYSICS "Enable physics module" OFF)
option(DSRT_ENABLE_RESOURCES "Enable resource module" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX /MP /EHsc /utf-8)
else()
    add_compile_options(-Wall -Wextra -Werror -Wpedantic -Wconversion -Wshadow -fPIC)
endif()

# Platform detection
if(WIN32)
    add_definitions(-DDSRT_PLATFORM_WINDOWS)
elseif(APPLE)
    add_definitions(-DDSRT_PLATFORM_APPLE)
elseif(UNIX)
    add_definitions(-DDSRT_PLATFORM_LINUX)
endif()

# Add subdirectories
add_subdirectory(DSRT-Core)
add_subdirectory(DSRT-Math)
add_subdirectory(DSRT-Graphics)
add_subdirectory(DSRT-Scene)
add_subdirectory(DSRT-Audio)
add_subdirectory(DSRT-Input)

if(DSRT_ENABLE_RESOURCES)
    add_subdirectory(DSRT-Resource)
endif()

if(DSRT_ENABLE_PHYSICS)
    add_subdirectory(DSRT-Physics)
endif()

add_subdirectory(DSRT-Engine)

# Examples and tests
if(DSRT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(DSRT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
include(GNUInstallDirs)

install(TARGETS DSRT-Engine
    EXPORT DSRTTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/DSRT/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/DSRT
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT DSRTTargets
    FILE DSRTConfig.cmake
    NAMESPACE DSRT::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DSRT
)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/DSRTConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/DSRTConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/DSRTConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DSRT
)
