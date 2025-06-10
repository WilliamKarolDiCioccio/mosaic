#include "agdk_platform.hpp"

namespace mosaic
{
namespace platform
{
namespace agdk
{

pieces::RefResult<core::Platform, std::string> AGDKPlatform::initialize()
{
    if (!getPlatformContext())
    {
        return pieces::ErrRef<core::Platform, std::string>("Platform context not available");
    }

    auto context = static_cast<AGDKPlatformContext*>(getPlatformContext());
    if (!context->window)
    {
        return pieces::ErrRef<core::Platform, std::string>("Android window not available");
    }

    auto result = m_app->initialize();

    if (result.isErr())
    {
        return pieces::ErrRef<core::Platform, std::string>(std::move(result.error()));
    }

    m_app->resume();

    return pieces::OkRef<core::Platform, std::string>(*this);
}

pieces::RefResult<core::Platform, std::string> AGDKPlatform::run()
{
    if (m_app->shouldExit())
    {
        auto context = static_cast<AGDKPlatformContext*>(getPlatformContext());

        if (context && context->app)
        {
            GameActivity_finish(context->app->activity);
        }

        return pieces::OkRef<core::Platform, std::string>(*this);
    }

    if (m_app->isResumed())
    {
        auto result = m_app->update();

        if (result.isErr())
        {
            return pieces::ErrRef<core::Platform, std::string>(std::move(result.error()));
        }
    }

    return pieces::OkRef<core::Platform, std::string>(*this);
}

void AGDKPlatform::pause() { m_app->pause(); }

void AGDKPlatform::resume()
{
    auto context = static_cast<AGDKPlatformContext*>(getPlatformContext());

    if (context && context->window)
    {
        m_app->resume();
    }
    else
    {
        MOSAIC_ERROR("AGDKPlatform: Cannot resume - no valid window");
    }
}

void AGDKPlatform::shutdown() { m_app->shutdown(); }

} // namespace agdk
} // namespace platform
} // namespace mosaic
