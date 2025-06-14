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
    ~WebGPURenderSystem() override = default;

   public:
    pieces::RefResult<RenderSystem, std::string> initialize(const window::Window* _window) override;
    void shutdown() override;
};

} // namespace webgpu
} // namespace graphics
} // namespace mosaic
