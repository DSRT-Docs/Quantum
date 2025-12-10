// src/public/Engine.cpp
#include "DSRT/Engine.h"
#include "Core/Log.h"
#include "internal/Platform/GLFWWindow.h"
#include "internal/Graphics/OpenGL/GLRendererImpl.h"

namespace DSRT {

Engine::Engine() {}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Initialize(const Config& config) {
    Log::Info("Initializing DSRT Engine...");

    // Window (internal class)
    if (!internal::GLFWWindow::InitializeSystem()) {
        Log::Error("Failed to initialize GLFW");
        return false;
    }

    // Select renderer (factory)
    #ifdef DSRT_RENDERER_OPENGL
        m_Renderer = new internal::GLRendererImpl();
    #else
        m_Renderer = new internal::GLRendererImpl();
    #endif

    if (!m_Renderer->Initialize(config.WindowWidth, config.WindowHeight)) {
        Log::Error("Renderer initialization failed!");
        return false;
    }

    m_Running = true;
    return true;
}

void Engine::Run() {
    while (m_Running) {
        MainLoop();
    }
}

void Engine::MainLoop() {
    m_Renderer->BeginFrame();
    // TODO: scene update, entity system, scripting
    m_Renderer->EndFrame();
}

void Engine::Shutdown() {
    if (m_Renderer) {
        delete m_Renderer;
        m_Renderer = nullptr;
    }
    internal::GLFWWindow::TerminateSystem();
}

} // namespace DSRT
