// include/DSRT/Graphics/Renderer.h
#pragma once
#include "DSRT/Export.h"
#include "DSRT/Math/Matrix4.h"

namespace DSRT {

class Texture;
class Mesh;
class Shader;

struct RendererCapabilities {
    bool SupportsInstancing = false;
    bool SupportsComputeShaders = false;
    int MaxTextures = 0;
    int MaxUniformBuffers = 0;
};

class DSRT_API Renderer {
public:
    virtual ~Renderer() = default;

    virtual bool Initialize(uint32_t width, uint32_t height) = 0;
    virtual void BeginFrame() = 0;
    virtual void DrawMesh(const Mesh* mesh, const Matrix4& transform) = 0;
    virtual void EndFrame() = 0;
    virtual RendererCapabilities GetCapabilities() const = 0;

    static Renderer* Create();
};

} // namespace DSRT
