#pragma once

#include "mosaic/defines.hpp"

MOSAIC_DISABLE_ALL_WARNINGS
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <spdlog/spdlog.h>
#endif
MOSAIC_POP_WARNINGS

#include <memory>
#include <string>

namespace mosaic
{
namespace core
{

class MOSAIC_API LoggerManager final
{
   private:
    static bool s_isInitialized;

#ifndef MOSAIC_PLATFORM_EMSCRIPTEN
    static std::shared_ptr<spdlog::logger> s_instance;
#endif

   public:
    static bool initialize(const std::string& _filePath) noexcept;
    static void shutdown() noexcept;

#ifndef MOSAIC_PLATFORM_EMSCRIPTEN
    static std::shared_ptr<spdlog::logger> get();
#endif
};

} // namespace core
} // namespace mosaic

#ifdef __EMSCRIPTEN__

#ifdef _DEBUG
#define MOSAIC_TRACE(...) emscripten_log(EM_LOG_DEBUG, __VA_ARGS__)
#define MOSAIC_DEBUG(...) emscripten_log(EM_LOG_DEBUG, __VA_ARGS__)
#else
#define MOSAIC_TRACE(...) ((void)0)
#define MOSAIC_DEBUG(...) ((void)0)
#endif

#define MOSAIC_INFO(...) emscripten_log(EM_LOG_INFO, __VA_ARGS__)
#define MOSAIC_WARN(...) emscripten_log(EM_LOG_WARN, __VA_ARGS__)
#define MOSAIC_ERROR(...) emscripten_log(EM_LOG_ERROR, __VA_ARGS__)
#define MOSAIC_CRITICAL(...) emscripten_log(EM_LOG_ERROR, __VA_ARGS__)

#else

#ifdef _DEBUG
#define MOSAIC_TRACE(...) mosaic::core::LoggerManager::get()->trace(__VA_ARGS__)
#define MOSAIC_DEBUG(...) mosaic::core::LoggerManager::get()->debug(__VA_ARGS__)
#else
#define MOSAIC_TRACE(...) ((void)0)
#define MOSAIC_DEBUG(...) ((void)0)
#endif

#define MOSAIC_INFO(...) mosaic::core::LoggerManager::get()->info(__VA_ARGS__)
#define MOSAIC_WARN(...) mosaic::core::LoggerManager::get()->warn(__VA_ARGS__)
#define MOSAIC_ERROR(...) mosaic::core::LoggerManager::get()->error(__VA_ARGS__)
#define MOSAIC_CRITICAL(...) mosaic::core::LoggerManager::get()->critical(__VA_ARGS__)

#endif
