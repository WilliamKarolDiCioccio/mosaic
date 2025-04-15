#include "mosaic/graphics/renderer.hpp"

#include "WebGPU/webgpu_renderer_api.hpp"
#include "Vulkan/vulkan_renderer_api.hpp"

namespace mosaic
{
namespace graphics
{

void Renderer::setAPI(RendererAPIType _apiType)
{
    if (s_rendererAPI)
    {
        s_rendererAPI->shutdown();
    }

    switch (_apiType)
    {
        case RendererAPIType::web_gpu:
            s_rendererAPI = std::make_unique<webgpu::WebGPURendererAPI>();
            break;
        case RendererAPIType::vulkan:
            s_rendererAPI = std::make_unique<vulkan::VulkanRendererAPI>();
            break;
    }
}

void Renderer::initialize(const core::Window* _window) { s_rendererAPI->initialize(_window); }

void Renderer::render()
{
    s_rendererAPI->beginFrame();
    s_rendererAPI->updateResources();
    s_rendererAPI->drawScene();
    s_rendererAPI->endFrame();
}

void Renderer::shutdown() { s_rendererAPI->shutdown(); }

} // namespace graphics
} // namespace mosaic
