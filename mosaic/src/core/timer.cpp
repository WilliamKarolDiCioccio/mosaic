#include "mosaic/core/timer.hpp"

namespace mosaic
{
namespace core
{

double Timer::s_lastTime = getCurrentTime();

double Timer::getCurrentTime()
{
    using namespace std::chrono;

    auto now = steady_clock::now();
    double seconds = duration_cast<duration<double>>(now.time_since_epoch()).count();
    return seconds;
}

std::string Timer::getCurrentDate()
{
    std::time_t now = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

double Timer::getDeltaTime()
{
    double now = getCurrentTime();
    double dt = now - s_lastTime;
    s_lastTime = now;
    return dt;
}

void Timer::sleepFor(std::chrono::duration<double> _seconds)
{
    std::this_thread::sleep_for(_seconds);
}

void Timer::scheduleCallback(std::chrono::duration<double> _delaySeconds,
                             std::function<void()> _callback)
{
    double triggerTime = getCurrentTime() + _delaySeconds.count();
    std::lock_guard<std::mutex> lock(m_mutex);
    m_callbacks.push_back({triggerTime, _callback});
}

void Timer::runScheduledCallbacks()
{
    using namespace std::chrono_literals;

    while (m_running)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            double now = getCurrentTime();

            for (auto it = m_callbacks.begin(); it != m_callbacks.end();)
            {
                if (now >= it->triggerTime)
                {
                    it->callback();
                    it = m_callbacks.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        sleepFor(0.1ms);
    }
}

} // namespace core
} // namespace mosaic
