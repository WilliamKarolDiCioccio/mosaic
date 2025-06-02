#pragma once

#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include "mosaic/core/platform.hpp"

namespace mosaic
{
namespace platform
{
namespace emscripten
{

class EmscriptenPlatform : public Platform
{
   public:
    EmscriptenPlatform() = default;
    ~EmscriptenPlatform() override = default;

   public:
    pieces::RefResult<Platform, std::string> initialize() override;
    void update() override;
    void shutdown() override;

    // Messages boxes / alert dialogs

    void showInfo(const std::string& message) override;
    void showWarning(const std::string& message) override;
    void showError(const std::string& message) override;

    // Shell commands

    pieces::Result<int, std::string> runShellCommand(
        const std::string& _command, const std::vector<std::string>& _args) const override;
    pieces::Result<int, std::string> runProgram(
        const std::string& _programPath, const std::vector<std::string>& _args) const override;

    // Configuration management

    bool writeConfig(const std::string& key, const std::string& value) const override;
    pieces::Result<std::string, std::string> readConfig(const std::string& key) const override;
};

} // namespace emscripten
} // namespace platform
} // namespace mosaic
