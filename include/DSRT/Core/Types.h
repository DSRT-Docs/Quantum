/**
 * @file Types.h
 * @brief Fundamental types and aliases for DSRT Engine
 * @ingroup Core
 */

#pragma once
#include "Export.h"
#include <cstdint>
#include <memory>

namespace DSRT {

/**
 * @brief 64-bit unique identifier for engine objects
 * @details Used for entities, components, resources, etc.
 */
using Handle = uint64_t;

/**
 * @brief Constant representing an invalid handle
 * @details Returned when operations fail to create valid handles
 */
constexpr Handle InvalidHandle = 0xFFFFFFFFFFFFFFFF;

/**
 * @enum Result
 * @brief Operation result codes
 * @details All public API functions return Result where appropriate
 */
enum class Result {
    Success,            /**< Operation completed successfully */
    Failure,            /**< General failure (unspecified) */
    InvalidParameter,   /**< Invalid parameter passed to function */
    OutOfMemory,        /**< Memory allocation failed */
    NotInitialized,     /**< System not yet initialized */
    AlreadyInitialized, /**< System already initialized */
    FileNotFound,       /**< Requested file not found */
    Unsupported         /**< Feature not supported on current platform */
};

/**
 * @brief Shared pointer alias for DSRT objects
 * @tparam T Type of object
 * @details Use for objects with shared ownership semantics
 */
template<typename T>
using Ref = std::shared_ptr<T>;

/**
 * @brief Weak pointer alias for DSRT objects
 * @tparam T Type of object
 * @details Use for non-owning references to shared objects
 */
template<typename T>
using WeakRef = std::weak_ptr<T>;

/**
 * @brief Unique pointer alias for DSRT objects
 * @tparam T Type of object
 * @details Use for objects with exclusive ownership
 */
template<typename T>
using UniqueRef = std::unique_ptr<T>;

/**
 * @enum Platform
 * @brief Supported platforms
 * @details Detected at compile time
 */
enum class Platform {
    Windows,    /**< Microsoft Windows */
    Linux,      /**< Linux distributions */
    macOS,      /**< Apple macOS */
    Android,    /**< Android mobile OS */
    iOS,        /**< Apple iOS */
    Unknown     /**< Unknown/unsupported platform */
};

/**
 * @enum GraphicsAPI
 * @brief Supported graphics APIs
 * @details Selected at runtime based on platform and configuration
 */
enum class GraphicsAPI {
    OpenGL,     /**< OpenGL 4.3+ */
    Vulkan,     /**< Vulkan 1.2+ */
    Direct3D11, /**< Direct3D 11 */
    Direct3D12, /**< Direct3D 12 */
    Metal,      /**< Apple Metal */
    None        /**< No graphics API (headless mode) */
};

} // namespace DSRT
