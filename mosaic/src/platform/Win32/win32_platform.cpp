#include "win32_platform.hpp"

#include <GLFW/glfw3.h>
#include <VersionHelpers.h>

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")

namespace mosaic
{
namespace platform
{
namespace win32
{

pieces::RefResult<core::Platform, std::string> Win32Platform::initialize()
{
    if (!glfwInit())
    {
        return pieces::ErrRef<Platform, std::string>("Failed to initialize GLFW");
    }

    auto result = m_app->initialize();

    if (result.isErr())
    {
        return pieces::ErrRef<core::Platform, std::string>(std::move(result.error()));
    }

    return pieces::OkRef<Platform, std::string>(*this);
}

pieces::RefResult<core::Platform, std::string> Win32Platform::run()
{
    while (!m_app->shouldExit())
    {
        glfwPollEvents();

        m_app->update();
    }

    return pieces::OkRef<core::Platform, std::string>(*this);
}

void Win32Platform::pause() {}

void Win32Platform::resume() {}

void Win32Platform::shutdown()
{
    m_app->shutdown();

    glfwTerminate();
}

} // namespace win32
} // namespace platform
} // namespace mosaic
