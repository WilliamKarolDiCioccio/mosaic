#pragma once

#include <memory>

#include "core/application.hpp"

namespace mosaic
{

template <typename AppType>
concept IsApplication =
    std::derived_from<AppType, core::Application> && !std::is_abstract_v<AppType>;

template <typename AppType, typename... Args>
    requires IsApplication<AppType>
int runApp(Args&&... args)
{
    auto app = std::make_unique<AppType>(std::forward<Args>(args)...);

    auto result = app->initialize().andThen(std::mem_fn(&core::Application::run));

    if (result.isErr())
    {
        MOSAIC_ERROR("Failed to initialize the application: {0}", result.error().c_str());
        return 1;
    }

    return 0;
}

} // namespace mosaic

#pragma once

#if defined(MOSAIC_PLATFORM_WINDOWS)

#include <windows.h>

#if defined(MOSAIC_DEBUG_BUILD) || defined(MOSAIC_DEV_BUILD)

#define MOSAIC_ENTRY_POINT(AppType, ...)                                           \
    int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, \
                       _In_ PSTR lpCmdLine, _In_ int nCmdShow)                     \
    {                                                                              \
        AllocConsole();                                                            \
                                                                                   \
        FILE* fp;                                                                  \
        freopen_s(&fp, "CONOUT$", "w", stdout);                                    \
        freopen_s(&fp, "CONOUT$", "w", stderr);                                    \
        freopen_s(&fp, "CONIN$", "r", stdin);                                      \
        std::ios::sync_with_stdio(true);                                           \
                                                                                   \
        return mosaic::runApp<AppType>(__VA_ARGS__);                               \
                                                                                   \
        FreeConsole();                                                             \
    }

#else

#define MOSAIC_ENTRY_POINT(AppType, ...)                                           \
    int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, \
                       _In_ PSTR lpCmdLine, _In_ int nCmdShow)                     \
    {                                                                              \
        return mosaic::runApp<AppType>(__VA_ARGS__);                               \
    }

#endif

#elif defined(MOSAIC_PLATFORM_LINUX) || defined(MOSAIC_PLATFORM_MACOS) || \
    defined(MOSAIC_PLATFORM_EMSCRIPTEN)

#define MOSAIC_ENTRY_POINT(AppType, ...) \
    int main(int argc, char** argv) { return mosaic::runApp<AppType>(__VA_ARGS__); }

#elif defined(MOSAIC_PLATFORM_ANDROID)

#define MOSAIC_ENTRY_POINT(AppType, ...)

#endif
