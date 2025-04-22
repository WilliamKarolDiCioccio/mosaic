#include "mosaic/graphics/render_system.hpp"

#include "WebGPU/webgpu_render_context.hpp"
#include "Vulkan/vulkan_render_context.hpp"

namespace mosaic
{
namespace graphics
{

pieces::Result<RenderContext*, std::string> RenderSystem::createContext(const core::Window* _window)
{
    if (m_contexts.find(_window->getGLFWHandle()) != m_contexts.end())
    {
        MOSAIC_WARN("RenderSystem: Context already exists for this window");
        return pieces::Ok<RenderContext*, std::string>(m_contexts[_window->getGLFWHandle()].get());
    }

    std::unique_ptr<RenderContext> context;

    switch (m_apiType)
    {
        case RendererAPIType::web_gpu:
            context = std::make_unique<webgpu::WebGPURenderContext>(_window,
                                                                    RenderContextSettings{true, 2});
            break;
        case RendererAPIType::vulkan:
            context = std::make_unique<vulkan::VulkanRenderContext>(_window,
                                                                    RenderContextSettings{true, 2});
            break;
        default:
            return pieces::Err<RenderContext*, std::string>("RenderSystem: Unsupported API type");
    }

    m_contexts[_window->getGLFWHandle()] = std::move(context);

    return pieces::Ok<RenderContext*, std::string>(m_contexts[_window->getGLFWHandle()].get());
}

void RenderSystem::destroyContext(const core::Window* _window)
{
    auto it = m_contexts.find(_window->getGLFWHandle());

    if (it != m_contexts.end())
    {
        m_contexts.erase(it);
    }
}

} // namespace graphics
} // namespace mosaic
