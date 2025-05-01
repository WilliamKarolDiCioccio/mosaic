#pragma once

#include <pieces/result.hpp>

#include "mosaic/core/window.hpp"

#include "buffer.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include "pipeline.hpp"
#include "draw_call.hpp"

namespace mosaic
{
namespace graphics
{

struct RenderContextSettings
{
    bool enableDebugLayers;
    uint32_t backbufferCount;

    RenderContextSettings(bool _enableDebugLayers, uint32_t _backbufferCount)
        : enableDebugLayers(_enableDebugLayers), backbufferCount(_backbufferCount) {};
};

class RenderSystem;

class RenderContext
{
   protected:
    const core::Window* m_window;
    RenderContextSettings m_settings;

   public:
    RenderContext(const core::Window* _window, const RenderContextSettings& _settings)
        : m_window(_window), m_settings(_settings) {};
    virtual ~RenderContext() = default;

   public:
    virtual pieces::RefResult<RenderContext, std::string> initialize(
        RenderSystem* _renderSystem) = 0;
    virtual void shutdown() = 0;

    void render();

    // setScene()
    // setCamera()

   protected:
    virtual void resizeFramebuffer() = 0;
    virtual void beginFrame() = 0;
    virtual void updateResources() = 0;
    virtual void drawScene() = 0;
    virtual void endFrame() = 0;
};

} // namespace graphics
} // namespace mosaic
