#include "mosaic/core/platform.hpp"

#if defined(MOSAIC_PLATFORM_WINDOWS)
#include "platform/Win32/win32_platform.hpp"
#elif defined(MOSAIC_PLATFORM_EMSCRIPTEN)
#include "platform/emscripten/emscripten_platform.hpp"
#elif defined(MOSAIC_PLATFORM_ANDROID)
#include "platform/AGDK/agdk_platform.hpp"
#endif

namespace mosaic
{
namespace core
{

Platform* Platform::s_instance = nullptr;

Platform::Platform(Application* _app) : m_app(_app)
{
    assert(s_instance != nullptr && "Platform instance already exists!");

    s_instance = this;
}

std::unique_ptr<Platform> Platform::create(Application* _app)
{
#if defined(MOSAIC_PLATFORM_WINDOWS)
    return std::make_unique<platform::win32::Win32Platform>(_app);
#elif defined(MOSAIC_PLATFORM_EMSCRIPTEN)
    return std::make_unique<platform::emscripten::EmscriptenPlatform>(_app);
#elif defined(MOSAIC_PLATFORM_ANDROID)
    return std::make_unique<platform::agdk::AGDKPlatform>(_app)
#endif
}

} // namespace core
} // namespace mosaic
