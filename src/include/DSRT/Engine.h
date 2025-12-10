// include/DSRT/Engine.h
#pragma once
#include "DSRT/Export.h"
#include "Core/Config.h"
#include "Graphics/Renderer.h"

namespace DSRT {

class DSRT_API Engine {
public:
    Engine();
    ~Engine();

    bool Initialize(const Config& config);
    void Run();
    void Shutdown();

    Renderer* GetRenderer() const { return m_Renderer; }

private:
    Renderer* m_Renderer = nullptr;
    bool m_Running = false;

    void MainLoop();
};

} // namespace DSRT
