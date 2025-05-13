#pragma once

#include <memory>
#include <unordered_map>

#include <pieces/result.hpp>

#include "mosaic/defines.hpp"
#include "render_context.hpp"

namespace mosaic
{
namespace graphics
{

enum class RendererAPIType
{
    web_gpu,
    vulkan,
    none
};

class MOSAIC_API RenderSystem
{
   private:
    bool m_initialized = false;
    RendererAPIType m_apiType;
    std::unordered_map<GLFWwindow*, std::unique_ptr<RenderContext>> m_contexts;

   public:
    RenderSystem(RendererAPIType _apiType) : m_apiType(_apiType) {};
    virtual ~RenderSystem() = default;

    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;

    RenderSystem(RenderSystem&&) = default;
    RenderSystem& operator=(RenderSystem&&) = default;

    static std::unique_ptr<RenderSystem> create(RendererAPIType _apiType);

   public:
    virtual pieces::RefResult<RenderSystem, std::string> initialize(
        const core::Window* _window) = 0;
    virtual void shutdown() = 0;

    pieces::Result<RenderContext*, std::string> createContext(const core::Window* _window);
    void destroyContext(const core::Window* _window);

    // createMaterial()
    // createShader()
    // createTexture()
    // createMesh()
    // createRenderPass()

    inline void destroyAllContexts() { m_contexts.clear(); }

    inline void render()
    {
        for (auto& [window, context] : m_contexts)
        {
            context->render();
        }
    }

    inline RenderContext* getContext(const core::Window* _window) const
    {
        const auto glfwWindow = _window->getGLFWHandle();

        if (m_contexts.find(glfwWindow) != m_contexts.end())
        {
            return m_contexts.at(glfwWindow).get();
        }

        return nullptr;
    }
};

} // namespace graphics
} // namespace mosaic
