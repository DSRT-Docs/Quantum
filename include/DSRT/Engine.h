/**
 * @file Engine.h
 * @brief Main DSRT Engine entry point and lifecycle management
 * @ingroup Engine
 * 
 * @details Provides static facade for initializing, updating, and shutting down
 *          all engine subsystems. This is the primary interface for game code.
 */

#pragma once
#include "Export.h"
#include "Core/Types.h"
#include <cstdint>
#include <string>

namespace DSRT {

/**
 * @struct EngineConfig
 * @brief Engine initialization configuration
 * @ingroup Engine
 * 
 * @details All settings required to initialize the DSRT Engine.
 *          Passed to Engine::Initialize().
 */
struct EngineConfig {
    // Window settings
    const char* appName = "DSRT Application";   /**< Application name for window title */
    uint32_t windowWidth = 1280;                /**< Initial window width in pixels */
    uint32_t windowHeight = 720;                /**< Initial window height in pixels */
    bool fullscreen = false;                    /**< Start in fullscreen mode */
    bool resizable = true;                      /**< Allow window resizing */
    bool borderless = false;                    /**< Borderless window */
    
    // Graphics settings
    GraphicsAPI graphicsAPI = GraphicsAPI::OpenGL; /**< Graphics API to use */
    bool enableVSync = true;                    /**< Enable vertical synchronization */
    uint32_t msaaSamples = 4;                   /**< Multisample anti-aliasing samples */
    bool enableDebugContext = false;            /**< Enable graphics API debug context */
    
    // Engine features
    uint32_t enabledFeatures = 0;               /**< Bitmask of EngineFlags */
    bool enableAssetHotReload = true;           /**< Reload assets when files change */
    bool enableAutoShaderCompilation = true;    /**< Compile shaders at runtime */
    
    // Performance settings
    uint32_t maxFPS = 0;                        /**< Maximum FPS (0 = unlimited) */
    bool fixedTimeStep = false;                 /**< Use fixed time step */
    float fixedDeltaTime = 1.0f / 60.0f;        /**< Fixed update interval in seconds */
    
    // Memory settings
    size_t defaultHeapSize = 64 * 1024 * 1024;  /**< Default allocator heap size */
    size_t frameHeapSize = 16 * 1024 * 1024;    /**< Frame allocator heap size */
    size_t persistentHeapSize = 32 * 1024 * 1024; /**< Persistent allocator heap size */
    
    // Platform-specific
    void* nativeWindowHandle = nullptr;         /**< Existing native window handle */
    void* nativeDisplayHandle = nullptr;        /**< Existing native display handle */
};

/**
 * @class Engine
 * @brief Static engine facade and lifecycle manager
 * @ingroup Engine
 * 
 * @details Provides global access to engine functionality through static methods.
 *          Follows singleton pattern (not instance-based) for simplicity.
 * 
 * @example Basic engine usage:
 * @code
 * DSRT::EngineConfig config;
 * config.appName = "My Game";
 * config.windowWidth = 1920;
 * config.windowHeight = 1080;
 * 
 * if (!DSRT::Engine::Initialize(config)) {
 *     return EXIT_FAILURE;
 * }
 * 
 * while (DSRT::Engine::IsRunning()) {
 *     DSRT::Engine::PollEvents();
 *     DSRT::Engine::Update();
 *     DSRT::Engine::Render();
 * }
 * 
 * DSRT::Engine::Shutdown();
 * @endcode
 * 
 * @example Advanced configuration:
 * @code
 * DSRT::EngineConfig config;
 * config.graphicsAPI = DSRT::GraphicsAPI::Vulkan;
 * config.enabledFeatures = 
 *     DSRT::EngineFlags::EnableAudio |
 *     DSRT::EngineFlags::EnablePhysics |
 *     DSRT::EngineFlags::EnableDebugUI;
 * config.maxFPS = 144;
 * 
 * DSRT::Engine::Initialize(config);
 * @endcode
 */
class DSRT_API Engine {
public:
    /**
     * @brief Initialize DSRT Engine with configuration
     * @param config Engine configuration
     * @return true if initialization succeeded
     * 
     * @details Performs the following in order:
     *          1. Platform initialization (window, input)
     *          2. Memory system setup
     *          3. Graphics device creation
     *          4. Audio device initialization
     *          5. Asset system startup
     *          6. Default world creation
     * 
     * @throws std::runtime_error if critical subsystems fail
     * @warning Must be called before any other engine functions
     */
    static bool Initialize(const EngineConfig& config = EngineConfig());
    
    /**
     * @brief Shutdown engine and release all resources
     * 
     * @details Performs cleanup in reverse initialization order.
     *          Safe to call multiple times (idempotent).
     */
    static void Shutdown();
    
    /**
     * @brief Check if engine main loop should continue
     * @return true while engine should keep running
     * 
     * @details Returns false when:
     *          - Window close requested
     *          - Engine::Quit() called
     *          - Critical error occurred
     */
    static bool IsRunning();
    
    /**
     * @brief Poll platform events
     * 
     * @details Processes window events, input, etc.
     *          Should be called once per frame before Update().
     */
    static void PollEvents();
    
    /**
     * @brief Update engine state
     * @param deltaTime Time since last update in seconds
     * 
     * @details If deltaTime is 0.0, uses actual elapsed time.
     *          Updates all active systems in order:
     *          1. Input system
     *          2. Physics simulation
     *          3. Script/behavior updates
     *          4. Animation system
     *          5. Audio updates
     */
    static void Update(float deltaTime = 0.0f);
    
    /**
     * @brief Render current frame
     * 
     * @details Renders all visible objects in active world(s).
     *          Should be called once per frame after Update().
     */
    static void Render();
    
    /**
     * @brief Request engine to stop
     * 
     * @details Sets internal running flag to false.
     *          IsRunning() will return false on next check.
     */
    static void Quit();
    
    // Information getters
    
    /**
     * @brief Get engine version string
     * @return Version string (e.g., "1.2.3")
     */
    static const char* GetVersion();
    
    /**
     * @brief Get application name from config
     * @return Application name string
     */
    static const char* GetAppName();
    
    /**
     * @brief Get engine build configuration
     * @return Build type string ("Debug", "Release", etc.)
     */
    static const char* GetBuildConfig();
    
    /**
     * @brief Get graphics API in use
     * @return GraphicsAPI enum value
     */
    static GraphicsAPI GetGraphicsAPI();
    
    /**
     * @brief Get current platform
     * @return Platform enum value
     */
    static Platform GetPlatform();
    
    // Statistics
    
    /**
     * @brief Get frame rate
     * @return Current frames per second
     */
    static float GetFPS();
    
    /**
     * @brief Get frame time
     * @return Time spent on last frame in seconds
     */
    static float GetFrameTime();
    
    /**
     * @brief Get memory usage statistics
     * @param[out] total Total allocated memory
     * @param[out] used Currently used memory
     * @param[out] peak Peak memory usage
     */
    static void GetMemoryStats(size_t& total, size_t& used, size_t& peak);
    
    /**
     * @brief Get entity count in active world
     * @return Number of entities
     */
    static size_t GetEntityCount();
    
    /**
     * @brief Get draw call count from last frame
     * @return Number of draw calls
     */
    static uint32_t GetDrawCallCount();
    
    /**
     * @brief Get triangle count from last frame
     * @return Number of rendered triangles
     */
    static uint32_t GetTriangleCount();
    
    // Subsystem access
    
    /**
     * @brief Get main renderer instance
     * @return Pointer to main renderer
     * @warning Do not delete - owned by engine
     */
    static class Graphics::Renderer* GetRenderer();
    
    /**
     * @brief Get main audio device
     * @return Pointer to audio device
     * @warning Do not delete - owned by engine
     */
    static class Audio::AudioDevice* GetAudioDevice();
    
    /**
     * @brief Get main input manager
     * @return Pointer to input manager
     * @warning Do not delete - owned by engine
     */
    static class Input::InputManager* GetInputManager();
    
    /**
     * @brief Get default world
     * @return Reference to default world
     */
    static class Scene::World& GetWorld();
    
    // Utility
    
    /**
     * @brief Get elapsed time since engine start
     * @return Time in seconds
     */
    static float GetElapsedTime();
    
    /**
     * @brief Sleep for specified duration
     * @param seconds Time to sleep in seconds
     */
    static void Sleep(float seconds);
    
    /**
     * @brief Set window title
     * @param title New window title
     */
    static void SetWindowTitle(const char* title);
    
    /**
     * @brief Show or hide mouse cursor
     * @param visible true to show cursor
     */
    static void SetCursorVisible(bool visible);
    
    /**
     * @brief Capture or release mouse cursor
     * @param captured true to capture (confine to window)
     */
    static void SetCursorCaptured(bool captured);
    
private:
    // Prevent instantiation
    Engine() = delete;
    ~Engine() = delete;
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
};

} // namespace DSRT 
