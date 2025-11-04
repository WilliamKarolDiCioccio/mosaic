#pragma once

#include <memory>
#include <vector>
#include <string>
#include <filesystem>

#include "source.hpp"

namespace codex
{

class SourceExtractor
{
   public:
    std::vector<std::shared_ptr<Source>> extract(const std::vector<std::filesystem::path>& _paths);
};

} // namespace codex
