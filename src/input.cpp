#define _CRT_SECURE_NO_WARNINGS

#include "input.h"
#include "interop.hpp"
#include "marni.h"
#include "openre.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// Prevent Visual Studio from reordering joystickapi.h needs windows.h first.
#include <joystickapi.h>

#include <cstdint>

using namespace openre::interop;

namespace openre::input
{
    enum
    {
        ID_KEY_FORWARD = 0x11,
        ID_KEY_BACKWARD = 0x24,
        ID_KEY_TURN_RIGHT = 0x802,
        ID_KEY_TURN_LEFT = 0x408,
        ID_KEY_GET_READY = 0x100,
        ID_KEY_FIRE_AND_CONFIRM = 0x10C0,
        ID_KEY_RUN_AND_CANCEL = 0x2200,
        ID_KEY_MAP = 0x4000,
        // Unknown keys
        // ID_KEY_STATUS: Open inventory
        // ID_KEY_CTL_CONFIGURE: Open settings
    };

    int GetGamepadState()
    {
        int gamepadState = INPUT_NONE;
        if (gGameTable.g_key & ID_KEY_FORWARD)
        {
            gamepadState |= INPUT_UP;
        }
        if (gGameTable.g_key & ID_KEY_BACKWARD)
        {
            gamepadState |= INPUT_DOWN;
        }
        if (gGameTable.g_key & ID_KEY_TURN_RIGHT)
        {
            gamepadState |= INPUT_RIGHT;
        }
        if (gGameTable.g_key & ID_KEY_TURN_LEFT)
        {
            gamepadState |= INPUT_LEFT;
        }
        if (gGameTable.g_key & ID_KEY_GET_READY)
        {
            gamepadState |= INPUT_X;
        }
        if (gGameTable.g_key & ID_KEY_FIRE_AND_CONFIRM)
        {
            gamepadState |= INPUT_A;
        }
        if (gGameTable.g_key & ID_KEY_RUN_AND_CANCEL)
        {
            gamepadState |= INPUT_B;
        }
        if (gGameTable.g_key & ID_KEY_MAP)
        {
            gamepadState |= INPUT_START;
        }

        return gamepadState;
    }

    enum
    {
        INPUT_DEVICE_KEYBOARD,
        INPUT_DEVICE_GAMEPAD
    };

    // 0x00410450
    void on_key_up(int keyCode, int a1)
    {
        for (int i = 0; i < 0x20; i++)
        {
            if (keyCode == gGameTable.input.mapping[i])
            {
                gGameTable.input.state &= ~(1 << i);
            }
        }
    }

    // 0x00410410
    void on_read_key(int keyCode, int a1)
    {
        for (int i = 0; i < 0x20; i++)
        {
            if (keyCode == gGameTable.input.mapping[i])
            {
                gGameTable.input.state |= (1 << i);
            }
        }
    }

    // 0x00410400
    int input_get_some_byte()
    {
        return gGameTable.input.state;
    }

    static uint32_t input_keyboard_data[32] = {
        0x1000, 0x4000, 0x8000, 0x2000, 0x20, 0x44, 0x2, 0x10, 0x4, 0x1,    0x8,    0x80,   0x100,  0x800, 0,    0,
        0,      0,      0,      0,      0,    0,    0,   0,    0,   0x1000, 0x4000, 0x8000, 0x2000, 0x80,  0x80, 0x40,
    };

    static uint32_t input_gamepad_data[32] = {
        0x1000, 0x4000, 0x8000, 0x2000, 0, 0, 0, 0, 0x80, 0x44, 0x8, 0x800, 0x10, 0x100, 0x2, 0,
        0x2,    0,      0,      0,      0, 0, 0, 0, 0,    0,    0,   0,     0,    0,     0,   0,
    };

    // 0x0043BAC0
    int get_input_device_state(int rawState, int inputType)
    {
        auto inputState = 0;
        for (int i = 0; i < 32; i++)
        {
            if (rawState & (1 << i))
            {
                if (inputType == INPUT_DEVICE_KEYBOARD)
                {
                    inputState |= input_keyboard_data[i];
                }
                else if (inputType == INPUT_DEVICE_GAMEPAD)
                {
                    inputState |= input_gamepad_data[i];
                }
            }
        }
        return inputState;
    }

    // 0x004100F0
    int joy_get_pos_ex(uint8_t* a0)
    {
        using sig = int (*)(uint8_t*);
        auto p = (sig)0x004100F0;
        return p(a0);
    }

    // 0x0043BB00
    int sub_43BB00()
    {
        auto& input = gGameTable.input;
        auto v1 = gGameTable.dword_66D394;

        joy_get_pos_ex(gGameTable.input.mapping);
        if (input.keyboard.enabled)
        {
            v1 = get_input_device_state(input.keyboard.raw_state, INPUT_DEVICE_KEYBOARD);

            gGameTable.dword_99CF64 = input.keyboard.raw_state;
            gGameTable.dword_66D394 = v1;
        }
        gGameTable.dword_99CF70 = 0;
        if (input.num_devices >= 2 && input.gamepad.enabled)
        {
            auto v2 = get_input_device_state(input.gamepad.raw_state, INPUT_DEVICE_GAMEPAD);
            v1 |= v2;
            gGameTable.dword_99CF70 = v2;
            gGameTable.dword_66D394 = v1;
        }

        return v1;
    }

    static const char* gamepadInfoStr = "ジョイスティックＩＤ%d番は、%sボタンの数 %d 軸の数%d ";

    // 0x004102E0
    static void input_init(Input* input)
    {
        // First is keyboard
        input->num_devices = 1 + joyGetNumDevs();
        std::memset(&input->keyboard, 0, sizeof(InputDevice) * 32);
        // At least one gamepad connected
        if (input->num_devices > 1)
        {
            auto joycapsa = reinterpret_cast<JOYCAPSA*>(&input->gamepad.joycapsa);
            JOYINFOEX joyInfo;

            joyGetDevCapsA(0, joycapsa, sizeof(JOYCAPSA));
            std::memset(&input->gamepad, 0, 0x34);
            input->gamepad.raw_state = 0x34;
            input->gamepad.var_04 = 0xFF;
            if (joyGetPosEx(0, &joyInfo) == JOYERR_NOERROR)
            {
                char buffer[1024];
                sprintf(buffer, gamepadInfoStr, 0, joycapsa->szPname, joycapsa->wNumButtons, joycapsa->wMaxAxes);
                marni::out();
            }
            else
            {
                input->gamepad.var_1C8 = 0;
            }
        }
        else
        {
            marni::out();
        }
    }

    void input_init_hooks()
    {
        writeJmp(0x00410450, &on_key_up);
        writeJmp(0x00410410, &on_read_key);
        writeJmp(0x00410400, &input_get_some_byte);
        writeJmp(0x0043BB00, &sub_43BB00);
        hookThisCall(0x004102E0, &input_init);
    }
};
