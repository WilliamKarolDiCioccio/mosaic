#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <mutex>

namespace codex
{

class FilesCollector final
{
   private:
    std::vector<std::string> m_extensions;
    std::vector<std::string> m_paths;
    bool m_recursive;

    mutable std::mutex m_mutex;

   public:
    FilesCollector(const std::vector<std::string>& _extensions,
                   const std::vector<std::string>& _paths, bool _recursive = true);

   public:
    std::vector<std::filesystem::path> collect();

   private:
    std::vector<std::filesystem::path> collectFromPath(const std::filesystem::path& _basePath);
    bool hasValidExtension(const std::filesystem::path& _file) const;
};

} // namespace codex
