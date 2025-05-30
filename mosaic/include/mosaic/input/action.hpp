#pragma once

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

#include "input_mappings.hpp"
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

/**
 * @brief This using declares a variant type that can hold any of the action trigger types.
 *
 * This allows for a lightweight way to handle different types of action triggers without needing to
 * create a class hierarchy or use polymorphism.
 */
using ActionTrigger = std::variant<KeyboardKeyActionTrigger, MouseButtonActionTrigger,
                                   MouseCursorPosActionTrigger, MouseWheelScrollActionTrigger>;

/**
 * @brief An action can be simply defined as a vector of action triggers that all need to be
 * satisfied in order to trigger the action.
 */
using Action = std::vector<ActionTrigger>;

} // namespace input
} // namespace mosaic
