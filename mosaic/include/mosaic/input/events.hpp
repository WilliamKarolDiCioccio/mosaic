#pragma once

#include <chrono>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>

#include "mosaic/utils/enum.hpp"
#include "mosaic/utils/sized_queue.hpp"

namespace mosaic
{
namespace input
{

using namespace std::chrono_literals;

// Keyboard keys and mouse buttons share the same possible states, so we can use the same enum.
enum class KeyButtonState : uint32_t
{
    none = 0,
    release = 1 << 0,
    press = 1 << 1,
    hold = 1 << 2,
    double_press = 1 << 3
};

MOSAIC_DEFINE_ENUM_FLAGS_OPERATORS(KeyButtonState)

/**
 * @brief The `InputEventMetadata` struct contains meta-data for input events.
 *
 * It's the `InputArena`'s responsibility to fill this struct with the correct data.
 *
 * @see InputArena
 */
struct InputEventMetadata
{
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    std::chrono::duration<double> duration;

    InputEventMetadata(std::chrono::time_point<std::chrono::high_resolution_clock> _now,
                       std::chrono::duration<double> _duration)
        : timestamp(_now), duration(_duration) {};
};

struct KeyboardKeyEvent
{
    InputEventMetadata metadata;
    KeyButtonState state;
    KeyButtonState lastSignificantState;

    KeyboardKeyEvent()
        : state(KeyButtonState::none),
          lastSignificantState(KeyButtonState::none),
          metadata(std::chrono::high_resolution_clock::now(), 0ms) {};

    KeyboardKeyEvent(KeyButtonState _state, KeyButtonState _lastSignificantState,
                     std::chrono::time_point<std::chrono::high_resolution_clock> _now,
                     std::chrono::duration<double> _duration = 0ms)
        : state(_state), lastSignificantState(_lastSignificantState), metadata(_now, _duration) {};
};

struct MouseButtonEvent
{
    InputEventMetadata metadata;
    KeyButtonState state;
    KeyButtonState lastSignificantState;

    MouseButtonEvent()
        : state(KeyButtonState::none),
          lastSignificantState(KeyButtonState::none),
          metadata(std::chrono::high_resolution_clock::now(), 0ms) {};

    MouseButtonEvent(KeyButtonState _state, KeyButtonState _lastSignificantState,
                     std::chrono::time_point<std::chrono::high_resolution_clock> _now,
                     std::chrono::duration<double> _duration = 0ms)
        : state(_state), lastSignificantState(_lastSignificantState), metadata(_now, _duration) {};
};

struct MouseCursorPosEvent
{
    InputEventMetadata metadata;
    glm::vec2 rawPos;
    glm::vec2 lastRawPos;

    MouseCursorPosEvent()
        : rawPos(0.0f),
          lastRawPos(0.0f),
          metadata(std::chrono::high_resolution_clock::now(), 0ms) {};

    MouseCursorPosEvent(glm::vec2 _rawPos, glm::vec2 _lastRawPos,
                        std::chrono::time_point<std::chrono::high_resolution_clock> _now,
                        std::chrono::duration<double> _duration = 0ms)
        : rawPos(_rawPos), lastRawPos(_lastRawPos), metadata(_now, _duration) {};
};

struct MouseWheelScrollEvent
{
    InputEventMetadata metadata;
    glm::vec2 rawScrollOffset;

    MouseWheelScrollEvent()
        : rawScrollOffset(0.0f), metadata(std::chrono::high_resolution_clock::now(), 0ms) {};

    MouseWheelScrollEvent(glm::vec2 _rawScrollOffset,
                          std::chrono::time_point<std::chrono::high_resolution_clock> _now,
                          std::chrono::duration<double> _duration = 0ms)
        : rawScrollOffset(_rawScrollOffset), metadata(_now, _duration) {};
};

} // namespace input
} // namespace mosaic
