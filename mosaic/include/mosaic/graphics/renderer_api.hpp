#pragma once

#include "mosaic/core/window.hpp"

namespace mosaic
{
namespace graphics
{

class RendererAPI
{
   public:
    virtual ~RendererAPI() = default;

    virtual void initialize(const core::Window* _window) = 0;
    virtual void shutdown() = 0;
    virtual void recreateSwapchain() = 0;
    virtual void beginFrame() = 0;
    virtual void updateResources() = 0;
    virtual void drawScene() = 0;
    virtual void endFrame() = 0;
};

} // namespace graphics
} // namespace mosaic
