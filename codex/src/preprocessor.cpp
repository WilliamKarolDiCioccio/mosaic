#include "codex/preprocessor.hpp"
#include <future>
#include <iostream>
#include <sstream>
#include <regex>

namespace codex
{

class SingleFilePreprocessor
{
   private:
    std::shared_ptr<Source> m_source;
    std::unordered_map<std::string, std::string> m_defines;

   private:
    std::string processLine(const std::string& line);
    bool isDefineLine(const std::string& line, const std::string& macroName);

   public:
    SingleFilePreprocessor(const std::shared_ptr<Source>& _source,
                           const std::unordered_map<std::string, std::string>& _externalMacros);

   public:
    void expand();
};

SingleFilePreprocessor::SingleFilePreprocessor(
    const std::shared_ptr<Source>& _source,
    const std::unordered_map<std::string, std::string>& _defines)
    : m_source(_source), m_defines(_defines)
{
}

bool SingleFilePreprocessor::isDefineLine(const std::string& line, const std::string& macroName)
{
    // Check if this line defines the macro
    std::regex defineRegex(R"(^\s*#\s*define\s+)" + macroName + R"(\s)");
    return std::regex_search(line, defineRegex);
}

std::string SingleFilePreprocessor::processLine(const std::string& line)
{
    std::string result = line;

    // Process each define
    for (const auto& [macroName, replacement] : m_defines)
    {
        // Skip if this line defines the macro
        if (isDefineLine(line, macroName))
        {
            continue;
        }

        // Create regex with word boundaries to match only complete identifiers
        std::string pattern = R"(\b)" + macroName + R"(\b)";
        std::regex macroRegex(pattern);

        // Replace all occurrences of the macro with its replacement
        result = std::regex_replace(result, macroRegex, replacement);
    }

    return result;
}

void SingleFilePreprocessor::expand()
{
    if (!m_source || m_source->content.empty())
    {
        return;
    }

    std::istringstream input(m_source->content);
    std::ostringstream output;
    std::string line;

    // Process line by line
    bool firstLine = true;
    while (std::getline(input, line))
    {
        if (!firstLine)
        {
            output << '\n';
        }
        firstLine = false;

        output << processLine(line);
    }

    m_source->content = output.str();
}

Preprocessor::Preprocessor(const std::unordered_map<std::string, std::string>& _defines)
    : m_defines(_defines)
{
}

void Preprocessor::expand(std::vector<std::shared_ptr<Source>>& _sources)
{
    std::vector<std::future<void>> futures;

    futures.reserve(_sources.size());

    for (auto& src : _sources)
    {
        futures.emplace_back(std::async(std::launch::async, [this, src]()
                                        { SingleFilePreprocessor(src, m_defines).expand(); }));
    }

    for (auto& future : futures)
    {
        future.get();
    }
}

} // namespace codex
