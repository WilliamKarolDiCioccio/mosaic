#include "mosaic/core/application.hpp"

#include <iostream>

#include "mosaic/core/logger.hpp"
#include "mosaic/core/tracer.hpp"
#include "mosaic/version.hpp"

namespace mosaic
{
namespace core
{

Application::Application(const std::string& _appName, const std::string& _logFilePath,
                         const std::string& _traceFilePath)
    : m_data(_appName, _logFilePath, _traceFilePath)
{
    LoggerManager::initialize(_appName, _logFilePath);
    TracerManager::initialize(_traceFilePath);
}

Application::~Application()
{
    if (m_state.isInitialized) shutdown();

    LoggerManager::shutdown();
    TracerManager::shutdown();
}

void Application::initialize()
{
    MOSAIC_BEGIN_TRACE("Application::initialize", TraceCategory::function);
    MOSAIC_INFO("Initializing Mosaic {0} application", _MOSAIC_VERSION);

    if (m_state.isInitialized) return;

    onInitialize();

    m_state.isInitialized = true;

    MOSAIC_END_TRACE();
}

void Application::realUpdate()
{
    if (!m_state.isInitialized) return;
    if (m_state.isPaused) return;

    onUpdate();
}

void Application::update()
{
#ifdef __EMSCRIPTEN__
    auto callback = [](void* arg)
    {
        Application* pApp = reinterpret_cast<Application*>(arg);

        pApp->realUpdate();
    };

    emscripten_set_main_loop_arg(callback, &app, 0, true);
#else
    while (m_state.isInitialized && !m_state.isPaused)
    {
        realUpdate();
    }
#endif
}

void Application::pause()
{
    MOSAIC_INFO("Application paused");

    if (!m_state.isInitialized)
        return;
    else if (m_state.isPaused)
        return;

    onPause();

    m_state.isPaused = true;
}

void Application::resume()
{
    MOSAIC_INFO("Application resumed");

    if (!m_state.isInitialized)
        return;
    else if (!m_state.isPaused)
        return;

    onResume();

    m_state.isPaused = false;
}

void Application::shutdown()
{
    if (!m_state.isInitialized) return;

    onShutdown();

    MOSAIC_INFO("Shutting down application");
    m_state.isInitialized = false;
}

} // namespace core
} // namespace mosaic
