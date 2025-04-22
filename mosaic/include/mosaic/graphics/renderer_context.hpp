#pragma once

#include "mosaic/core/window.hpp"

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

class RenderContext
{
   protected:
    const core::Window* m_window;
    RenderContextSettings m_settings;

   public:
    RenderContext(const core::Window* _window, const RenderContextSettings& _settings)
        : m_window(_window), m_settings(_settings) {};

    virtual ~RenderContext() = default;

    virtual void resizeFramebuffer() = 0;
    virtual void beginFrame() = 0;
    virtual void updateResources() = 0;
    virtual void drawScene() = 0;
    virtual void endFrame() = 0;
};

} // namespace graphics
} // namespace mosaic
