#pragma once

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
#define MOSAIC_PLATFORM_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
#define MOSAIC_PLATFORM_MACOS
#elif defined(__linux__)
#define MOSAIC_PLATFORM_LINUX
#else
#error "Unknown platform!"
#endif

// Import/Export macros
#if defined(MOSAIC_PLATFORM_WINDOWS)
#if defined(_MOSAIC_BUILD_DLL)
#define MOSAIC_API __declspec(dllexport)
#else
#define MOSAIC_API __declspec(dllimport)
#endif
#elif defined(MOSAIC_PLATFORM_MACOS) || defined(_MOSAIC_PLATFORM_LINUX)
#if defined(_MOSAIC_BUILD_DLL)
#define MOSAIC_API __attribute__((visibility("default")))
#else
#define MOSAIC_API
#endif
#else
#define MOSAIC_API
#endif

// Warning suppression macros
#if defined(MOSAIC_PLATFORM_WINDOWS)
#define MOSAIC_PUSH_WARNINGS __pragma(warning(push))
#define MOSAIC_POP_WARNINGS __pragma(warning(pop))
#define MOSAIC_DISABLE_ALL_WARNINGS __pragma(warning(push, 0))
#define MOSAIC_DISABLE_WARNING(warning) __pragma(warning(disable : warning))
#elif defined(MOSAIC_PLATFORM_MACOS) || defined(MOSAIC_PLATFORM_LINUX)
#define MOSAIC_PUSH_WARNINGS _Pragma("GCC diagnostic push")
#define MOSAIC_POP_WARNINGS _Pragma("GCC diagnostic pop")
#define MOSAIC_DISABLE_ALL_WARNINGS                                            \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wall\"") \
        _Pragma("GCC diagnostic ignored \"-Wextra\"") _Pragma("GCC diagnostic ignored \"-Wpedantic\"")
#define MOSAIC_DISABLE_WARNING(warning) _Pragma(MOSAIC_STRINGIFY(GCC diagnostic ignored warning))
#endif

// Helper macro for string conversion
#define MOSAIC_STRINGIFY_HELPER(x) #x
#define MOSAIC_STRINGIFY(x) MOSAIC_STRINGIFY_HELPER(x)
