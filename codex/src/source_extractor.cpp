#include "codex/source_extractor.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <future>

namespace codex
{

class SingleSourceExtractor
{
   private:
    std::filesystem::path m_path;
    std::string m_sourceCode;

   public:
    SingleSourceExtractor(const std::filesystem::path& _path);

   public:
    std::shared_ptr<Source> extractSource();
};

SingleSourceExtractor::SingleSourceExtractor(const std::filesystem::path& _path) : m_path(_path) {}

std::shared_ptr<Source> SingleSourceExtractor::extractSource()
{
    std::ifstream file(m_path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << m_path << "\n";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    m_sourceCode = buffer.str();

    auto source = std::make_shared<Source>(
        m_path.filename().string(), m_path, m_sourceCode, "UTF-8",
        std::filesystem::last_write_time(m_path).time_since_epoch().count());

    return source;
}

std::vector<std::shared_ptr<Source>> SourceExtractor::extract(
    const std::vector<std::filesystem::path>& _paths)
{
    std::vector<std::future<std::shared_ptr<Source>>> futures;
    std::vector<std::shared_ptr<Source>> results;

    futures.reserve(_paths.size());
    results.reserve(_paths.size());

    for (const auto& path : _paths)
    {
        futures.emplace_back(std::async(std::launch::async, [this, path]()
                                        { return SingleSourceExtractor(path).extractSource(); }));
    }

    for (auto& future : futures)
    {
        try
        {
            auto fResult = future.get();
            if (fResult) results.emplace_back(fResult);
        }
        catch (const std::exception& e)
        {
            std::cout << "Error extracting source: " << e.what() << "\n";
        }
    }

    return results;
}

} // namespace codex
