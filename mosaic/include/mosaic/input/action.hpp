#pragma once

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

#include "glfw_mappings.hpp"
#include "events.hpp"

namespace mosaic
{
namespace input
{

struct KeyboardKeyActionTrigger
{
    std::vector<std::string> requiredVirtualKeys;
    KeyboardKey key;
    std::function<bool(const std::unordered_map<std::string, KeyboardKeyEvent>&)> callback;
};

struct MouseButtonActionTrigger
{
    std::vector<std::string> requiredVirtualKeys;
    MouseButton button;
    std::function<bool(const std::unordered_map<std::string, MouseButtonEvent>&)> callback;
};

struct MouseCursorPosActionTrigger
{
    std::function<bool(const MouseCursorPosEvent&)> callback;
};

struct MouseWheelScrollActionTrigger
{
    std::function<bool(const MouseWheelScrollEvent&)> callback;
};

using ActionTrigger = std::variant<KeyboardKeyActionTrigger, MouseButtonActionTrigger,
                                   MouseCursorPosActionTrigger, MouseWheelScrollActionTrigger>;

using Action = std::vector<ActionTrigger>;

} // namespace input
} // namespace mosaic
