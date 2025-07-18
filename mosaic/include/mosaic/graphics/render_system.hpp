#pragma once

#include <memory>
#include <unordered_map>

#include <pieces/core/result.hpp>

#include "mosaic/internal/defines.hpp"
#include "mosaic/window/window.hpp"

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
    RendererAPIType m_apiType;
    std::unordered_map<const window::Window*, std::unique_ptr<RenderContext>> m_contexts;

   public:
    RenderSystem(RendererAPIType _apiType) : m_apiType(_apiType) {};
    virtual ~RenderSystem() = default;

    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;
    RenderSystem(RenderSystem&&) = default;
    RenderSystem& operator=(RenderSystem&&) = default;

    static std::unique_ptr<RenderSystem> create(RendererAPIType _type);

   public:
    virtual pieces::RefResult<RenderSystem, std::string> initialize(
        const window::Window* _window) = 0;
    virtual void shutdown() = 0;

    pieces::Result<RenderContext*, std::string> createContext(const window::Window* _window);
    void destroyContext(const window::Window* _window);

    // createMaterial()
    // createShader()
    // createTexture()
    // createMesh()
    // createRenderPass()

    inline void destroyAllContexts()
    {
        for (auto& [window, context] : m_contexts)
        {
            context->shutdown();
        }

        m_contexts.clear();
    }

    inline void render()
    {
        for (auto& [window, context] : m_contexts)
        {
            context->render();
        }
    }

    inline RenderContext* getContext(const window::Window* _window) const
    {
        if (m_contexts.find(_window) != m_contexts.end())
        {
            return m_contexts.at(_window).get();
        }

        return nullptr;
    }
};

} // namespace graphics
} // namespace mosaic
