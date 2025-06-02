#include "emscripten_platform.hpp"

namespace mosaic
{
namespace platform
{
namespace emscripten
{

pieces::RefResult<Platform, std::string> EmscriptenPlatform::initialize()
{
    if (!glfwInit())
    {
        return pieces::ErrRef<Platform, std::string>("Failed to initialize GLFW");
    }

    return pieces::OkRef<Platform, std::string>(*this);
}

void EmscriptenPlatform::update() { glfwPollEvents(); }

void EmscriptenPlatform::shutdown() { glfwTerminate(); }

void EmscriptenPlatform::showInfo(const std::string& message)
{
    EM_ASM({ alert(UTF8ToString($0)); }, message.c_str());
}

void EmscriptenPlatform::showWarning(const std::string& message)
{
    EM_ASM({ alert(UTF8ToString($0)); }, message.c_str());
}

void EmscriptenPlatform::showError(const std::string& message)
{
    EM_ASM({ alert(UTF8ToString($0)); }, message.c_str());
}

pieces::Result<int, std::string> EmscriptenPlatform::runShellCommand(
    const std::string& _command, const std::vector<std::string>& _args) const
{
    std::string fullCmd = _command;
    for (const auto& arg : _args) fullCmd += " " + arg;

    EM_ASM({ console.log("Attempted to run command: " + UTF8ToString($0)); }, fullCmd.c_str());

    return pieces::Err<int, std::string>(
        "Shell commands are not supported in the browser environment");
}

pieces::Result<int, std::string> EmscriptenPlatform::runProgram(
    const std::string& _programPath, const std::vector<std::string>& _args) const
{
    std::string fullCmd = _programPath;
    for (const auto& arg : _args) fullCmd += " " + arg;

    EM_ASM({ console.log("Attempted to run program: " + UTF8ToString($0)); }, fullCmd.c_str());

    return pieces::Err<int, std::string>(
        "Running programs is not supported in the browser environment");
}

bool EmscriptenPlatform::writeConfig(const std::string& key, const std::string& value) const
{
    EM_ASM(
        { localStorage.setItem(UTF8ToString($0), UTF8ToString($1)); }, key.c_str(), value.c_str());

    return true; // Assume success
}

pieces::Result<std::string, std::string> EmscriptenPlatform::readConfig(
    const std::string& key) const
{
    char* value = (char*)EM_ASM_INT(
        {
            var val = localStorage.getItem(UTF8ToString($0));
            if (val == = null) return 0;
            var buffer = _malloc(val.length + 1);
            stringToUTF8(val, buffer, val.length + 1);
            return buffer;
        },
        key.c_str());

    if (!value)
    {
        return pieces::Err<std::string, std::string>("Key not found: " + key);
    }

    std::string result(value);

    free(value);

    return pieces::Ok<std::string, std::string>(std::move(result));
}

} // namespace emscripten
} // namespace platform
} // namespace mosaic
