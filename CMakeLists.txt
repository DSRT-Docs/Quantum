cmake_minimum_required(VERSION 3.15)
project(DSRT VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DSRT_BUILD_EDITOR "Build DSRT Editor" ON)
option(DSRT_BUILD_SAMPLES "Build samples" ON)
option(DSRT_BUILD_TESTS "Build tests" OFF)

set(DSRT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Core)
add_subdirectory(Engine)
add_subdirectory(Editor)
add_subdirectory(Runtime)

if(DSRT_BUILD_SAMPLES)
    add_subdirectory(Samples)
endif()

if(DSRT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()
