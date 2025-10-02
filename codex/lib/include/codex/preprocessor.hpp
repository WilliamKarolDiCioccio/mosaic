#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <filesystem>

#include "source.hpp"

namespace codex
{

class Preprocessor
{
   private:
    std::unordered_map<std::string, std::string> m_defines;

   public:
    Preprocessor(const std::unordered_map<std::string, std::string>& _defines = {});

   public:
    void expand(std::vector<std::shared_ptr<Source>>& _sources);
};

} // namespace codex
