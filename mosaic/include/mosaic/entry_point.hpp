#pragma once

#include <memory>

#include <mosaic/core/logger.hpp>
#include <mosaic/core/tracer.hpp>
#include <mosaic/core/application.hpp>
#include <mosaic/core/platform.hpp>

#ifndef MOSAIC_PLATFORM_ANDROID

namespace mosaic
{

template <typename AppType, typename... Args>
    requires core::IsApplication<AppType>
int runApp(Args&&... args)
{
    core::LoggerManager::initialize();

    core::LoggerManager::getInstance()->addSink<core::DefaultSink>("default", core::DefaultSink());

    // This scope guard ensures all resources have been disposed before shutting down the logger and
    // tracer.

    {
        auto app = std::make_unique<AppType>(std::forward<Args>(args)...);

        auto platform = core::Platform::create(app.get());

        auto result = platform->initialize().andThen(std::mem_fn(&core::Platform::run));

        if (result.isErr())
        {
            MOSAIC_ERROR(result.error().c_str());
            return 1;
        }

        platform->shutdown();
    }

    core::LoggerManager::shutdown();

    return 0;
}

} // namespace mosaic

#endif

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
