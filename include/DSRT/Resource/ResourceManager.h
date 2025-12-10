/**
 * @file ResourceManager.h
 * @brief Asset loading and management system
 * @ingroup Resource
 * 
 * @details Provides unified interface for loading, caching, and managing
 *          game assets (textures, meshes, shaders, audio, etc.).
 */

#pragma once
#include "../Export.h"
#include "../Core/Types.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

namespace DSRT {
namespace Resource {

/**
 * @enum ResourceType
 * @brief Resource type identifiers
 * @ingroup Resource
 */
enum class ResourceType {
    Unknown,
    Texture,        /**< 2D texture */
    TextureCube,    /**< Cube map texture */
    Mesh,           /**< 3D mesh */
    Material,       /**< Material definition */
    Shader,         /**< Shader program */
    ShaderProgram,  /**< Linked shader program */
    Audio,          /**< Audio sample */
    Font,           /**< Font face */
    Animation,      /**< Animation data */
    Skeleton,       /**< Skeleton/bone hierarchy */
    Scene,          /**< Scene file */
    Script,         /**< Script file */
    Data,           /**< Generic binary data */
    
    Count
};

/**
 * @struct ResourceHandle
 * @brief Handle to loaded resource
 * @ingroup Resource
 */
struct DSRT_API ResourceHandle {
    uint64_t id = 0;                    /**< Unique resource ID */
    ResourceType type = ResourceType::Unknown; /**< Resource type */
    std::string path;                   /**< Source file path */
    size_t size = 0;                    /**< Memory size in bytes */
    uint32_t refCount = 0;              /**< Reference count */
    void* data = nullptr;               /**< Resource data pointer */
    
    /**
     * @brief Check if handle is valid
     * @return true if handle points to loaded resource
     */
    bool IsValid() const { return id != 0 && data != nullptr; }
    
    /**
     * @brief Check if resource is of specific type
     * @tparam T Expected resource type
     * @return true if type matches
     */
    template<typename T>
    bool IsType() const;
    
    /**
     * @brief Cast resource to specific type
     * @tparam T Target type
     * @return Typed pointer, or nullptr if type mismatch
     */
    template<typename T>
    T* As() const;
};

/**
 * @struct ResourceLoadParams
 * @brief Resource loading parameters
 * @ingroup Resource
 */
struct DSRT_API ResourceLoadParams {
    bool async = false;                 /**< Load asynchronously */
    bool cache = true;                  /**< Cache loaded resource */
    bool reloadable = true;             /**< Allow hot reloading */
    uint32_t priority = 0;              /**< Loading priority */
    void* userData = nullptr;           /**< User-defined data */
    
    // Type-specific parameters
    union {
        struct {
            bool generateMipmaps;       /**< Generate mipmap chain */
            bool sRGB;                  /**< sRGB color space */
            uint32_t anisotropy;        /**< Anisotropic filtering level */
        } texture;
        
        struct {
            bool optimize;              /**< Optimize mesh data */
            bool calculateTangents;     /**< Calculate tangent space */
            bool mergeSubmeshes;        /**< Merge submeshes */
        } mesh;
        
        struct {
            bool preprocess;            /**< Preprocess shader source */
            bool validate;              /**< Validate shader */
        } shader;
    } params;                           /**< Type-specific parameters */
};

/**
 * @struct ResourceStats
 * @brief Resource manager statistics
 * @ingroup Resource
 */
struct DSRT_API ResourceStats {
    size_t totalMemory = 0;             /**< Total allocated memory */
    size_t cachedMemory = 0;            /**< Memory used by cache */
    uint32_t loadedCount = 0;           /**< Number of loaded resources */
    uint32_t cachedCount = 0;           /**< Number of cached resources */
    uint32_t pendingLoads = 0;          /**< Pending async loads */
    uint32_t failedLoads = 0;           /**< Failed load attempts */
    uint32_t hotReloads = 0;            /**< Hot reload operations */
};

/**
 * @class ResourceManager
 * @brief Central asset loading and management system
 * @ingroup Resource
 * 
 * @details Provides synchronous and asynchronous asset loading with caching,
 *          hot reloading, and dependency tracking.
 * 
 * @example Loading resources:
 * @code
 * ResourceManager& resources = GetResourceManager();
 * 
 * // Load texture synchronously
 * auto texture = resources.Load<Texture>("assets/textures/wood.jpg");
 * 
 * // Load mesh asynchronously
 * resources.LoadAsync<Mesh>("assets/models/character.fbx",
 *     [](Mesh* mesh) {
 *         if (mesh) {
 *             character->SetMesh(mesh);
 *         }
 *     }
 * );
 * 
 * // Get cached resource
 * auto cached = resources.Get<Texture>("assets/textures/wood.jpg");
 * @endcode
 * 
 * @example Hot reloading:
 * @code
 * // Watch file for changes
 * resources.WatchFile("assets/shaders/lighting.glsl",
 *     [](const std::string& path) {
 *         // File changed - reload
 *         resources.Reload<Shader>(path);
 *     }
 * );
 * @endcode
 */
class DSRT_API ResourceManager {
public:
    /**
     * @brief Create resource manager
     * @return Shared pointer to resource manager
     */
    static Ref<ResourceManager> Create();
    
    virtual ~ResourceManager() = default;
    
    /**
     * @brief Initialize resource manager
     * @param assetRoot Root directory for assets
     * @return true if initialization succeeded
     */
    virtual bool Initialize(const std::string& assetRoot = "assets") = 0;
    
    /**
     * @brief Shutdown resource manager
     */
    virtual void Shutdown() = 0;
    
    /**
     * @brief Update resource manager
     * @param deltaTime Time since last update
     * 
     * @details Processes async loads, hot reloads, and cleanup.
     */
    virtual void Update(float deltaTime) = 0;
    
    // Resource loading
    
    /**
     * @brief Load resource synchronously
     * @tparam T Resource type
     * @param path Resource path (relative to asset root)
     * @param params Loading parameters
     * @return Resource handle, or invalid handle on failure
     * 
     * @warning Blocks until resource is loaded.
     *          Use LoadAsync() for non-blocking loads.
     */
    template<typename T>
    ResourceHandle Load(const std::string& path, const ResourceLoadParams& params = ResourceLoadParams());
    
    /**
     * @brief Load resource asynchronously
     * @tparam T Resource type
     * @param path Resource path
     * @param callback Callback when loading completes
     * @param params Loading parameters
     * @return Loading task ID, or 0 on failure
     */
    template<typename T>
    uint64_t LoadAsync(const std::string& path, 
                      std::function<void(ResourceHandle)> callback,
                      const ResourceLoadParams& params = ResourceLoadParams());
    
    /**
     * @brief Get cached resource
     * @tparam T Resource type
     * @param path Resource path
     * @return Resource handle, or invalid handle if not cached
     */
    template<typename T>
    ResourceHandle Get(const std::string& path) const;
    
    /**
     * @brief Get resource by ID
     * @tparam T Resource type
     * @param id Resource ID
     * @return Resource handle, or invalid handle if not found
     */
    template<typename T>
    ResourceHandle Get(uint64_t id) const;
    
    /**
     * @brief Check if resource is loaded
     * @param path Resource path
     * @return true if resource is in cache
     */
    virtual bool IsLoaded(const std::string& path) const = 0;
    
    /**
     * @brief Check if resource is loading
     * @param path Resource path
     * @return true if resource is being loaded
     */
    virtual bool IsLoading(const std::string& path) const = 0;
    
    /**
     * @brief Reload resource from disk
     * @tparam T Resource type
     * @param path Resource path
     * @return New resource handle
     */
    template<typename T>
    ResourceHandle Reload(const std::string& path);
    
    /**
     * @brief Unload resource
     * @param path Resource path
     * @return true if resource was unloaded
     * 
     * @details Decrements reference count. Resource is actually
     *          unloaded when refCount reaches zero.
     */
    virtual bool Unload(const std::string& path) = 0;
    
    /**
     * @brief Unload resource by ID
     * @param id Resource ID
     * @return true if resource was unloaded
     */
    virtual bool Unload(uint64_t id) = 0;
    
    /**
     * @brief Unload all resources of type
     * @param type Resource type
     * @return Number of resources unloaded
     */
    virtual uint32_t UnloadAll(ResourceType type) = 0;
    
    /**
     * @brief Unload all resources
     * @return Total number of resources unloaded
     */
    virtual uint32_t UnloadAll() = 0;
    
    // Resource tracking
    
    /**
     * @brief Increment resource reference count
     * @param handle Resource handle
     * @return New reference count
     */
    virtual uint32_t AddRef(const ResourceHandle& handle) = 0;
    
    /**
     * @brief Decrement resource reference count
     * @param handle Resource handle
     * @return New reference count
     */
    virtual uint32_t Release(const ResourceHandle& handle) = 0;
    
    /**
     * @brief Get resource reference count
     * @param handle Resource handle
     * @return Current reference count
     */
    virtual uint32_t GetRefCount(const ResourceHandle& handle) const = 0;
    
    // File watching
    
    /**
     * @brief Watch file for changes
     * @param path File path to watch
     * @param callback Function to call when file changes
     * @return Watch ID, or 0 on failure
     */
    virtual uint64_t WatchFile(const std::string& path, 
                               std::function<void(const std::string&)> callback) = 0;
    
    /**
     * @brief Stop watching file
     * @param watchId Watch ID from WatchFile()
     * @return true if watch was stopped
     */
    virtual bool UnwatchFile(uint64_t watchId) = 0;
    
    /**
     * @brief Check if file is being watched
     * @param path File path
     * @return true if file is watched
     */
    virtual bool IsFileWatched(const std::string& path) const = 0;
    
    // Search paths
    
    /**
     * @brief Add search path
     * @param path Directory path to search
     * @param priority Search priority (higher = searched first)
     * @return true if path was added
     */
    virtual bool AddSearchPath(const std::string& path, int priority = 0) = 0;
    
    /**
     * @brief Remove search path
     * @param path Directory path to remove
     * @return true if path was removed
     */
    virtual bool RemoveSearchPath(const std::string& path) = 0;
    
    /**
     * @brief Clear all search paths
     */
    virtual void ClearSearchPaths() = 0;
    
    /**
     * @brief Find file in search paths
     * @param filename File to find
     * @return Full path if found, empty string otherwise
     */
    virtual std::string FindFile(const std::string& filename) const = 0;
    
    // Statistics
    
    /**
     * @brief Get resource statistics
     * @return Current statistics
     */
    virtual ResourceStats GetStats() const = 0;
    
    /**
     * @brief Print resource statistics to log
     */
    virtual void PrintStats() const = 0;
    
    /**
     * @brief Get memory usage by resource type
     * @param type Resource type
     * @return Memory usage in bytes
     */
    virtual size_t GetMemoryUsage(ResourceType type) const = 0;
    
    // Utility
    
    /**
     * @brief Get resource path from ID
     * @param id Resource ID
     * @return Resource path, or empty string if not found
     */
    virtual std::string GetResourcePath(uint64_t id) const = 0;
    
    /**
     * @brief Get resource ID from path
     * @param path Resource path
     * @return Resource ID, or 0 if not loaded
     */
    virtual uint64_t GetResourceId(const std::string& path) const = 0;
    
    /**
     * @brief Register resource type loader
     * @tparam T Resource type
     * @param loader Loader function
     * @return true if loader was registered
     */
    template<typename T>
    bool RegisterLoader(std::function<ResourceHandle(const std::string&, const ResourceLoadParams&)> loader);
    
    /**
     * @brief Unregister resource type loader
     * @tparam T Resource type
     * @return true if loader was unregistered
     */
    template<typename T>
    bool UnregisterLoader();
    
protected:
    ResourceManager() = default;
    
private:
    // Internal implementation
    template<typename T>
    ResourceHandle LoadImpl(const std::string& path, const ResourceLoadParams& params);
    
    template<typename T>
    uint64_t LoadAsyncImpl(const std::string& path, 
                          std::function<void(ResourceHandle)> callback,
                          const ResourceLoadParams& params);
    
    template<typename T>
    ResourceHandle GetImpl(const std::string& path) const;
    
    template<typename T>
    ResourceHandle GetImpl(uint64_t id) const;
    
    template<typename T>
    ResourceHandle ReloadImpl(const std::string& path);
    
    template<typename T>
    bool RegisterLoaderImpl(std::function<ResourceHandle(const std::string&, const ResourceLoadParams&)> loader);
    
    template<typename T>
    bool UnregisterLoaderImpl();
};

// Template implementations

template<typename T>
inline ResourceHandle ResourceManager::Load(const std::string& path, const ResourceLoadParams& params) {
    return LoadImpl<T>(path, params);
}

template<typename T>
inline uint64_t ResourceManager::LoadAsync(const std::string& path, 
                                         std::function<void(ResourceHandle)> callback,
                                         const ResourceLoadParams& params) {
    return LoadAsyncImpl<T>(path, callback, params);
}

template<typename T>
inline ResourceHandle ResourceManager::Get(const std::string& path) const {
    return GetImpl<T>(path);
}

template<typename T>
inline ResourceHandle ResourceManager::Get(uint64_t id) const {
    return GetImpl<T>(id);
}

template<typename T>
inline ResourceHandle ResourceManager::Reload(const std::string& path) {
    return ReloadImpl<T>(path);
}

template<typename T>
inline bool ResourceManager::RegisterLoader(
    std::function<ResourceHandle(const std::string&, const ResourceLoadParams&)> loader) {
    return RegisterLoaderImpl<T>(loader);
}

template<typename T>
inline bool ResourceManager::UnregisterLoader() {
    return UnregisterLoaderImpl<T>();
}

} // namespace Resource
} // namespace DSRT
