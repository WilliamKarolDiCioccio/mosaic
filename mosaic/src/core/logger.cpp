#include "mosaic/core/logger.hpp"

#include <chrono>
#include <ctime>
#include <cassert>

namespace mosaic
{
namespace core
{

pieces::RefResult<Sink, std::string> DefaultSink::initialize()
{
    return pieces::OkRef<Sink, std::string>(*this);
}

void DefaultSink::shutdown() {}

void DefaultSink::trace(const std::string& _message) const
{
    SystemConsole::printTrace(_message + '\n');
}

void DefaultSink::debug(const std::string& _message) const
{
    SystemConsole::printDebug(_message + '\n');
}

void DefaultSink::info(const std::string& _message) const
{
    SystemConsole::printInfo(_message + '\n');
}

void DefaultSink::warn(const std::string& _message) const
{
    SystemConsole::printWarn(_message + '\n');
}

void DefaultSink::error(const std::string& _message) const
{
    SystemConsole::printError(_message + '\n');
}

void DefaultSink::critical(const std::string& _message) const
{
    SystemConsole::printCritical(_message + '\n');
}

LoggerManager* LoggerManager::s_instance = nullptr;

LoggerManager::LoggerManager(const LoggerConfig& _config) : m_config(_config) {}

bool LoggerManager::initialize(const LoggerConfig& _config) noexcept
{
    if (s_instance) return false;

    s_instance = new LoggerManager(_config);

    return true;
}

void LoggerManager::shutdown() noexcept
{
    if (!s_instance) return;

    delete s_instance;
    s_instance = nullptr;
}

} // namespace core
} // namespace mosaic
