#include "room.h"
#include "interop.hpp"
#include "openre.h"

namespace openre::room
{
    // 0x004DE7B0
    static void set_room()
    {
        auto v0 = gGameTable.ctcb;
        auto v1 = gGameTable.ctcb->var_13;

        while (2)
        {
            switch (v1)
            {
            case 0:
                std::strcpy(gGameTable.room_path, "Pl0\\Rdt\\room1000.rdt");
                switch (gGameTable.graphicsPtr)
                {
                case 0: goto LABEL_6;
                case 1:

                case 2:
                LABEL_6:

                default:
                LABEL_8:
                    ///////////////////////
                    ///////////////////////
                    ///////////////////////

                    // qmemcpy(stage_font_name, v2, 0x16u);
                    // goto LABEL_8;
                }

            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            }
        }
    }

    // 0x004EC7D0
    static char snd_room_load()
    {
        using sig = char (*)();
        auto p = (sig)0x004EC7D0;
        return p();
    }

    // 0x005033C0
    static int get_room_id(bool include_cut)
    {
        auto id = gGameTable.current_room | ((gGameTable.current_stage + 1) << 8);
        if (include_cut)
        {
            return (gGameTable.current_cut << 16) | id;
        }
        return id;
    }

    void init_room_hooks()
    {
        interop::writeJmp(0x005033C0, &get_room_id);
    }
};