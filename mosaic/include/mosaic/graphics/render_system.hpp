#pragma once

#include <memory>

#include "mosaic/defines.hpp"
#include "renderer_context.hpp"

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
    friend class RenderContext;

   private:
    RendererAPIType m_apiType;
    std::unordered_map<GLFWwindow*, std::unique_ptr<RenderContext>> m_contexts;

   private:
    RenderSystem() : m_apiType(RendererAPIType::none) { destroyAllContexts(); }

   public:
    ~RenderSystem() = default;
    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;

   public:
    RenderContext* createContext(const core::Window* _window);
    void destroyContext(const core::Window* _window);

    inline void destroyAllContexts() { m_contexts.clear(); }

    inline void render()
    {
        for (auto& context : m_contexts)
        {
            context.second->beginFrame();
            context.second->updateResources();
            context.second->drawScene();
            context.second->endFrame();
        }
    }

    inline RenderContext* getRenderContext(const core::Window* _window) { return nullptr; }

    inline void setAPI(RendererAPIType _apiType) { m_apiType = _apiType; }

    inline static RenderSystem& get()
    {
        static RenderSystem instance;
        return instance;
    }
};

} // namespace graphics
} // namespace mosaic
