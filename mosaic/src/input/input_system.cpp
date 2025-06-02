#include "mosaic/input/input_system.hpp"

namespace mosaic
{
namespace input
{

pieces::Result<InputContext*, std::string> InputSystem::registerWindow(core::Window* _window)
{
    auto nativeHandle = _window->getNativeHandle();

    if (m_contexts.find(nativeHandle) != m_contexts.end())
    {
        MOSAIC_WARN("InputSystem: Window already registered");
        return pieces::Ok<InputContext*, std::string>(m_contexts.at(nativeHandle).get());
    }

    m_contexts[nativeHandle] = std::make_unique<InputContext>(_window);

    return pieces::Ok<InputContext*, std::string>(m_contexts.at(nativeHandle).get());
}

void InputSystem::unregisterWindow(core::Window* _window)
{
    auto nativeHandle = _window->getNativeHandle();

    if (m_contexts.find(nativeHandle) != m_contexts.end())
    {
        m_contexts.erase(nativeHandle);
    }
}

} // namespace input
} // namespace mosaic
