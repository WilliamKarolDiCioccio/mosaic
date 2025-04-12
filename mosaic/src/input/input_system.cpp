#include "mosaic/input/input_system.hpp"

namespace mosaic
{
namespace input
{

InputContext* InputSystem::registerWindow(const graphics::Window& _window)
{
    auto glfwWindow = _window.getGLFWHandle();

    if (m_contexts.find(glfwWindow) == m_contexts.end())
    {
        m_contexts[glfwWindow] = std::make_unique<InputContext>(glfwWindow);
    }

    const auto& inputContext = m_contexts[glfwWindow];

    return inputContext.get();
}

void InputSystem::unregisterWindow(const graphics::Window& _window)
{
    auto glfwWindow = _window.getGLFWHandle();

    if (m_contexts.find(glfwWindow) != m_contexts.end())
    {
        m_contexts.erase(glfwWindow);
    }
}

} // namespace input
} // namespace mosaic
