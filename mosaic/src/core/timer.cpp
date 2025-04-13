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

ScheduledCallback Timer::scheduleCallback(std::chrono::duration<double> _delaySeconds,
                                          std::function<void()> _callback)
{
    double triggerTime = getCurrentTime() + _delaySeconds.count();
    std::lock_guard<std::mutex> lock(m_mutex);
    m_callbacks.push_back({triggerTime, _callback});
    return m_callbacks.back();
}

void Timer::cancelCallback(const uuids::uuid& _uuid)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find_if(m_callbacks.begin(), m_callbacks.end(),
                           [&_uuid](const ScheduledCallback& cb) { return cb.uuid == _uuid; });
    if (it != m_callbacks.end())
    {
        it->cancelled = true;
    }
}

void Timer::runScheduledCallbacks()
{
    using namespace std::chrono_literals;

    while (m_running)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            double now = getCurrentTime();

            std::vector<size_t> callbacksToRemove;

            for (size_t i = 0; i < m_callbacks.size(); ++i)
            {
                if (m_callbacks[i].cancelled)
                {
                    callbacksToRemove.push_back(i);
                }
                else if (now >= m_callbacks[i].triggerTime)
                {
                    m_callbacks[i].callback();
                    callbacksToRemove.push_back(i);
                }
            }

            std::sort(callbacksToRemove.begin(), callbacksToRemove.end(), std::greater<size_t>());

            for (auto idx : callbacksToRemove)
            {
                m_callbacks.erase(m_callbacks.begin() + idx);
            }
        }

        sleepFor(0.1ms);
    }
}

} // namespace core
} // namespace mosaic
