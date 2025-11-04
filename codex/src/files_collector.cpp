#include "codex/files_collector.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <future>
#include <string>
#include <utility>
#include <vector>

namespace codex
{

FilesCollector::FilesCollector(const std::vector<std::string>& _extensions,
                               const std::vector<std::string>& _paths, bool _recursive)
    : m_extensions(_extensions), m_paths(_paths), m_recursive(_recursive)
{
    // Normalize extensions to lowercase for consistency
    for (auto& ext : m_extensions)
    {
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }
}

std::vector<std::filesystem::path> FilesCollector::collect()
{
    std::vector<std::future<std::vector<std::filesystem::path>>> futures;
    std::vector<std::filesystem::path> results;

    for (const auto& path : m_paths)
    {
        futures.emplace_back(
            std::async(std::launch::async, [this, path] { return collectFromPath(path); }));
    }

    for (auto& future : futures)
    {
        auto fResult = future.get();
        results.insert(results.end(), fResult.begin(), fResult.end());
    }

    return results;
}

std::vector<std::filesystem::path> FilesCollector::collectFromPath(
    const std::filesystem::path& _basePath)
{
    if (!std::filesystem::exists(_basePath)) return {};

    auto options = std::filesystem::directory_options::skip_permission_denied;
    std::vector<std::filesystem::path> collectedFiles;

    if (m_recursive)
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(_basePath, options))
        {
            if (entry.is_regular_file() && hasValidExtension(entry.path()))
            {
                std::scoped_lock lock(m_mutex);
                collectedFiles.push_back(entry.path());
            }
        }
    }
    else
    {
        for (const auto& entry : std::filesystem::directory_iterator(_basePath, options))
        {
            if (entry.is_regular_file() && hasValidExtension(entry.path()))
            {
                std::scoped_lock lock(m_mutex);
                collectedFiles.push_back(entry.path());
            }
        }
    }

    return collectedFiles;
}

bool FilesCollector::hasValidExtension(const std::filesystem::path& _file) const
{
    auto ext = _file.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return std::find(m_extensions.begin(), m_extensions.end(), ext) != m_extensions.end();
}

} // namespace codex
