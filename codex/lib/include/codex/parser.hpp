#pragma once

#include <string>

namespace codex
{

class Parser
{
   public:
    Parser();
    ~Parser();

    void parse(const std::string& input);
};

} // namespace codex