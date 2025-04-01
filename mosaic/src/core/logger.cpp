#include "mosaic/core/logger.hpp"

MOSAIC_DISABLE_ALL_WARNINGS
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
MOSAIC_POP_WARNINGS

#include <chrono>
#include <ctime>

namespace mosaic
{
namespace core
{

bool LoggerManager::s_isInitialized = false;
std::shared_ptr<spdlog::logger> LoggerManager::s_instance = nullptr;

bool LoggerManager::initialize(const std::string& _loggerName,
                               const std::string& _filePath) noexcept
{
    if (s_instance != nullptr) return false;

    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto stderr_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();

    std::string timestamp =
        std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
    std::string log_filename = _filePath + "/log_" + timestamp + ".log";
    auto file_sink =
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_filename, 1048576 * 5, 3);

    stdout_sink->set_level(spdlog::level::info);
    stderr_sink->set_level(spdlog::level::err);
    file_sink->set_level(spdlog::level::info);

    std::vector<spdlog::sink_ptr> sinks{stdout_sink, stderr_sink, file_sink};
    s_instance = std::make_shared<spdlog::logger>(_loggerName, sinks.begin(), sinks.end());

    spdlog::set_default_logger(s_instance);
    spdlog::set_level(spdlog::level::info);
    spdlog::flush_on(spdlog::level::info);

    return true;
}

void LoggerManager::shutdown() noexcept
{
    if (!s_instance) return;

    spdlog::drop_all();
    s_instance = nullptr;
}

std::shared_ptr<spdlog::logger> LoggerManager::get()
{
    if (!s_instance)
        throw std::runtime_error("Logger not initialized. Call LoggerManager::initialize() first.");
    return s_instance;
}

} // namespace core
} // namespace mosaic
