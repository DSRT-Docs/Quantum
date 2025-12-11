/**
 * @file main.cpp
 * @brief Basic DSRT Engine example - Window and rendering
 */

#include "DSRT/Engine.h"
#include "DSRT/Graphics/Renderer.h"
#include "DSRT/Math/Vector3.h"
#include "DSRT/Core/Log.h"

#include <iostream>

using namespace DSRT;

int main() {
    // Configure engine
    EngineConfig config;
    config.appName = "DSRT Basic Example";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.enableVSync = true;
    config.enableDebugUI = false;
    config.graphicsAPI = GraphicsAPI::OpenGL;
    
    // Initialize engine
    if (!Engine::Initialize(config)) {
        std::cerr << "Failed to initialize DSRT Engine!" << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "Engine Version: " << Engine::GetVersion() << std::endl;
    std::cout << "Platform: " << static_cast<int>(Engine::GetPlatform()) << std::endl;
    std::cout << "Graphics API: " << static_cast<int>(Engine::GetGraphicsAPI()) << std::endl;
    
    // Get subsystems
    auto* renderer = Engine::GetRenderer();
    auto& world = Engine::GetWorld();
    
    if (renderer) {
        auto caps = renderer->GetCapabilities();
        std::cout << "Renderer: " << caps.renderer << " (" << caps.vendor << ")" << std::endl;
    }
    
    // Create some entities
    auto entity1 = world.CreateEntity("Player");
    auto entity2 = world.CreateEntity("Enemy");
    
    std::cout << "Created " << Engine::GetEntityCount() << " entities" << std::endl;
    
    // Main loop
    std::cout << "\nStarting main loop (5 seconds)..." << std::endl;
    
    float elapsed = 0.0f;
    const float duration = 5.0f; // Run for 5 seconds
    
    while (Engine::IsRunning() && elapsed < duration) {
        // Update engine
        Engine::PollEvents();
        Engine::Update(); // Auto-calculates delta time
        
        // Custom game logic here
        elapsed = Engine::GetElapsedTime();
        
        // Render frame
        Engine::Render();
        
        // Print stats every second
        static float lastPrint = 0.0f;
        if (elapsed - lastPrint >= 1.0f) {
            std::cout << "Time: " << elapsed << "s, FPS: " << Engine::GetFPS() 
                      << ", Entities: " << Engine::GetEntityCount() << std::endl;
            lastPrint = elapsed;
        }
        
        // Check for exit
        if (elapsed >= duration) {
            Engine::Quit();
        }
    }
    
    // Shutdown
    std::cout << "\nShutting down..." << std::endl;
    Engine::Shutdown();
    
    std::cout << "Example completed successfully!" << std::endl;
    return EXIT_SUCCESS;
}
