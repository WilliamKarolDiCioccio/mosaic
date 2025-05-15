#pragma once

#include "mosaic/graphics/render_system.hpp"

#include "webgpu_instance.hpp"
#include "webgpu_device.hpp"

namespace mosaic
{
namespace graphics
{
namespace webgpu
{

class WebGPURenderSystem : public RenderSystem
{
   public:
    WebGPURenderSystem() : RenderSystem(RendererAPIType::web_gpu) {};

   public:
    pieces::RefResult<RenderSystem, std::string> initialize(const core::Window* _window) override;
    void shutdown() override;
};

} // namespace webgpu
} // namespace graphics
} // namespace mosaic
