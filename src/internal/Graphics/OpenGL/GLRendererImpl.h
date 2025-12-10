// src/internal/Graphics/OpenGL/GLRendererImpl.h
#pragma once

#include "DSRT/Graphics/Renderer.h"
#include <glad/glad.h>
#include <unordered_map>

namespace DSRT { namespace internal {

class GLRendererImpl : public Renderer {
public:
    GLRendererImpl();
    ~GLRendererImpl();

    bool Initialize(uint32_t width, uint32_t height) override;
    void BeginFrame() override;
    void DrawMesh(const Mesh* mesh, const Matrix4& transform) override;
    void EndFrame() override;
    RendererCapabilities GetCapabilities() const override;

private:
    GLuint m_DefaultShader = 0;
    GLuint m_Framebuffer = 0;

    struct RenderState {
        GLuint CurrentShader = 0;
        GLuint CurrentVAO = 0;
        GLuint CurrentTexture = 0;
    } m_State;

    std::unordered_map<const Mesh*, GLuint> m_MeshCache;

    GLuint CompileShader(const char* src, GLenum type);
    GLuint CreateDefaultShader();
    void SetupFramebuffer();
};

}} // namespace
