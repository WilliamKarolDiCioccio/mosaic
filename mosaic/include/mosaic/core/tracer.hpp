#pragma once

#include <atomic>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <stack>
#include <stdexcept>
#include <string>
#include <thread>

#include "mosaic/defines.hpp"

MOSAIC_DISABLE_ALL_WARNINGS
#include <nlohmann/json.hpp>
MOSAIC_POP_WARNINGS

namespace mosaic::core
{

enum class TraceCategory
{
    function,
    scope,
};

struct Trace
{
    TraceCategory category;
    std::string name;
    size_t threadID;
    int64_t start, end;
};

class TracerManager final
{
   private:
    MOSAIC_API static inline bool s_isInitialized = false;

    static inline std::array<std::string, 2> m_categories = {"function", "scope"};
    static inline nlohmann::json m_data;
    static inline std::string m_tracesPath;
    static inline std::stack<Trace> m_activeTraces;
    static inline std::mutex m_mutex;

   private:
    TracerManager() = delete;

    static void ensureDirectoryExists(const std::string& path);

   public:
    static bool initialize(const std::string& _tracesDir) noexcept;
    static void shutdown() noexcept;
    static void beginTrace(const std::string& _name, TraceCategory _category) noexcept;
    static void endTrace() noexcept;

    MOSAIC_API static std::shared_ptr<TracerManager> get();
};

} // namespace mosaic::core

#define MOSAIC_BEGIN_TRACE(name, category) mosaic::core::TracerManager::beginTrace(name, category)
#define MOSAIC_END_TRACE() mosaic::core::TracerManager::endTrace()
