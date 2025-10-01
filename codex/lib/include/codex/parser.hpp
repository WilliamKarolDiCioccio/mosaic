#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <tuple>

#include <tree_sitter/api.h>

#include "nodes.hpp"

namespace codex
{

class Parser
{
   public:
    std::vector<std::shared_ptr<SourceNode>> parse(
        const std::vector<std::shared_ptr<Source>>& _sources);
};

} // namespace codex
