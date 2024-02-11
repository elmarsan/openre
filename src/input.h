#pragma once

namespace openre::input
{
    enum InputKey
    {
        INPUT_NONE = (0 << 0),
        INPUT_LEFT = (1 << 0),
        INPUT_DOWN = (1 << 1),
        INPUT_RIGHT = (1 << 2),
        INPUT_UP = (1 << 3),
        INPUT_X = (1 << 4),
        INPUT_A = (1 << 5),
        INPUT_B = (1 << 6),
        INPUT_Y = (1 << 7),
        INPUT_SELECT = (1 << 8),
        INPUT_START = (1 << 9)
    };

    [[nodiscard]] int GetGamepadState();
    void input_init_hooks();
};