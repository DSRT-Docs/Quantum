#include "DSRT/Engine.h"
#include "DSRT/Core/Assert.h"
#include "DSRT/Core/Memory.h"
#include "DSRT/Core/Log.h"
#include "DSRT/Graphics/Renderer.h"
#include "DSRT/Audio/AudioDevice.h"
#include "DSRT/Input/InputManager.h"
#include "DSRT/Scene/World.h"
#include "DSRT/Resource/ResourceManager.h"

#include <memory>
#include <chrono>
#include <thread>

namespace DSRT {

// Engine internal state
namespace Internal {
    struct EngineState {
        EngineConfig config;
        bool initialized = false;
        bool running = false;
        bool shutdownRequested = false;
        
        // Subsystems
        std::unique_ptr<Graphics::Renderer> renderer;
        std::unique_ptr<Audio::AudioDevice> audioDevice;
        std::unique_ptr<Input::InputManager> inputManager;
        std::unique_ptr<Resource::ResourceManager> resourceManager;
        std::unique_ptr<Scene::World> defaultWorld;
        
        // Timing
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point lastFrameTime;
        float deltaTime = 0.0f;
        float elapsedTime = 0.0f;
        float fps = 0.0f;
        uint32_t frameCount = 0;
        
        // Statistics
        uint32_t drawCallCount = 0;
        uint32_t triangleCount = 0;
        size_t memoryTotal = 0;
        size_t memoryUsed = 0;
        size_t memoryPeak = 0;
    };
    
    static EngineState s_State;
}

bool Engine::Initialize(const EngineConfig& config) {
    if (Internal::s_State.initialized) {
        Log::Error("Engine already initialized");
        return false;
    }
    
    Log::Info("Initializing DSRT Engine...");
    Log::Info("Version: %s", GetVersion());
    Log::Info("Platform: %d", static_cast<int>(GetPlatform()));
    Log::Info("Graphics API: %d", static_cast<int>(config.graphicsAPI));
    
    // Store config
    Internal::s_State.config = config;
    
    try {
        // 1. Initialize memory system
        Log::Debug("Initializing memory system...");
        Core::Memory::Initialize();
        
        // 2. Initialize subsystems
        Log::Debug("Initializing subsystems...");
        
        // Graphics
        if (config.graphicsAPI != GraphicsAPI::None) {
            Internal::s_State.renderer = std::unique_ptr<Graphics::Renderer>(Graphics::Renderer::Create());
            if (!Internal::s_State.renderer || !Internal::s_State.renderer->Initialize(config.windowWidth, config.windowHeight)) {
                Log::Error("Failed to initialize renderer");
                return false;
            }
            
            auto caps = Internal::s_State.renderer->GetCapabilities();
            Log::Info("Graphics: %s %s (%s)", caps.vendor, caps.renderer, caps.version);
        }
        
        // Audio
        if (config.enabledFeatures & EngineFlags::EnableAudio) {
            Internal::s_State.audioDevice = Audio::AudioDevice::Create();
            if (!Internal::s_State.audioDevice || !Internal::s_State.audioDevice->Initialize()) {
                Log::Warning("Failed to initialize audio device");
            } else {
                Log::Info("Audio: %s", Internal::s_State.audioDevice->GetBackendName());
            }
        }
        
        // Input
        Internal::s_State.inputManager = Input::InputManager::Create();
        if (!Internal::s_State.inputManager || !Internal::s_State.inputManager->Initialize()) {
            Log::Error("Failed to initialize input manager");
            return false;
        }
        
        // Resource manager
        if (config.enableAssetHotReload) {
            Internal::s_State.resourceManager = Resource::ResourceManager::Create();
            if (!Internal::s_State.resourceManager || !Internal::s_State.resourceManager->Initialize()) {
                Log::Warning("Failed to initialize resource manager");
            }
        }
        
        // Default world
        Internal::s_State.defaultWorld = std::make_unique<Scene::World>("DefaultWorld");
        Internal::s_State.defaultWorld->Initialize();
        
        // 3. Initialize timing
        Internal::s_State.startTime = std::chrono::high_resolution_clock::now();
        Internal::s_State.lastFrameTime = Internal::s_State.startTime;
        
        // 4. Set initial state
        Internal::s_State.initialized = true;
        Internal::s_State.running = true;
        Internal::s_State.shutdownRequested = false;
        
        Log::Info("Engine initialized successfully");
        Log::Info("Application: %s", config.appName);
        Log::Info("Window: %dx%d", config.windowWidth, config.windowHeight);
        Log::Info("VSync: %s", config.enableVSync ? "Enabled" : "Disabled");
        
        return true;
        
    } catch (const std::exception& e) {
        Log::Error("Engine initialization failed: %s", e.what());
        Shutdown();
        return false;
    }
}

void Engine::Shutdown() {
    if (!Internal::s_State.initialized) {
        return;
    }
    
    Log::Info("Shutting down DSRT Engine...");
    
    Internal::s_State.running = false;
    Internal::s_State.shutdownRequested = true;
    
    // Shutdown in reverse order
    if (Internal::s_State.defaultWorld) {
        Internal::s_State.defaultWorld->Shutdown();
        Internal::s_State.defaultWorld.reset();
    }
    
    if (Internal::s_State.resourceManager) {
        Internal::s_State.resourceManager->Shutdown();
        Internal::s_State.resourceManager.reset();
    }
    
    if (Internal::s_State.inputManager) {
        Internal::s_State.inputManager->Shutdown();
        Internal::s_State.inputManager.reset();
    }
    
    if (Internal::s_State.audioDevice) {
        Internal::s_State.audioDevice->Shutdown();
        Internal::s_State.audioDevice.reset();
    }
    
    if (Internal::s_State.renderer) {
        // Renderer doesn't have Shutdown() in interface
        Internal::s_State.renderer.reset();
    }
    
    Core::Memory::Shutdown();
    
    Internal::s_State.initialized = false;
    
    Log::Info("Engine shutdown complete");
}

bool Engine::IsRunning() {
    return Internal::s_State.running && !Internal::s_State.shutdownRequested;
}

void Engine::PollEvents() {
    if (!Internal::s_State.initialized) return;
    
    if (Internal::s_State.inputManager) {
        Internal::s_State.inputManager->Update(Internal::s_State.deltaTime);
    }
    
    // Platform-specific event polling would go here
    // (window events, etc.)
}

void Engine::Update(float deltaTime) {
    if (!Internal::s_State.initialized) return;
    
    // Calculate delta time if not provided
    auto now = std::chrono::high_resolution_clock::now();
    if (deltaTime <= 0.0f) {
        std::chrono::duration<float> duration = now - Internal::s_State.lastFrameTime;
        Internal::s_State.deltaTime = duration.count();
    } else {
        Internal::s_State.deltaTime = deltaTime;
    }
    
    // Clamp delta time to avoid spiral of death
    const float MAX_DELTA_TIME = 0.1f; // 100ms
    if (Internal::s_State.deltaTime > MAX_DELTA_TIME) {
        Internal::s_State.deltaTime = MAX_DELTA_TIME;
    }
    
    // Update elapsed time
    std::chrono::duration<float> totalDuration = now - Internal::s_State.startTime;
    Internal::s_State.elapsedTime = totalDuration.count();
    
    // Update FPS
    Internal::s_State.frameCount++;
    if (Internal::s_State.elapsedTime > 1.0f) {
        Internal::s_State.fps = Internal::s_State.frameCount / Internal::s_State.elapsedTime;
        Internal::s_State.frameCount = 0;
        Internal::s_State.startTime = now;
    }
    
    // Update resource manager
    if (Internal::s_State.resourceManager) {
        Internal::s_State.resourceManager->Update(Internal::s_State.deltaTime);
    }
    
    // Update world
    if (Internal::s_State.defaultWorld) {
        Internal::s_State.defaultWorld->Update(Internal::s_State.deltaTime);
    }
    
    Internal::s_State.lastFrameTime = now;
}

void Engine::Render() {
    if (!Internal::s_State.initialized || !Internal::s_State.renderer) return;
    
    Internal::s_State.renderer->BeginFrame();
    
    if (Internal::s_State.defaultWorld) {
        Internal::s_State.defaultWorld->Render();
    }
    
    Internal::s_State.renderer->EndFrame();
    
    // Update statistics
    // (In real implementation, these would come from renderer)
    Internal::s_State.drawCallCount = 0; // Would be updated by renderer
    Internal::s_State.triangleCount = 0; // Would be updated by renderer
    
    // Update memory statistics
    Internal::s_State.memoryTotal = Core::Memory::GetTotalAllocated();
    Internal::s_State.memoryPeak = Core::Memory::GetPeakAllocated();
    Internal::s_State.memoryUsed = Internal::s_State.memoryTotal; // Simplified
}

void Engine::Quit() {
    Internal::s_State.shutdownRequested = true;
}

const char* Engine::GetVersion() {
    return "1.0.0";
}

const char* Engine::GetAppName() {
    return Internal::s_State.config.appName;
}

const char* Engine::GetBuildConfig() {
    #ifdef NDEBUG
        return "Release";
    #else
        return "Debug";
    #endif
}

GraphicsAPI Engine::GetGraphicsAPI() {
    return Internal::s_State.config.graphicsAPI;
}

Platform Engine::GetPlatform() {
    #if defined(_WIN32) || defined(_WIN64)
        return Platform::Windows;
    #elif defined(__APPLE__)
        return Platform::macOS;
    #elif defined(__linux__)
        return Platform::Linux;
    #elif defined(__ANDROID__)
        return Platform::Android;
    #elif defined(__IOS__)
        return Platform::iOS;
    #else
        return Platform::Unknown;
    #endif
}

float Engine::GetFPS() {
    return Internal::s_State.fps;
}

float Engine::GetFrameTime() {
    return Internal::s_State.deltaTime;
}

void Engine::GetMemoryStats(size_t& total, size_t& used, size_t& peak) {
    total = Internal::s_State.memoryTotal;
    used = Internal::s_State.memoryUsed;
    peak = Internal::s_State.memoryPeak;
}

size_t Engine::GetEntityCount() {
    if (Internal::s_State.defaultWorld) {
        return Internal::s_State.defaultWorld->GetEntityCount();
    }
    return 0;
}

uint32_t Engine::GetDrawCallCount() {
    return Internal::s_State.drawCallCount;
}

uint32_t Engine::GetTriangleCount() {
    return Internal::s_State.triangleCount;
}

Graphics::Renderer* Engine::GetRenderer() {
    return Internal::s_State.renderer.get();
}

Audio::AudioDevice* Engine::GetAudioDevice() {
    return Internal::s_State.audioDevice.get();
}

Input::InputManager* Engine::GetInputManager() {
    return Internal::s_State.inputManager.get();
}

Scene::World& Engine::GetWorld() {
    DSRT_ASSERT(Internal::s_State.defaultWorld, "Default world not initialized");
    return *Internal::s_State.defaultWorld;
}

float Engine::GetElapsedTime() {
    return Internal::s_State.elapsedTime;
}

void Engine::Sleep(float seconds) {
    std::this_thread::sleep_for(std::chrono::duration<float>(seconds));
}

void Engine::SetWindowTitle(const char* title) {
    // Platform-specific implementation
    Log::Debug("SetWindowTitle: %s", title);
}

void Engine::SetCursorVisible(bool visible) {
    if (Internal::s_State.inputManager) {
        Internal::s_State.inputManager->SetMouseCursorVisible(visible);
    }
}

void Engine::SetCursorCaptured(bool captured) {
    if (Internal::s_State.inputManager) {
        Internal::s_State.inputManager->SetMouseCursorCaptured(captured);
    }
}

} // namespace DSRT
