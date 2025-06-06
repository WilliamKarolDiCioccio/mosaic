#include "mosaic/core/platform.hpp"

#if defined(MOSAIC_PLATFORM_WINDOWS)
#include "platform/Win32/win32_platform.hpp"
#elif defined(MOSAIC_PLATFORM_EMSCRIPTEN)
#include "platform/emscripten/emscripten_platform.hpp"
#endif

namespace mosaic
{
namespace platform
{

std::unique_ptr<Platform> Platform::create()
{
#if defined(MOSAIC_PLATFORM_WINDOWS)
    return std::make_unique<platform::win32::Win32Platform>();
#elif defined(MOSAIC_PLATFORM_EMSCRIPTEN)
    return std::make_unique<platform::emscripten::EmscriptenPlatform>();
#elif defined(MOSAIC_PLATFORM_ANDROID)
    return nullptr; // TODO: Implement platform::agdk::AGDKPlatform
#endif
}

} // namespace platform
} // namespace mosaic
