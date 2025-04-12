#include "mosaic/input/raw_input_handler.hpp"

namespace mosaic
{
namespace input
{

std::unordered_map<GLFWwindow*, RawInputHandler*> RawInputHandler::s_handlers = {};

RawInputHandler::RawInputHandler(GLFWwindow* _glfwWindow)
    : m_glfwWindow(_glfwWindow), m_isActive(glfwGetWindowAttrib(_glfwWindow, GLFW_FOCUSED))
{
    glfwSetScrollCallback(m_glfwWindow, mouseScrollCallback);
    glfwSetWindowFocusCallback(m_glfwWindow, windowFocusCallback);

    s_handlers[m_glfwWindow] = this;
}

RawInputHandler::~RawInputHandler()
{
    glfwSetScrollCallback(m_glfwWindow, nullptr);
    glfwSetWindowFocusCallback(m_glfwWindow, nullptr);

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

void RawInputHandler::windowFocusCallback(GLFWwindow* _window, int _focused)
{
    if (s_handlers.find(_window) == s_handlers.end())
    {
        MOSAIC_ERROR("RawInputHandler: No handler found for window");
        return;
    }

    s_handlers.at(_window)->m_isActive = _focused;
}

} // namespace input
} // namespace mosaic
