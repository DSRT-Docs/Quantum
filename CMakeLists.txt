project(DSRT-Engine)

# Source files
set(ENGINE_SOURCES
    src/public/Engine.cpp
    src/public/Core/Log.cpp
    src/public/Core/Memory.cpp
    src/public/Core/Assert.cpp
    src/public/Math/Vector3.cpp
    src/public/Math/Matrix4.cpp
    src/public/Graphics/Renderer.cpp
    src/public/Scene/Entity.cpp
    src/public/Scene/World.cpp
    src/public/Audio/AudioDevice.cpp
    src/public/Input/InputManager.cpp
)

# Create library
if(DSRT_BUILD_SHARED)
    add_library(DSRT-Engine SHARED ${ENGINE_SOURCES})
    set_target_properties(DSRT-Engine PROPERTIES
        OUTPUT_NAME "DSRT"
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
    )
    target_compile_definitions(DSRT-Engine PRIVATE DSRT_BUILD_DLL)
endif()

if(DSRT_BUILD_STATIC)
    add_library(DSRT-Engine_STATIC STATIC ${ENGINE_SOURCES})
    set_target_properties(DSRT-Engine_STATIC PROPERTIES
        OUTPUT_NAME "DSRT"
    )
    target_compile_definitions(DSRT-Engine_STATIC PRIVATE DSRT_STATIC)
endif()

# Include directories
target_include_directories(DSRT-Engine
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/DSRT>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Dependencies
target_link_libraries(DSRT-Engine
    PUBLIC
        Threads::Threads
)

if(WIN32)
    target_link_libraries(DSRT-Engine
        PUBLIC
            winmm
            user32
            gdi32
            shell32
    )
endif()

# Compile definitions
target_compile_definitions(DSRT-Engine PRIVATE
    $<$<BOOL:${DSRT_ENABLE_ASSERTS}>:DSRT_ENABLE_ASSERTIONS>
    $<$<BOOL:${DSRT_ENABLE_LOGGING}>:DSRT_ENABLE_LOGGING>
    $<$<BOOL:${DSRT_ENABLE_PROFILING}>:DSRT_ENABLE_PROFILING>
)
