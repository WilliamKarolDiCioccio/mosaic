#include "mosaic/input/input_system.hpp"

namespace mosaic
{
namespace input
{

pieces::Result<InputContext*, std::string> InputSystem::registerWindow(const core::Window* _window)
{
    auto glfwWindow = _window->getGLFWHandle();

    if (m_contexts.find(glfwWindow) != m_contexts.end())
    {
        MOSAIC_WARN("InputSystem: Window already registered");
        return pieces::Ok<InputContext*, std::string>(m_contexts.at(glfwWindow).get());
    }

    m_contexts[glfwWindow] = std::make_unique<InputContext>(_window);

    return pieces::Ok<InputContext*, std::string>(m_contexts.at(glfwWindow).get());
}

void InputSystem::unregisterWindow(const core::Window* _window)
{
    auto glfwWindow = _window->getGLFWHandle();

    if (m_contexts.find(glfwWindow) != m_contexts.end())
    {
        m_contexts.erase(glfwWindow);
    }
}

} // namespace input
} // namespace mosaic
