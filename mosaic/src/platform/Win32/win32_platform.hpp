#pragma once

#include "mosaic/core/platform.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <windows.h>
#include <shlobj.h>
#include <fmt/format.h>

namespace mosaic
{
namespace platform
{
namespace win32
{

class Win32Platform : public Platform
{
   public:
    Win32Platform() = default;
    ~Win32Platform() override = default;

   public:
    pieces::RefResult<Platform, std::string> initialize() override;
    void update() override;
    void shutdown() override;

    // Messages boxes / alert dialogs

    void showInfo(const std::string& _message) override;
    void showWarning(const std::string& _message) override;
    void showError(const std::string& _message) override;

    // Shell commands

    pieces::Result<int, std::string> runShellCommand(
        const std::string& _command, const std::vector<std::string>& _args) const override;
    pieces::Result<int, std::string> runProgram(
        const std::string& _programPath, const std::vector<std::string>& _args) const override;

    // Configuration management

    bool writeConfig(const std::string& key, const std::string& value) const override;
    pieces::Result<std::string, std::string> readConfig(const std::string& key) const override;
};

} // namespace win32
} // namespace platform
} // namespace mosaic
