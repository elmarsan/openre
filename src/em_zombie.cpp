#include "enemy.h"
#include "interop.hpp"
#include "openre.h"
#include "re2.h"

namespace openre::enemy
{
    // 0x004DF320
    static void root_ck(EnemyEntity* enemy, int a1, int a2, int a3)
    {
        interop::call<void, EnemyEntity*, int, int, int>(0x004DF320, enemy, a1, a2, a3);
    }

    // 0x004DF500
    static int guide_check(int16_t a0, int16_t a1)
    {
        return interop::call<int, int16_t, int16_t>(0x004DF500, a0, a1);
    }

    // 0x004517F0
    void em_zombie(EnemyEntity* enemy)
    {
        // ((EnemyFunc)0x004517F0)(enemy);

        auto pos = gGameTable.pl.m.pos;

        if (check_flag(FlagGroup::Status, FG_STATUS_PARTNER))
        {
            if ((enemy->l_spl & 0xC0000000) == 0x40000000)
            {
                pos = gGameTable.splayer_work->m.pos;
            }
            if (guide_check(pos.x, pos.z) == -1)
            {
                pos = gGameTable.pl.m.pos;
            }
        }
        root_ck(enemy, gGameTable.pl.m.pos.x, enemy->var_235, 0);
        if (check_flag(FlagGroup::Stop, FG_STOP_02))
        {
            return;
        }

        if (enemy->type & 1 || enemy->var_21A & 2)
        {
            enemy->be_flg |= 0x4000000;
        }
        if (enemy->be_flg & 0x4000000 && !(enemy->be_flg & 0x8000000))
        {
            enemy->be_flg |= 0x4000000;
            if ((enemy->type & 0xFFF) != EM_14 && (enemy->type & 0xFFF) != EM_ZOMBIE_GUY1)
            {
                // enemy->be_flg &= 0xFBFFFFFF;
            }          
        }
    }
}