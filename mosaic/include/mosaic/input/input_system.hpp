#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

#include "mosaic/graphics/window.hpp"

#include "input_context.hpp"

namespace mosaic
{
namespace input
{

class MOSAIC_API InputSystem
{
   public:
    std::unordered_map<GLFWwindow*, std::unique_ptr<InputContext>> m_contexts;

   public:
    InputSystem() = default;
    ~InputSystem() = default;

    InputSystem(const InputSystem&) = delete;
    InputSystem& operator=(const InputSystem&) = delete;

   public:
    InputContext* registerWindow(const graphics::Window& _window);
    void unregisterWindow(const graphics::Window& _window);

    inline void unregisterAllWindows() { m_contexts.clear(); }

    inline void updateContexts() const
    {
        glfwPollEvents();

        for (auto& context : m_contexts)
        {
            context.second->update();
        }
    }

    inline InputContext* getContext(GLFWwindow* _window)
    {
        if (m_contexts.find(_window) == m_contexts.end())
        {
            MOSAIC_ERROR("Input context not found for the given window.");
            return nullptr;
        }

        return m_contexts[_window].get();
    }

    inline static InputSystem& getGlobalInputSystem()
    {
        static std::unique_ptr<InputSystem> instance = std::make_unique<InputSystem>();
        return *instance;
    }
};

} // namespace input
} // namespace mosaic
