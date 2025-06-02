#include "win32_platform.hpp"

#include <GLFW/glfw3.h>
#include <VersionHelpers.h>

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")

namespace mosaic
{
namespace platform
{
namespace win32
{

pieces::RefResult<Platform, std::string> Win32Platform::initialize()
{
    if (!glfwInit())
    {
        return pieces::ErrRef<Platform, std::string>("Failed to initialize GLFW");
    }

    return pieces::OkRef<Platform, std::string>(*this);
}

void Win32Platform::update() { glfwPollEvents(); }

void Win32Platform::shutdown() { glfwTerminate(); }

void Win32Platform::showInfo(const std::string& message)
{
    MessageBoxA(nullptr, message.c_str(), "Info", MB_OK | MB_ICONINFORMATION);
}

void Win32Platform::showWarning(const std::string& message)
{
    MessageBoxA(nullptr, message.c_str(), "Warning", MB_OK | MB_ICONWARNING);
}

void Win32Platform::showError(const std::string& message)
{
    MessageBoxA(nullptr, message.c_str(), "Error", MB_OK | MB_ICONERROR);
}

pieces::Result<int, std::string> Win32Platform::runShellCommand(
    const std::string& _command, const std::vector<std::string>& _args) const
{
    std::ostringstream oss;

    oss << _command;

    for (const auto& arg : _args) oss << " " << arg;

    std::string fullCmd = "cmd.exe /C \"" + oss.str() + "\"";

    STARTUPINFOA si = {sizeof(STARTUPINFOA)};
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi = {};

    BOOL success = CreateProcessA(nullptr,        // No module name (use command line)
                                  fullCmd.data(), // Command line
                                  nullptr,        // Process handle not inheritable
                                  nullptr,        // Thread handle not inheritable
                                  FALSE,          // Set handle inheritance to FALSE
                                  0,              // No creation flags
                                  nullptr,        // Use parent's environment block
                                  nullptr,        // Use parent's starting directory
                                  &si,            // Pointer to STARTUPINFO structure
                                  &pi             // Pointer to PROCESS_INFORMATION structure
    );

    if (!success)
    {
        pieces::Err<int, std::string>(
            fmt::format("Failed to run command: {} Error: {}", fullCmd.c_str(), GetLastError()));
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return pieces::Ok<int, std::string>(static_cast<int>(exitCode));
}

pieces::Result<int, std::string> Win32Platform::runProgram(
    const std::string& _programPath, const std::vector<std::string>& _args) const
{
    std::ostringstream oss;

    oss << '\"' << _programPath << '\"';

    for (const auto& arg : _args) oss << ' ' << arg;

    std::string cmdLine = oss.str();

    STARTUPINFOA si = {sizeof(STARTUPINFOA)};
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi = {};

    BOOL success = CreateProcessA(nullptr, &cmdLine[0], nullptr, nullptr, FALSE, 0, nullptr,
                                  nullptr, &si, &pi);

    if (!success)
    {
        pieces::Err<int, std::string>(
            fmt::format("Failed to create process: {} Error: {}", cmdLine, GetLastError()));
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return pieces::Ok<int, std::string>(static_cast<int>(exitCode));
}

constexpr const char* REGISTRY_BASE_PATH = "Software\\Mosaic";

bool Win32Platform::writeConfig(const std::string& key, const std::string& value) const
{
    HKEY hKey;

    if (RegCreateKeyExA(HKEY_CURRENT_USER, REGISTRY_BASE_PATH, 0, nullptr, REG_OPTION_NON_VOLATILE,
                        KEY_SET_VALUE, nullptr, &hKey, nullptr) != ERROR_SUCCESS)
    {
        return false;
    }

    LONG result =
        RegSetValueExA(hKey, key.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(value.c_str()),
                       static_cast<DWORD>(value.size() + 1) // +1 for null terminator
        );

    RegCloseKey(hKey);

    return (result == ERROR_SUCCESS);
}

pieces::Result<std::string, std::string> Win32Platform::readConfig(const std::string& key) const
{
    HKEY hKey;

    if (RegOpenKeyExA(HKEY_CURRENT_USER, REGISTRY_BASE_PATH, 0, KEY_QUERY_VALUE, &hKey) !=
        ERROR_SUCCESS)
    {
        return pieces::Err<std::string, std::string>("Failed to open registry key: " + key);
    }

    char buffer[512];
    DWORD bufferSize = sizeof(buffer);
    DWORD type = 0;

    LONG result =
        RegGetValueA(hKey, nullptr, key.c_str(), RRF_RT_REG_SZ, &type, buffer, &bufferSize);

    RegCloseKey(hKey);

    if (result == ERROR_SUCCESS)
    {
        return pieces::Ok<std::string, std::string>(
            std::string(buffer, bufferSize - 1)); // Exclude null terminator
    }

    return pieces::Err<std::string, std::string>("Failed to read registry value: " + key +
                                                 " Error: " + std::to_string(result));
}

} // namespace win32
} // namespace platform
} // namespace mosaic
