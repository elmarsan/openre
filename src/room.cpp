#include "room.h"
#include "interop.hpp"
#include "openre.h"
#include <cstdint>

namespace openre::room
{
    int32_t get_room_id(bool include_cut)
    {
        int32_t id = gGameTable.current_room | ((gGameTable.current_stage + 1) << 8);
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