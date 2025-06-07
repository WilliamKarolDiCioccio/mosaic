#include "mosaic/core/application.hpp"

#include <thread>

using namespace std::chrono_literals;

namespace mosaic
{
namespace core
{

bool Application::s_created = false;

Application::Application(const std::string& _appName)
    : m_initialized(false),
      m_exitRequested(false),
      m_appName(_appName),
      m_state(ApplicationState::paused)

{
    assert(!s_created);

    s_created = true;
}

pieces::RefResult<Application, std::string> Application::initialize()
{
    assert(!m_initialized && "Application is already initialized!");

    MOSAIC_DEBUG("Initializing Mosaic {0} application", _MOSAIC_VERSION);

    m_initialized = true;

    onInitialize();

    return pieces::OkRef<Application, std::string>(*this);
}

pieces::RefResult<Application, std::string> Application::update()
{
    if (m_state != ApplicationState::resumed) std::this_thread::sleep_for(16ms);

    onUpdate();

    return pieces::OkRef<Application, std::string>(*this);
}

void Application::pause()
{
    assert(m_initialized && "Application is not initialized!");

    MOSAIC_DEBUG("Application paused");

    if (m_state == ApplicationState::paused) return;

    onPause();

    m_state = ApplicationState::paused;
}

void Application::resume()
{
    assert(m_initialized && "Application is not initialized!");

    MOSAIC_DEBUG("Application resumed");

    if (m_state != ApplicationState::paused) return;

    onResume();

    m_state = ApplicationState::resumed;
}

void Application::shutdown()
{
    assert(m_initialized && "Application is not initialized!");

    MOSAIC_DEBUG("Shutting down application");

    onShutdown();

    m_initialized = false;
}

} // namespace core
} // namespace mosaic
