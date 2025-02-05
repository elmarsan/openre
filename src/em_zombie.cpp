#include "enemy.h"
#include "interop.hpp"
#include "item.h"
#include "openre.h"
#include "re2.h"

namespace openre::enemy
{
    // 0x004DF320
    static int root_ck(EnemyEntity* enemy, Vec32* pos, int a2, int a3)
    {
        return interop::call<int, EnemyEntity*, Vec32*, int, int>(0x004DF320, enemy, pos, a2, a3);
    }

    // 0x004DF500
    static int guide_check(int a0, int a1)
    {
        return interop::call<int, int, int>(0x004DF500, a0, a1);
    }

    // 0x0045BD20
    static void sub_45BD20(EnemyEntity* enemy)
    {
        interop::call<void, EnemyEntity*>(0x0045BD20, enemy);
    }

    // 0x0045B330
    static void sub_45B330(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        interop::call<void, EnemyEntity*, Emr*, Edd*>(0x0045B330, enemy, emr, edd);
    }

    // 0x0045BB80
    static void sub_45BB80(EnemyEntity* enemy)
    {
        interop::call<void, EnemyEntity*>(0x0045BB80, enemy);
    }

    // 0x0045B870
    static void sub_45B870(EnemyEntity* enemy)
    {
        interop::call<void, EnemyEntity*>(0x0045B870, enemy);
    }

    // 0x00459B60
    static void sub_459B60(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        interop::call<void, EnemyEntity*, Emr*, Edd*>(0x00459B60, enemy, emr, edd);
    }

    // 0x00456780
    static void sub_456780(EnemyEntity* enemy)
    {
        interop::call<void, EnemyEntity*>(0x00456780, enemy);
    }

    // 0x00456920
    static void sub_456920(EnemyEntity* enemy)
    {
        interop::call<void, EnemyEntity*>(0x00456920, enemy);
    }

    // 0x00456A00
    static void sub_456A00(EnemyEntity* enemy)
    {
        interop::call<void, EnemyEntity*>(0x00456A00, enemy);
    }

    // 0x00456B00
    static void sub_456B00(EnemyEntity* enemy)
    {
        interop::call<void, EnemyEntity*>(0x00456B00, enemy);
    }

    // 0x004B8470
    static int esp_call(int a0, int a1, Mat16 matrix, Vec16p vec)
    {
        using sig = int (*)(int, int, Mat16, Vec16p);
        auto p = (sig)0x004B8470;
        return p(a0, a1, matrix, vec);
    }

    static EnemyFunc* _moveFuncTable0 = (EnemyFunc*)0x00526E20;
    static EnemyFunc* _moveFuncTable1 = (EnemyFunc*)0x00526EA0;

    static EnemyRoutineFunc* _moveRoutineTable0 = (EnemyRoutineFunc*)0x00526E60;
    static EnemyRoutineFunc* _moveRoutineTable1 = (EnemyRoutineFunc*)0x00526EAC;

    // 0x004525B0
    static void em_zombie_move(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        switch (enemy->type & 0x3F)
        {
        case 0:
        case 2:
        case 4:
        case 6:
        case 8:
        {
            _moveFuncTable0[enemy->routine_1](enemy);
            _moveRoutineTable0[enemy->routine_1](enemy, emr, edd);
            break;
        }
        case 1:
        case 3:
        case 5:
        case 7:
        {
            _moveFuncTable1[enemy->routine_1](enemy);
            _moveRoutineTable1[enemy->routine_1](enemy, emr, edd);
            break;
        }
        }
    }

    static EnemyRoutineFunc* _dieRoutineTable0 = (EnemyRoutineFunc*)0x00527464;
    static EnemyRoutineFunc* _dieRoutineTable1 = (EnemyRoutineFunc*)0x005271BC;

    // 0x004585B0
    static void em_zombie_die(EnemyEntity* enemy, Emr* emr, Edd* edd)
    {
        interop::call<void, EnemyEntity*, Emr*, Edd*>(0x004585B0, enemy, emr, edd);
        return;

        auto value = static_cast<uint16_t>(enemy->var_21A | enemy->var_21B << 8);
        if (!(value & 0x4000))
        {
            gGameTable.word_98E9B8++;

            value |= 0x4000;
            enemy->var_21A = value & 0xff;
            enemy->var_21B = value >> 8;
        }
        enemy->var_21B &= ~2;

        if (gGameTable.pl.routine_0 == 5 && gGameTable.pl.pEnemy_ptr == enemy)
        {
            gGameTable.pl.routine_0 = 1;
            gGameTable.pl.routine_1 = 0;
            gGameTable.pl.routine_2 = 0;
            gGameTable.pl.routine_3 = 0;
            gGameTable.pl.damage_cnt &= ~0x80;
            gGameTable.pl.neck_flg &= 0xED;
            enemy->be_flg &= 0xFFFFEFFB;
            gGameTable.pl.be_flg &= 0xFFFFEFFB;
        }

        if (!enemy->routine_2)
        {
            // auto be_flg = &enemy->pSin_parts_ptr;
            // *reinterpret_cast<uint32_t*>(be_flg + 0x204) |= 0x80;
            // *reinterpret_cast<uint32_t*>(be_flg + 0x2B0) |= 0x80;
            // *reinterpret_cast<uint32_t*>(be_flg + 0x408) |= 0x80;
            // *reinterpret_cast<uint32_t*>(be_flg + 0x4B4) |= 0x80;
            // *reinterpret_cast<uint32_t*>(be_flg + 0x560) |= 0x80;
        }

        if (enemy->type & 1)
        {
            _dieRoutineTable0[enemy->routine_1](enemy, emr, edd);
        }

        value = static_cast<uint16_t>(enemy->var_21A | enemy->var_21B << 8);
        if (value & 0x10)
        {
            sub_459B60(enemy, emr, edd);
        }
        if (!(value & 2))
        {
            return _dieRoutineTable1[enemy->routine_1 + enemy->damage_no](enemy, emr, edd);
        }

        Vec16p vec16p{};
        if (gGameTable.blood_censor)
        {
            esp_call(1056576, 0, enemy->pSin_parts_ptr->workm, vec16p);
        }
        else
        {
            esp_call(8000, 0, enemy->pSin_parts_ptr->workm, vec16p);
        }

        if (rnd() & 1)
        {
            snd_se_enem(0xD, enemy);
        }
        else
        {
            snd_se_enem(0xB, enemy);
        }

        if (enemy->routine_1 == 10 && enemy->type >= 0)
        {
            sub_456780(enemy);
        }
        if (enemy->routine_1 == 11)
        {
            sub_456920(enemy);
        }
        if (enemy->routine_1 == 9 || enemy->routine_1 == 17)
        {
            if (enemy->type >= 0)
            {
                sub_456A00(enemy);
            }
        }
        if (enemy->routine_1 == 14)
        {
            sub_456B00(enemy);
        }
        enemy->routine_0 = 7;
        enemy->routine_1 = 9;
        enemy->routine_2 = 0;
        enemy->routine_3 = 0;
    }

    static EnemyRoutineFunc routines[] = {
        (EnemyRoutineFunc)0x00451A80, //  em_zombie_init
        em_zombie_move,
        (EnemyRoutineFunc)0x00455780, // em_zombie_hurt
        (EnemyRoutineFunc)0x004585B0, // em_zombie_die
        (EnemyRoutineFunc)0x004F6080,
        nullptr,
        nullptr,
        (EnemyRoutineFunc)0x0045A410,
        (EnemyRoutineFunc)0x00459DF0,
    };

    // 0x004517F0
    void em_zombie(EnemyEntity* enemy)
    {
        // ((EnemyFunc)0x004517F0)(enemy);
        // return;

        auto& pl = gGameTable.pl;
        auto* pos = &gGameTable.pl.m.pos;

        if (check_flag(FlagGroup::Status, FG_STATUS_PARTNER))
        {
            if ((enemy->l_spl & 0xC0000000) == 0x40000000)
            {
                pos = &gGameTable.splayer_work->m.pos;
            }
            if (guide_check(pos->x, pos->z) == -1)
            {
                pos = &gGameTable.pl.m.pos;
            }
        }
        root_ck(enemy, pos, enemy->var_235, 0);
        if (check_flag(FlagGroup::Stop, FG_STOP_02))
        {
            return;
        }

        if (enemy->type & 1 || enemy->var_21A & 2)
        {
            // Falling, lying on the ground or getting up from the ground?
            enemy->be_flg |= 0x4000000;
        }

        if (enemy->be_flg & 0x4000000 && !(enemy->be_flg & 0x8000000))
        {
            // enemy->be_flg |= 0x4000000;
            if ((pl.type & 0xFFF) != ITEM_TYPE_SPARK_SHOT && (pl.type & 0xFFF) != ITEM_TYPE_GATLING_GUN)
            {
                if (pl.status_flg < 0 && pl.m.pos.y - 1500 < enemy->m.pos.y)
                {
                    // enemy->be_flg &= 0xFBFFFFFF;
                    // enemy->be_flg = (enemy->be_flg | 0x4000000) & 0xFBFFFFFF;
                    enemy->be_flg &= ~0x4000000;
                }
                if ((pl.status_flg & 0x4000) && pl.m.pos.y - 1000 < enemy->m.pos.y)
                {
                    enemy->be_flg &= ~0x4000000;
                }
            }
        }
        if (enemy->var_239)
        {
            enemy->var_239--;
        }
        if (enemy->var_23E)
        {
            enemy->var_23E--;
        }
        if (enemy->damage_cnt & 0xF7)
        {
            enemy->damage_cnt--;
        }
        if (enemy->var_230)
        {
            enemy->var_230--;
            auto fgRoomEnemy = gGameTable.fg_room_enemy;
            gGameTable.fg_room_enemy = ((fgRoomEnemy << 8) | 1) << 8 | fgRoomEnemy & 0xFF;
        }
        routines[enemy->routine_0](enemy, enemy->pSub0_kan_t_ptr, enemy->pSub0_seq_t_ptr);
        // sub_45BD20(enemy);
        auto value = static_cast<uint16_t>(enemy->var_236 | (enemy->var_237 << 8));
        value++;
        enemy->var_236 = value & 0xFF;
        enemy->var_237 = (value >> 8) & 0xFF;
        enemy->pEnemy_ptr = &pl;
        enemy->neck_flg &= 0xFD;
        if (!(enemy->type & 0x4000) || enemy->var_21B & 2)
        {
            enemy->neck_flg |= 2;
        }
        rot_neck_em(enemy, enemy->cdir.y);
        sub_45B330(enemy, enemy->pSub0_kan_t_ptr, enemy->pSub0_seq_t_ptr);

        if (enemy->water && pl.life >= 0)
        {
            if (enemy->pSin_parts_ptr->workm.pos.y - 500 < enemy->water && (enemy->move_cnt & 0xF) == 1 && enemy->spd.x > 10)
            {
                sub_45BB80(enemy);
            }
            if ((*enemy->pNow_seq & 0xF8000000) == 0x8000000)
            {
                sub_45BB80(enemy);
            }
            if ((*enemy->pNow_seq & 0xF8000000) == 0x48000000)
            {
                sub_45BB80(enemy);
            }
        }
        sub_45B870(enemy);
        enemy->var_228 = enemy->routine_0;
        enemy->var_229 = enemy->routine_1;
        enemy->var_22A = enemy->routine_2;
        enemy->var_22B = enemy->routine_3;
        em_seq_sound(enemy);
        oba_ck_em(enemy);
        sca_ck_em(enemy, 1024);
    }
}