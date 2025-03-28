#include "input.h"
#include "interop.hpp"
#include "openre.h"

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

    // 0x00410450
    void on_key_up(int keyCode, int a1)
    {
        for (int i = 0; i < 0x20; i++)
        {
            if (keyCode == gGameTable.input.mapping[i])
            {
                gGameTable.input.keyboard &= ~(1 << i);
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
                gGameTable.input.keyboard |= (1 << i);
            }
        }
    }

    // 0x00410400
    int input_get_some_byte()
    {
        return gGameTable.input.keyboard;
    }

    // 0x0043BAC0
    int sub_43BAC0(int a0, int a1)
    {
        using sig = int (*)(int, int);
        auto p = (sig)0x0043BAC0;
        return p(a0, a1);
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
        auto v1 = gGameTable.dword_66D394;

        joy_get_pos_ex(gGameTable.input.mapping);
        if (gGameTable.input.var_1F8 != 0)
        {
            v1 = sub_43BAC0(gGameTable.input.var_24, 0);

            gGameTable.dword_99CF64 = gGameTable.input.var_24;
            gGameTable.dword_66D394 = v1;
        }
        gGameTable.dword_99CF70 = 0;
        if (gGameTable.input.var_3B24 >= 2 && gGameTable.input.var_3D0 != 0)
        {
            auto v2 = sub_43BAC0(gGameTable.input.var_1FC, 1);
            v1 |= v2;
            gGameTable.dword_99CF70 = v2;
            gGameTable.dword_66D394 = v1;
        }

        return v1;
    }

    void input_init_hooks()
    {
        writeJmp(0x00410450, on_key_up);
        writeJmp(0x00410410, on_read_key);
        writeJmp(0x00410400, input_get_some_byte);
        writeJmp(0x0043BB00, sub_43BB00);
    }
};
