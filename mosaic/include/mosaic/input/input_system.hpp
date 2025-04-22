#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

#include "input_context.hpp"

namespace mosaic
{
namespace input
{

/**
 * @brief The `InputSystem` class is responsible for managing input contexts for different windows.
 *
 * `InputSystem`'s responsibilities include:
 *
 * - Registering and unregistering input contexts for each window.
 *
 * - Managing GLFW window events loop.
 *
 * The input system as a whole consists of multiple componenets, ensuring clean separation of
 * concerns. Each of those components adds an abstraction layer on top of the previous one, allowing
 * for easy extensibility and maintainability.
 *
 * @note This class is a singleton and should be accessed through the static method
 * `getGlobalInputSystem()`. It is not meant to be instantiated directly.
 *
 * @see InputContext
 * @see InputArena
 * @see RawInputHandler
 * @see Window
 * @see https://www.glfw.org/documentation.html
 */
class MOSAIC_API InputSystem
{
   public:
    std::unordered_map<GLFWwindow*, std::unique_ptr<InputContext>> m_contexts;

   private:
    InputSystem() = default;

   public:
    ~InputSystem() = default;
    InputSystem(const InputSystem&) = delete;
    InputSystem& operator=(const InputSystem&) = delete;

   public:
    InputContext* registerWindow(const core::Window* _window);
    void unregisterWindow(const core::Window* _window);

    inline void unregisterAllWindows() { m_contexts.clear(); }

    /**
     * @brief Polls GLFW events and updates all registered input contexts.
     *
     * This is the first invocation in the main loop of the application. It should be called before
     * any other input handling functions to ensure fresh input data is available and minimize
     * latency.
     */
    inline void poll() const
    {
        glfwPollEvents();

        for (auto& context : m_contexts)
        {
            context.second->update();
        }
    }

    inline InputContext* getContext(const core::Window* _window) const
    {
        const auto glfwWindow = _window->getGLFWHandle();

        if (m_contexts.find(glfwWindow) == m_contexts.end())
        {
            MOSAIC_ERROR("Input context not found for the given window.");
            return nullptr;
        }

        return m_contexts.at(glfwWindow).get();
    }

    inline static InputSystem& get()
    {
        static InputSystem instance;
        return instance;
    }
};

} // namespace input
} // namespace mosaic
