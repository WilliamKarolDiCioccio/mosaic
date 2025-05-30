#pragma once

#if defined(MOSAIC_PLATFORM_WINDOWS) || defined(MOSAIC_PLATFORM_LINUX) || \
    defined(MOSAIC_PLATFORM_MACOS) || defined(MOSAIC_PLATFORM_EMSCRIPTEN)
#include <GLFW/glfw3.h>
#endif

namespace mosaic
{
namespace input
{

/**
 * @brief The `KeyboardKey` enum class represents the keyboard keys supported by GLFW.
 *
 * The values of this enum class are the same as the GLFW key codes. This allows for easy mapping
 * between GLFW key codes and the `KeyboardKey` enum class.
 *
 * @see https://www.glfw.org/docs/latest/group__keys.html
 */
enum class KeyboardKey : uint32_t
{
    key_a = GLFW_KEY_A,
    key_b = GLFW_KEY_B,
    key_c = GLFW_KEY_C,
    key_d = GLFW_KEY_D,
    key_e = GLFW_KEY_E,
    key_f = GLFW_KEY_F,
    key_g = GLFW_KEY_G,
    key_h = GLFW_KEY_H,
    key_i = GLFW_KEY_I,
    key_j = GLFW_KEY_J,
    key_k = GLFW_KEY_K,
    key_l = GLFW_KEY_L,
    key_m = GLFW_KEY_M,
    key_n = GLFW_KEY_N,
    key_o = GLFW_KEY_O,
    key_p = GLFW_KEY_P,
    key_q = GLFW_KEY_Q,
    key_r = GLFW_KEY_R,
    key_s = GLFW_KEY_S,
    key_t = GLFW_KEY_T,
    key_u = GLFW_KEY_U,
    key_v = GLFW_KEY_V,
    key_w = GLFW_KEY_W,
    key_x = GLFW_KEY_X,
    key_y = GLFW_KEY_Y,
    key_z = GLFW_KEY_Z,
    key_0 = GLFW_KEY_0,
    key_1 = GLFW_KEY_1,
    key_2 = GLFW_KEY_2,
    key_3 = GLFW_KEY_3,
    key_4 = GLFW_KEY_4,
    key_5 = GLFW_KEY_5,
    key_6 = GLFW_KEY_6,
    key_7 = GLFW_KEY_7,
    key_8 = GLFW_KEY_8,
    key_9 = GLFW_KEY_9,
    key_space = GLFW_KEY_SPACE,
    key_enter = GLFW_KEY_ENTER,
    key_escape = GLFW_KEY_ESCAPE,
    key_left_shift = GLFW_KEY_LEFT_SHIFT,
    key_right_Shift = GLFW_KEY_RIGHT_SHIFT,
    key_left_control = GLFW_KEY_LEFT_CONTROL,
    key_right_control = GLFW_KEY_RIGHT_CONTROL,
    key_left_alt = GLFW_KEY_LEFT_ALT,
    key_right_alt = GLFW_KEY_RIGHT_ALT,
    key_tab = GLFW_KEY_TAB,
    key_backspace = GLFW_KEY_BACKSPACE,
    key_insert = GLFW_KEY_INSERT,
    key_delete = GLFW_KEY_DELETE,
    key_home = GLFW_KEY_HOME,
    key_end = GLFW_KEY_END,
    key_page_up = GLFW_KEY_PAGE_UP,
    key_page_down = GLFW_KEY_PAGE_DOWN,
    key_arrow_up = GLFW_KEY_UP,
    key_arrow_down = GLFW_KEY_DOWN,
    key_arrow_left = GLFW_KEY_LEFT,
    key_arrow_right = GLFW_KEY_RIGHT,
    key_f1 = GLFW_KEY_F1,
    key_f2 = GLFW_KEY_F2,
    key_f3 = GLFW_KEY_F3,
    key_f4 = GLFW_KEY_F4,
    key_f5 = GLFW_KEY_F5,
    key_f6 = GLFW_KEY_F6,
    key_f7 = GLFW_KEY_F7,
    key_f8 = GLFW_KEY_F8,
    key_f9 = GLFW_KEY_F9,
    key_f10 = GLFW_KEY_F10,
    key_f11 = GLFW_KEY_F11,
    key_f12 = GLFW_KEY_F12,
};

/**
 * @brief The `c_keyboardKeys` array contains all the keyboard keys supported by GLFW.
 *
 * This allows iterating over all the keyboard keys and checking their state.
 */
constexpr std::array<KeyboardKey, 349> c_keyboardKeys = {
    KeyboardKey::key_a,
    KeyboardKey::key_b,
    KeyboardKey::key_c,
    KeyboardKey::key_d,
    KeyboardKey::key_e,
    KeyboardKey::key_f,
    KeyboardKey::key_g,
    KeyboardKey::key_h,
    KeyboardKey::key_i,
    KeyboardKey::key_j,
    KeyboardKey::key_k,
    KeyboardKey::key_l,
    KeyboardKey::key_m,
    KeyboardKey::key_n,
    KeyboardKey::key_o,
    KeyboardKey::key_p,
    KeyboardKey::key_q,
    KeyboardKey::key_r,
    KeyboardKey::key_s,
    KeyboardKey::key_t,
    KeyboardKey::key_u,
    KeyboardKey::key_v,
    KeyboardKey::key_w,
    KeyboardKey::key_x,
    KeyboardKey::key_y,
    KeyboardKey::key_z,
    KeyboardKey::key_0,
    KeyboardKey::key_1,
    KeyboardKey::key_2,
    KeyboardKey::key_3,
    KeyboardKey::key_4,
    KeyboardKey::key_5,
    KeyboardKey::key_6,
    KeyboardKey::key_7,
    KeyboardKey::key_8,
    KeyboardKey::key_9,
    KeyboardKey::key_space,
    KeyboardKey::key_enter,
    KeyboardKey::key_escape,
    KeyboardKey::key_left_shift,
    KeyboardKey::key_right_Shift,
    KeyboardKey::key_left_control,
    KeyboardKey::key_right_control,
    KeyboardKey::key_left_alt,
    KeyboardKey::key_right_alt,
    KeyboardKey::key_tab,
    KeyboardKey::key_backspace,
    KeyboardKey::key_insert,
    KeyboardKey::key_delete,
    KeyboardKey::key_home,
    KeyboardKey::key_end,
    KeyboardKey::key_page_up,
    KeyboardKey::key_page_down,
    KeyboardKey::key_arrow_up,
    KeyboardKey::key_arrow_down,
    KeyboardKey::key_arrow_left,
    KeyboardKey::key_arrow_right,
    KeyboardKey::key_f1,
    KeyboardKey::key_f2,
    KeyboardKey::key_f3,
    KeyboardKey::key_f4,
    KeyboardKey::key_f5,
    KeyboardKey::key_f6,
    KeyboardKey::key_f7,
    KeyboardKey::key_f8,
    KeyboardKey::key_f9,
    KeyboardKey::key_f10,
    KeyboardKey::key_f11,
    KeyboardKey::key_f12,
};

/**
 * @brief The `MouseButton` enum class represents the mouse buttons supported by GLFW.
 *
 * The values of this enum class are the same as the GLFW mouse button codes. This allows for easy
 * mapping between GLFW mouse button codes and the `MouseButton` enum class.
 *
 * @see https://www.glfw.org/docs/latest/group__buttons.html
 */
enum class MouseButton : uint32_t
{
    button_left = GLFW_MOUSE_BUTTON_LEFT,
    button_right = GLFW_MOUSE_BUTTON_RIGHT,
    button_middle = GLFW_MOUSE_BUTTON_MIDDLE,
    button_4 = GLFW_MOUSE_BUTTON_4,
    button_5 = GLFW_MOUSE_BUTTON_5,
    button_6 = GLFW_MOUSE_BUTTON_6,
    button_7 = GLFW_MOUSE_BUTTON_7,
    button_8 = GLFW_MOUSE_BUTTON_8,
};

/**
 * @brief The `c_mouseButtons` array contains all the mouse buttons supported by GLFW.
 *
 * This allows iterating over all the mouse buttons and checking their state.
 */
constexpr std::array<MouseButton, 8> c_mouseButtons = {
    MouseButton::button_left, MouseButton::button_right, MouseButton::button_middle,
    MouseButton::button_4,    MouseButton::button_5,     MouseButton::button_6,
    MouseButton::button_7,    MouseButton::button_8,
};

} // namespace input
} // namespace mosaic
