#include "enemy.h"
#include "entity.h"
#include "interop.hpp"
#include "openre.h"

namespace openre::enemy
{
    void em_spider_01(EnemyEntity* enemy, Emr* emr, Edd* edd);

    // 0x004EA1C0
    static int water_ck(int a0, int a1)
    {
        return interop::call<int, int, int>(0x004EA1C0, a0, a1);
    }

    // 0x004727E0
    static void sub_4727E0(EnemyEntity* enemy)
    {
        interop::call<void, EnemyEntity*>(0x004727E0, enemy);
    }

    // 0x00473530
    static void sub_473530(EnemyEntity* enemy)
    {
        interop::call<void, EnemyEntity*>(0x00473530, enemy);
    }

    // 0x004728B0
    static void sub_4728B0(EnemyEntity* enemy)
    {
        interop::call<void, EnemyEntity*>(0x004728B0, enemy);
    }

    // 0x004733D0
    static void sub_4733D0(EnemyEntity* enemy)
    {
        interop::call<void, EnemyEntity*>(0x004733D0, enemy);
    }

    // 0x004735A0
    static void sub_4735A0(EnemyEntity* enemy)
    {
        interop::call<void, EnemyEntity*>(0x004735A0, enemy);
    }

    // 0x00472DF0
    static void sub_472DF0(EnemyEntity* enemy, int a1, int a2)
    {
        interop::call<void, EnemyEntity*, int, int>(0x00472DF0, enemy, a1, a2);
    }

    // 0x00471190
    static void sub_471190(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        if (!enemy->routine_2)
        {
            enemy->routine_2 = 1;
            enemy->move_no = 1;
            enemy->move_cnt = 0;
            enemy->hokan_flg = 3;
            enemy->mplay_flg = 0;
            if (enemy->var_23C != 1)
            {
                sub_472DF0(enemy, 0, 0);
            }
            enemy->routine_2 += joint_move(enemy, emr, edd, 1024);
            return;
        }
        if (enemy->routine_2 == 1)
        {
            enemy->routine_2 += joint_move(enemy, emr, edd, 1024);
            return;
        }
        if (enemy->routine_2 == 2)
        {
            if (enemy->routine_1 == 1 || !(enemy->damage_no / 3))
            {
                enemy->routine_0 = 1;
                enemy->routine_1 = 9;
                enemy->routine_2 = 0;
                enemy->routine_3 = 0;
            }
            else
            {
                enemy->routine_0 = 1;
                enemy->routine_1 = gGameTable.byte_5286F4[rnd() & 7];
                enemy->routine_2 = 0;
                enemy->routine_3 = 0;
            }
        }
    }

    // 0x00471240
    static void sub_471240(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        if (!enemy->var_236)
        {
            sub_471190(enemy, emr, edd);
            return;
        }
        enemy->routine_0 = 1;
        enemy->routine_1 = enemy->var_233;
        enemy->routine_2 = enemy->var_234;
        enemy->routine_3 = enemy->var_235;
        if (!enemy->var_23C != 1)
        {
            sub_472DF0(enemy, 0, 0);
        }
        em_spider_01(enemy, emr, edd);
    }

    // 0x004712C0
    static void sub_4712C0(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        if (enemy->var_23C != 1)
        {
            sub_472DF0(enemy, 1, 0);
        }
        if (enemy->var_236)
        {
            enemy->routine_0 = 1;
            enemy->routine_1 = enemy->var_233;
            enemy->routine_2 = enemy->var_234;
            enemy->routine_3 = enemy->var_235;
            em_spider_01(enemy, emr, edd);
            return;
        }
        else
        {
            enemy->var_231 = 1;
            enemy->routine_0 = 1;
            enemy->routine_1 = 4;
            enemy->routine_2 = 0;
            enemy->routine_3 = 0;
        }
    }

    static EnemyRoutineFunc _hurtFuncs_471180[] = { sub_471190, sub_471240, sub_4712C0, sub_4712C0 };

    // 0x00471170
    // Hurt routine
    static void sub_471170(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        _hurtFuncs_471180[enemy->var_222](enemy, emr, edd);
    }

    // 0x004731A0
    static void sub_4731A0(EnemyEntity* enemy)
    {
        interop::call<void, EnemyEntity*>(0x004731A0, enemy);
    }

    // 0x00471840
    // Hurt routine
    static void sub_471840(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        if (!enemy->routine_2)
        {
            auto a1 = gGameTable.byte_528724[rnd() & 3];
            sub_472DF0(enemy, a1, 3);
            auto v4 = rnd() & 7;
            if (!((1 << v4) & enemy->var_220))
            {
                sub_472DF0(enemy, 2 * v4 + 3, 3);
            }
            if (enemy->d_life_u < 0 || enemy->d_life_c < 0 || enemy->d_life_d < 0)
            {
                sub_4731A0(enemy);
                enemy->d_life_u = 6;
                enemy->d_life_c = 6;
                enemy->d_life_d = 6;
            }
        }
        sub_471170(enemy, emr, edd);
    }

    // 0x004732A0
    static void sub_4732A0(EnemyEntity* enemy, int a1, uint8_t a2)
    {
        interop::call<void, EnemyEntity*, int, uint8_t>(0x004732A0, enemy, a1, a2);
    }

    // 0x00471360
    static void sub_471360(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        if (!enemy->routine_2)
        {
            enemy->routine_2 = 1;
            enemy->move_no = 1;
            enemy->move_cnt = 0;
            enemy->hokan_flg = 3;
            enemy->mplay_flg = 0;
            sub_4731A0(enemy);
            enemy->routine_2 += joint_move(enemy, emr, edd, 1024);
            return;
        }
        if (enemy->routine_2 == 1)
        {
            enemy->routine_2 += joint_move(enemy, emr, edd, 1024);
            return;
        }
        if (enemy->routine_2 == 2)
        {
            enemy->routine_0 = 1;
            enemy->routine_1 = 9;
            enemy->routine_2 = 0;
            enemy->routine_3 = 0;
        }
    }

    static EnemyRoutineFunc* _hurtFuncs47167C = (EnemyRoutineFunc*)0x00528718;

    // 0x00471610
    static void sub_471610(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        if (enemy->var_236)
        {
            sub_4731A0(enemy);
            enemy->routine_0 = 1;
            enemy->routine_1 = enemy->var_233;
            enemy->routine_2 = enemy->var_234;
            enemy->routine_3 = enemy->var_235;
            em_spider_01(enemy, emr, edd);
            return;
        }
        else if (enemy->type & 1)
        {
            sub_4731A0(enemy);
            sub_4732A0(enemy, 8196, (rnd() & 1) + 1);
            enemy->var_231 = 1;
            enemy->routine_0 = 1;
            enemy->routine_1 = 4;
            enemy->routine_2 = 0;
            enemy->routine_3 = 0;
        }
        else
        {
            _hurtFuncs47167C[enemy->routine_2](enemy, emr, edd);
        }
    }

    static EnemyRoutineFunc _hurtFuncs_471340[] = { sub_471360, sub_471360, sub_471610, sub_471610 };

    // 0x00471340
    // Hurt routine
    static void sub_471340(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        _hurtFuncs_471340[enemy->var_222](enemy, emr, edd);
    }

    // 0x004718E0
    // Hurt routine
    static void sub_4718E0(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        interop::call<void, EnemyEntity*, Emr*, Edd*>(0x004718E0, enemy, emr, edd);
    }

    // 0x00471980
    static void em_spider_hurt_09(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        for (int i = 0; i < 5; i++)
        {
            auto partsIndex = (rnd() & 0xF) + 3;
            auto be_flg = enemy->pSin_parts_ptr[partsIndex].Be_flg;
            if (be_flg & 1 && !(be_flg & 0x4A))
            {
                sub_472DF0(enemy, partsIndex, 5);
            }
        }

        enemy->routine_1 = 2;
        sub_471170(enemy, emr, edd);
    }

    // 0x004719F0
    // Hurt routine
    static void sub_4719F0(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        interop::call<void, EnemyEntity*, Emr*, Edd*>(0x004719F0, enemy, emr, edd);
    }

    // 0x00471AE0
    // Hurt routine
    static void sub_471AE0(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        interop::call<void, EnemyEntity*, Emr*, Edd*>(0x00471AE0, enemy, emr, edd);
    }

    // 0x00471B10
    static void em_spider_hurt_15(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        if (!enemy->var_23D)
        {
            for (int i = 0; i < 5; i++)
            {
                auto partsIndex = (rnd() & 0xF) + 3;
                auto be_flg = enemy->pSin_parts_ptr[partsIndex].Be_flg;
                if (be_flg & 1 && !(be_flg & 0x4A))
                {
                    sub_472DF0(enemy, partsIndex, 5);
                }
            }
            enemy->var_23D = 30;
        }

        enemy->routine_0 = 1;
        enemy->routine_1 = enemy->var_233;
        enemy->routine_2 = enemy->var_234;
        enemy->routine_3 = enemy->var_235;
        em_spider_01(enemy, emr, edd);
    }

    static EnemyRoutineFunc _hurtRoutines[] = {
        sub_471170,        // 0
        sub_471170,        // 1
        sub_471170,        // 2
        sub_471840,        // 3
        sub_471340,        // 4
        sub_471340,        // 5
        sub_471340,        // 6
        sub_471340,        // 7
        sub_4718E0,        // 8
        em_spider_hurt_09, // 9
        sub_4719F0,        // 10
        sub_471170,        // 11
        sub_471170,        // 12
        sub_471AE0,        // 13
        sub_471840,        // 14
        em_spider_hurt_15, // 15
        sub_471340,        // 16
        sub_471840,        // 17
        sub_471170,        // 18
    };

    static EnemyRoutineFunc* _01Routines = (EnemyRoutineFunc*)0x005285B8;

    // 0x00471110
    static void em_spider_hurt(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        if (!enemy->routine_2)
        {
            enemy->var_23C = enemy->routine_1;
        }
        if (!enemy->water)
        {
            enemy->water = water_ck(enemy->m.pos.x, enemy->m.pos.z);
        }
        return _hurtRoutines[enemy->routine_1](enemy, emr, edd);
    }

    // 0x00472730
    static void em_spider_die(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        if (enemy->routine_1)
        {
            // End die animation
            if (!enemy->timer2--)
            {
                enemy->move_no = 13;
                enemy->move_cnt = 0;
                enemy->hokan_flg = 7;
                enemy->mplay_flg = 0;

                enemy->routine_1 = 0;
            }
        }
        else if (joint_move(enemy, emr, edd, 512))
        {
            enemy->routine_1 = 1;
            enemy->timer2 = (rnd() & 0x1F) + 30;
        }
        if (!enemy->routine_2)
        {
            enemy->routine_2 = 1;
            enemy->var_220 = 2;
            enemy->timer1 = 0;
        }
        if (enemy->var_23A >= 0)
        {
            sub_4728B0(enemy);
        }

        enemy->be_flg |= 0x4000000;

        if (enemy->m.pos.y < gGameTable.pl.m.pos.y)
        {
            enemy->be_flg |= 0x8000000;
        }
    }

    // 0x0046F370
    static void em_spider_01(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        sub_4733D0(enemy);
        _01Routines[enemy->var_222](enemy, emr, edd);
        sub_4735A0(enemy);
        if (enemy->var_218)
        {
            enemy->var_218--;
        }
        if (enemy->var_232)
        {
            enemy->var_232--;
        }

        auto value = static_cast<uint16_t>(enemy->var_21E | (enemy->var_21F << 8));
        if (value)
        {
            value++;
            enemy->var_21E = value & 0xFF;
            enemy->var_21F = (value >> 8) & 0xFF;
        }
        else
        {
            enemy->var_21E = 0;
            enemy->var_21F = 0;
            enemy->var_233 = enemy->routine_1;
            enemy->var_234 = enemy->routine_2;
            enemy->var_235 = enemy->routine_3;
        }
    }

    static EnemyRoutineFunc _routines[] = {
        (EnemyRoutineFunc)0x0046F0C0, &em_spider_01, &em_spider_hurt, (EnemyRoutineFunc)0x00471BB0,
        (EnemyRoutineFunc)0x00492990, nullptr,       nullptr,         &em_spider_die,
    };

    // 0x0046F000
    void em_spider(EnemyEntity* enemy)
    {
        if (check_flag(FlagGroup::Stop, FG_STOP_02))
        {
            if (enemy->var_23F)
            {
                const auto v4 = water_ck(enemy->m.pos.x, enemy->m.pos.z);
                const auto res = v4 + 70;
                if (enemy->ground <= res)
                {
                    enemy->m.pos.y = enemy->ground;
                }
                else
                {
                    enemy->m.pos.y = res;
                }
            }

            return;
        }

        if (enemy->damage_cnt & 0x7F)
        {
            enemy->damage_cnt--;
        }
        if (enemy->var_23D)
        {
            enemy->var_23D--;
        }
        _routines[enemy->routine_0](enemy, enemy->pKan_t_ptr, enemy->pSeq_t_ptr);

        auto mul = static_cast<int64_t>(0x6E5D4C3B) * enemy->m.pos.y;
        auto hi32 = static_cast<int32_t>(mul >> 32);
        auto diff = hi32 - enemy->m.pos.y;
        auto floorDiv = diff >> 10;
        auto correction = (diff >> 31) & 1;
        enemy->nFloor = floorDiv + correction;

        oba_ck_em(enemy);
        sca_ck_em(enemy, 1536);
        sub_4727E0(enemy);
        sub_473530(enemy);
    }
}