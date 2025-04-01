#pragma once

#include "mosaic/defines.hpp"

MOSAIC_DISABLE_ALL_WARNINGS
#include <spdlog/spdlog.h>
MOSAIC_POP_WARNINGS

#include <memory>
#include <string>

namespace mosaic
{
namespace core
{

class LoggerManager final
{
   private:
    MOSAIC_API static bool s_isInitialized;
    MOSAIC_API static std::shared_ptr<spdlog::logger> s_instance;

   public:
    static bool initialize(const std::string& _loggerName, const std::string& _filePath) noexcept;
    static void shutdown() noexcept;

    MOSAIC_API static std::shared_ptr<spdlog::logger> get();
};

} // namespace core
} // namespace mosaic

#define MOSAIC_TRACE(...) mosaic::core::LoggerManager::get()->trace(__VA_ARGS__)
#define MOSAIC_DEBUG(...) mosaic::core::LoggerManager::get()->debug(__VA_ARGS__)
#define MOSAIC_INFO(...) mosaic::core::LoggerManager::get()->info(__VA_ARGS__)
#define MOSAIC_WARN(...) mosaic::core::LoggerManager::get()->warn(__VA_ARGS__)
#define MOSAIC_ERROR(...) mosaic::core::LoggerManager::get()->error(__VA_ARGS__)
#define MOSAIC_CRITICAL(...) mosaic::core::LoggerManager::get()->critical(__VA_ARGS__)
