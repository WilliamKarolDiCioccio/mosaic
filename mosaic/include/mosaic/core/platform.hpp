#pragma once

#include <string>
#include <memory>

#include <pieces/result.hpp>

#include "mosaic/defines.hpp"
#include "mosaic/core/logger.hpp"
#include "mosaic/core/tracer.hpp"

namespace mosaic
{
namespace platform
{

class MOSAIC_API Platform
{
   public:
    virtual ~Platform() = default;

    static std::unique_ptr<Platform> create();

   public:
    virtual pieces::RefResult<Platform, std::string> initialize() = 0;
    virtual void update() = 0;
    virtual void shutdown() = 0;

    // Messages boxes / alert dialogs

    virtual void showInfo(const std::string& _message) = 0;
    virtual void showWarning(const std::string& _message) = 0;
    virtual void showError(const std::string& _message) = 0;

    // Shell commands

    virtual int runShellCommand(const std::string& _command,
                                const std::vector<std::string>& _args) const = 0;
    virtual int runProgram(const std::string& _programPath,
                           const std::vector<std::string>& _args) const = 0;

    // Configuration management

    virtual bool writeConfig(const std::string& key, const std::string& value) const = 0;
    virtual pieces::Result<std::string, std::string> readConfig(const std::string& key) const = 0;
};

} // namespace platform
} // namespace mosaic
