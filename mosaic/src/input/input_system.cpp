#include "mosaic/input/input_system.hpp"

namespace mosaic
{
namespace input
{

InputContext* InputSystem::registerWindow(const graphics::Window* _window)
{
    auto glfwWindow = _window->getGLFWHandle();

    if (m_contexts.find(glfwWindow) != m_contexts.end())
    {
        MOSAIC_ERROR("InputSystem: Window already registered");
        return m_contexts[glfwWindow].get();
    }

    m_contexts[glfwWindow] = std::make_unique<InputContext>(_window);

    return m_contexts[glfwWindow].get();
}

void InputSystem::unregisterWindow(const graphics::Window* _window)
{
    auto glfwWindow = _window->getGLFWHandle();

    if (m_contexts.find(glfwWindow) != m_contexts.end())
    {
        m_contexts.erase(glfwWindow);
    }
}

} // namespace input
} // namespace mosaic
