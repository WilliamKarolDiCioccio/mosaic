#include "mosaic/core/application.hpp"

#include <thread>
#include <chrono>

using namespace std::chrono_literals;

namespace mosaic
{
namespace core
{

bool Application::s_created = false;

Application::Application(const std::string& _appName)
    : m_exitRequested(false), m_appName(_appName), m_state(ApplicationState::uninitialized)

{
    assert(!s_created && "Application instance already exists!");

    s_created = true;
}

pieces::RefResult<Application, std::string> Application::initialize()
{
    if (m_state != ApplicationState::uninitialized)
    {
        return pieces::ErrRef<Application, std::string>("Application already initialized");
    }

    try
    {
        onInitialize();
        m_state = ApplicationState::initialized;
        return pieces::OkRef<Application, std::string>(*this);
    }
    catch (const std::exception& e)
    {
        return pieces::ErrRef<Application, std::string>(e.what());
    }
}

pieces::RefResult<Application, std::string> Application::update()
{
    if (m_state != ApplicationState::resumed)
    {
        return pieces::OkRef<Application, std::string>(*this); // Skip update when not resumed
    }

    try
    {
        onUpdate();
        return pieces::OkRef<Application, std::string>(*this);
    }
    catch (const std::exception& e)
    {
        return pieces::ErrRef<Application, std::string>(e.what());
    }
}

void Application::pause()
{
    if (m_state == ApplicationState::resumed)
    {
        onPause();
        m_state = ApplicationState::paused;
    }
}

void Application::resume()
{
    if (m_state == ApplicationState::initialized || m_state == ApplicationState::paused)
    {
        onResume();
        m_state = ApplicationState::resumed;
    }
}

void Application::shutdown()
{
    if (m_state != ApplicationState::shutdown)
    {
        onShutdown();
        m_state = ApplicationState::shutdown;
    }
}

} // namespace core
} // namespace mosaic
