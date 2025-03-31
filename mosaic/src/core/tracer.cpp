#include "mosaic/core/tracer.hpp"

#include <system_error>

#include "mosaic/version.h"

namespace mosaic::core
{

bool TracerManager::initialize(const std::string& _tracesDir) noexcept
{
    if (s_isInitialized) return false;

    assert(!_tracesDir.empty() && "Traces directory path cannot be empty");

    std::lock_guard<std::mutex> lock(m_mutex);

    std::filesystem::path tracesDir(_tracesDir);
    if (!std::filesystem::exists(tracesDir)) std::filesystem::create_directories(tracesDir);

    auto timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::ostringstream oss;
    oss << _tracesDir << "/trace_" << timestamp << ".json";
    m_tracesPath = oss.str();

    m_data = {
        {"otherData", nlohmann::json::object()},
        {"traceEvents", nlohmann::json::array()},
        {"version", "1.0"},
        {"engineVersion", _IGE_VERSION},
        {"startTime", timestamp},
    };

    std::ofstream test_file(m_tracesPath);
    if (!test_file) return false;
    test_file.close();

    s_isInitialized = true;

    return true;
}

void TracerManager::shutdown() noexcept
{
    if (!s_isInitialized) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    m_data["endTime"] = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::ofstream file(m_tracesPath);
    if (file)
    {
        file << m_data.dump(4);
        file.close();
    }

    m_data.clear();

    while (!m_activeTraces.empty()) m_activeTraces.pop();
}

void TracerManager::beginTrace(const std::string& _name, TraceCategory _category) noexcept
{
    assert(s_isInitialized && "TracerManager must be initialized before use");
    assert(!_name.empty() && "Trace name cannot be empty");

    auto threadID = std::this_thread::get_id();
    const auto start = std::chrono::high_resolution_clock::now();

    Trace trace = {
        .category = _category,
        .name = _name,
        .threadID = std::hash<std::thread::id>{}(threadID),
        .start = std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count(),
    };

    std::lock_guard<std::mutex> lock(m_mutex);
    m_activeTraces.push(trace);
}

void TracerManager::endTrace() noexcept
{
    assert(s_isInitialized && "TracerManager must be initialized before use");

    std::lock_guard<std::mutex> lock(m_mutex);

    assert(!m_activeTraces.empty() && "No active trace to end");
    if (m_activeTraces.empty()) return;

    auto& trace = m_activeTraces.top();
    const auto end = std::chrono::high_resolution_clock::now();
    trace.end = std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count();

    nlohmann::json traceEvent = {
        {"cat", m_categories[(uint8_t)trace.category]},
        {"dur", trace.end - trace.start},
        {"name", trace.name},
        {"ph", "X"},
        {"pid", 0},
        {"tid", trace.threadID},
        {"ts", trace.start},
    };

    m_data["traceEvents"].push_back(traceEvent);
    m_activeTraces.pop();
}

} // namespace mosaic::core
