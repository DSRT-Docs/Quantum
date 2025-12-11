#pragma once

/**
 * @file DSRTCore.h
 * @brief Core engine definitions and macros
 */

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define DSRT_PLATFORM_WINDOWS 1
#elif defined(__linux__)
    #define DSRT_PLATFORM_LINUX 1
#elif defined(__ANDROID__)
    #define DSRT_PLATFORM_ANDROID 1
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
        #define DSRT_PLATFORM_IOS 1
    #else
        #define DSRT_PLATFORM_MACOS 1
    #endif
#endif

// Compiler detection
#if defined(_MSC_VER)
    #define DSRT_COMPILER_MSVC 1
#elif defined(__GNUC__)
    #define DSRT_COMPILER_GCC 1
#elif defined(__clang__)
    #define DSRT_COMPILER_CLANG 1
#endif

// Architecture detection
#if defined(__x86_64__) || defined(_M_X64)
    #define DSRT_ARCH_X64 1
#elif defined(__i386__) || defined(_M_IX86)
    #define DSRT_ARCH_X86 1
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define DSRT_ARCH_ARM64 1
#elif defined(__arm__) || defined(_M_ARM)
    #define DSRT_ARCH_ARM 1
#endif

// Debug/Release configuration
#if defined(NDEBUG)
    #define DSRT_CONFIG_RELEASE 1
    #define DSRT_CONFIG_DEBUG 0
#else
    #define DSRT_CONFIG_DEBUG 1
    #define DSRT_CONFIG_RELEASE 0
#endif

// DLL export/import
#if DSRT_PLATFORM_WINDOWS
    #ifdef DSRT_ENGINE_EXPORT
        #define DSRT_API __declspec(dllexport)
    #else
        #define DSRT_API __declspec(dllimport)
    #endif
#else
    #define DSRT_API __attribute__((visibility("default")))
#endif

// Force inline
#if DSRT_COMPILER_MSVC
    #define DSRT_FORCE_INLINE __forceinline
#else
    #define DSRT_FORCE_INLINE __attribute__((always_inline)) inline
#endif

// Align macro
#define DSRT_ALIGN(x) alignas(x)

// Assertion
#if DSRT_CONFIG_DEBUG
    #include <cassert>
    #define DSRT_ASSERT(expr) assert(expr)
    #define DSRT_ASSERT_MSG(expr, msg) assert((expr) && (msg))
#else
    #define DSRT_ASSERT(expr) ((void)0)
    #define DSRT_ASSERT_MSG(expr, msg) ((void)0)
#endif

// Unused parameter
#define DSRT_UNUSED(x) (void)(x)

// Bit manipulation
#define DSRT_BIT(x) (1 << (x))
#define DSRT_FLAG_CHECK(flags, bit) (((flags) & (bit)) == (bit))
#define DSRT_FLAG_SET(flags, bit) ((flags) |= (bit))
#define DSRT_FLAG_CLEAR(flags, bit) ((flags) &= ~(bit))
#define DSRT_FLAG_TOGGLE(flags, bit) ((flags) ^= (bit))

// Memory alignment
#define DSRT_ALIGNED_MALLOC(size, alignment) \
    _aligned_malloc((size), (alignment))
#define DSRT_ALIGNED_FREE(ptr) \
    _aligned_free(ptr)

// Stringify
#define DSRT_STRINGIFY(x) #x
#define DSRT_TO_STRING(x) DSRT_STRINGIFY(x)

// Version macros
#define DSRT_VERSION_MAJOR 1
#define DSRT_VERSION_MINOR 0
#define DSRT_VERSION_PATCH 0
#define DSRT_VERSION_STRING \
    DSRT_TO_STRING(DSRT_VERSION_MAJOR) "." \
    DSRT_TO_STRING(DSRT_VERSION_MINOR) "." \
    DSRT_TO_STRING(DSRT_VERSION_PATCH)

// Engine namespace
namespace DSRT {
    
    /**
     * @brief Initializes the engine core systems
     * @return True if initialization succeeded
     */
    DSRT_API bool InitializeCore();
    
    /**
     * @brief Shuts down the engine core systems
     */
    DSRT_API void ShutdownCore();
    
    /**
     * @brief Gets the engine version string
     * @return Version string in format "major.minor.patch"
     */
    DSRT_API const char* GetEngineVersion();
    
    /**
     * @brief Gets the engine build configuration
     * @return "Debug" or "Release"
     */
    DSRT_API const char* GetBuildConfiguration();
    
    /**
     * @brief Gets the platform name
     * @return Platform name string
     */
    DSRT_API const char* GetPlatformName();
    
    /**
     * @brief Gets the compiler name
     * @return Compiler name string
     */
    DSRT_API const char* GetCompilerName();
    
} // namespace DSRT
