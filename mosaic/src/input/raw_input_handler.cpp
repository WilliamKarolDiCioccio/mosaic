#include "mosaic/input/raw_input_handler.hpp"

namespace mosaic
{
namespace input
{

RawInputHandler::RawInputHandler(const core::Window* _window)
    : m_glfwWindow(_window->getGLFWHandle()),
      m_isActive(glfwGetWindowAttrib(m_glfwWindow, GLFW_FOCUSED))
{
    const_cast<core::Window*>(_window)->registerWindowScrollCallback(
        [this](double _xoffset, double _yoffset)
        {
            if (!this)
            {
                MOSAIC_ERROR("RawInputHandler: No handler found for window");
                return;
            }

            this->m_mouseScrollQueue.push(MouseScrollInputData(_xoffset, _yoffset));
        });

    const_cast<core::Window*>(_window)->registerWindowFocusCallback(
        [this](int _focused)
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
