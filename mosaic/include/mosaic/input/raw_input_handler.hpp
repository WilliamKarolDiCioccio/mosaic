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

    static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void cursorEnterCallback(GLFWwindow* window, int entered);

    inline bool isActive() const { return m_isActive; }

    inline int getKeyboardKeyInput(KeyboardKey _key) const
    {
        return glfwGetKey(m_glfwWindow, static_cast<int>(_key));
    }

    inline int getMouseButtonInput(MouseButton _button) const
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
};

} // namespace input
} // namespace mosaic
