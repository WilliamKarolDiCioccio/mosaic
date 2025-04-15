#include "mosaic/input/raw_input_handler.hpp"

namespace mosaic
{
namespace input
{

RawInputHandler::RawInputHandler(const graphics::Window* _window)
    : m_glfwWindow(_window->getGLFWHandle()),
      m_isActive(glfwGetWindowAttrib(m_glfwWindow, GLFW_FOCUSED))
{
    const_cast<graphics::Window*>(_window)->registerWindowScrollCallback(
        [this](GLFWwindow* _window, double _xoffset, double _yoffset)
        {
            if (!this)
            {
                MOSAIC_ERROR("RawInputHandler: No handler found for window");
                return;
            }

            this->m_mouseScrollQueue.push(MouseScrollInputData(_xoffset, _yoffset));
        });

    const_cast<graphics::Window*>(_window)->registerWindowFocusCallback(
        [this](GLFWwindow* _window, int _focused)
        {
            if (!this)
            {
                MOSAIC_ERROR("RawInputHandler: No handler found for window");
                return;
            }

            this->m_isActive = _focused == GLFW_TRUE;
        });
}

} // namespace input
} // namespace mosaic
