#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <functional>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "mosaic/core/logger.hpp"
#include "glfw_mappings.hpp"
#include "mosaic/utils/tsafe/queue.hpp"

namespace mosaic
{
namespace input
{

using KeyboardKeyInputData = int;

using MouseButtonInputData = int;

using MouseScrollInputData = glm::vec2;

using CursorPosInputData = glm::vec2;

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
    GLFWwindow* m_glfwWindow;
    static std::unordered_map<GLFWwindow*, RawInputHandler*> s_handlers;
    bool m_isActive;
    utils::tsafe::ThreadSafeQueue<MouseScrollInputData> m_mouseScrollQueue;

   public:
    RawInputHandler(GLFWwindow* _glfwWindow);
    ~RawInputHandler();

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
        return m_mouseScrollQueue.try_pop().value();
    }

   private:
    static void mouseScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset);
    static void windowFocusCallback(GLFWwindow* _window, int _focused);
};

} // namespace input
} // namespace mosaic
