#pragma once

#include <string>
#include <vector>
#include <queue>
#include <optional>
#include <functional>

#include "mosaic/core/logger.hpp"
#include "mosaic/graphics/window.hpp"

#include "glfw_mappings.hpp"

namespace mosaic
{
namespace input
{

/**
 * @brief The `RawInputHandler` provides thread-safe access the input state.
 *
 * @note Due to latency issues we've switched from callbacks to polling the input state. This
 * means this class now has really little responsibility and is just a wrapper around GLFW
 * functions, except for the mouse scroll input which is still handled by callbacks. The reason for
 * this is that GLFW does not provide a way to poll mouse scroll input.
 */
class RawInputHandler
{
   private:
    using KeyboardKeyInputData = int;
    using MouseButtonInputData = int;
    using MouseScrollInputData = glm::vec2;
    using CursorPosInputData = glm::vec2;

   private:
    GLFWwindow* m_glfwWindow;
    std::queue<MouseScrollInputData> m_mouseScrollQueue;
    bool m_isActive;

   public:
    RawInputHandler(const graphics::Window* _window);
    ~RawInputHandler() = default;

    RawInputHandler(const RawInputHandler&) = delete;
    RawInputHandler& operator=(const RawInputHandler&) = delete;

    inline bool isActive() const { return m_isActive; }

    inline KeyboardKeyInputData getKeyboardKeyInput(KeyboardKey _key) const
    {
        return glfwGetKey(m_glfwWindow, static_cast<int>(_key));
    }

    inline MouseButtonInputData getMouseButtonInput(MouseButton _button) const
    {
        return glfwGetMouseButton(m_glfwWindow, static_cast<int>(_button));
    }

    inline CursorPosInputData getCursorPosInput()
    {
        double xpos, ypos;
        glfwGetCursorPos(m_glfwWindow, &xpos, &ypos);
        return glm::vec2(xpos, ypos);
    }

    inline bool mouseScrollInputAvailable() const { return !m_mouseScrollQueue.empty(); }

    inline MouseScrollInputData getMouseScrollInput()
    {
        MouseScrollInputData scrollInput = m_mouseScrollQueue.front();
        m_mouseScrollQueue.pop();
        return scrollInput;
    }
};

} // namespace input
} // namespace mosaic
