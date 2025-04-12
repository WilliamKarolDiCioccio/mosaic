#include "mosaic/input/raw_input_handler.hpp"

namespace mosaic
{
namespace input
{

std::unordered_map<GLFWwindow*, RawInputHandler*> RawInputHandler::s_handlers = {};

RawInputHandler::RawInputHandler(GLFWwindow* _glfwWindow)
    : m_glfwWindow(_glfwWindow), m_isActive(false)
{
    glfwSetCursorEnterCallback(m_glfwWindow, cursorEnterCallback);
    glfwSetScrollCallback(m_glfwWindow, mouseScrollCallback);
    glfwSetCharCallback(m_glfwWindow, nullptr);
    glfwSetDropCallback(m_glfwWindow, nullptr);

    s_handlers[m_glfwWindow] = this;
}

RawInputHandler::~RawInputHandler()
{
    glfwSetKeyCallback(m_glfwWindow, nullptr);
    glfwSetMouseButtonCallback(m_glfwWindow, nullptr);
    glfwSetCursorPosCallback(m_glfwWindow, nullptr);
    glfwSetScrollCallback(m_glfwWindow, nullptr);
    glfwSetCharCallback(m_glfwWindow, nullptr);
    glfwSetDropCallback(m_glfwWindow, nullptr);

    s_handlers.erase(m_glfwWindow);
}

void RawInputHandler::mouseScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset)
{
    if (s_handlers.find(_window) == s_handlers.end())
    {
        MOSAIC_ERROR("RawInputHandler: No handler found for window");
        return;
    }

    s_handlers.at(_window)->m_mouseScrollQueue.push(MouseScrollInputData(_xoffset, _yoffset));
}

void RawInputHandler::cursorEnterCallback(GLFWwindow* _window, int _entered)
{
    if (s_handlers.find(_window) == s_handlers.end())
    {
        MOSAIC_ERROR("RawInputHandler: No handler found for window");
        return;
    }

    s_handlers.at(_window)->m_isActive = _entered;
}

} // namespace input
} // namespace mosaic
