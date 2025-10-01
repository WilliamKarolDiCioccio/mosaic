#pragma once

#include <string>
#include <filesystem>

namespace codex
{

struct Source
{
    std::string name;
    std::filesystem::path path;
    std::string sourceCode;
    std::string encoding;
    double lastModifiedTime = 0;

    Source(std::string _name, std::filesystem::path _path, std::string _sourceCode,
           std::string _encoding, double _lastModifiedTime)
        : name(std::move(_name)),
          path(std::move(_path)),
          sourceCode(std::move(_sourceCode)),
          encoding(std::move(_encoding)),
          lastModifiedTime(_lastModifiedTime) {};
};

} // namespace codex
