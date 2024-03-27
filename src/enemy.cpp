#include "enemy.h"
#include "interop.hpp"
#include "openre.h"
#include <array>

namespace openre::enemy
{
    struct UnknownA3
    {
        uint8_t pad_00[0xA0 - 0x00];
        uint16_t var_A0;
    };

    using EnemyFunc = void (*)(EnemyEntity*);
    using EnemyRoutineFunc = void (*)(EnemyEntity*, void*, int);

    static void em_zombie(EnemyEntity* enemy);
    static void em_20(EnemyEntity* enemy);
    static void em_21(EnemyEntity* enemy);
    static void em_22(EnemyEntity* enemy);
    static void em_23(EnemyEntity* enemy);
    static void em_24(EnemyEntity* enemy);
    static void em_25(EnemyEntity* enemy);
    static void em_26(EnemyEntity* enemy);
    static void em_27(EnemyEntity* enemy);
    static void em_28(EnemyEntity* enemy);
    static void em_29(EnemyEntity* enemy);
    static void em_2A(EnemyEntity* enemy);
    static void em_2B(EnemyEntity* enemy);
    static void em_2C(EnemyEntity* enemy);
    static void em_2D(EnemyEntity* enemy);
    static void em_2E(EnemyEntity* enemy);
    static void em_2F(EnemyEntity* enemy);
    static void em_30(EnemyEntity* enemy);
    static void em_31(EnemyEntity* enemy);
    static void em_33(EnemyEntity* enemy);
    static void em_34(EnemyEntity* enemy);
    static void em_36(EnemyEntity* enemy);
    static void em_37(EnemyEntity* enemy);
    static void em_38(EnemyEntity* enemy);
    static void em_3A(EnemyEntity* enemy);
    static void em_3B(EnemyEntity* enemy);
    static void em_3E(EnemyEntity* enemy);
    static void em_3F(EnemyEntity* enemy);

    static int16_t word_52DA48[] = { 30,   59,   87,   114,  140,  165,  189,  212,  234,  255,  275,  294,  312,  329,  345,
                                     360,  374,  387,  399,  410,  420,  429,  437,  444,  450,  455,  459,  462,  464,  465,
                                     180,  354,  522,  684,  840,  990,  1134, 1272, 1404, 1530, 1650, 1764, 1872, 1974, 2070,
                                     2160, 2244, 2322, 2394, 2460, 2520, 2574, 2622, 2664, 2700, 2730, 2754, 2772, 2784, 2790 };

    // 0x004B1DD0
    static void em_move_tbl_set()
    {
        gGameTable.enemy_init_entries[0].type = EM_NONE;
        gGameTable.enemy_init_entries[1].type = EM_NONE;

        gGameTable.enemy_init_table[EM_ZOMBIE_COP] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_BRAD] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_GUY1] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_GIRL] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_14] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_TESTSUBJECT] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_SCIENTIST] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_NAKED] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_GUY2] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_19] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_1A] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_1B] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_1C] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_1D] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_GUY3] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_RANDOM] = em_zombie;
        gGameTable.enemy_init_table[EM_ZOMBIE_DOG] = em_20;
        gGameTable.enemy_init_table[EM_CROW] = em_21;
        gGameTable.enemy_init_table[EM_LICKER_RED] = em_22;
        gGameTable.enemy_init_table[EM_ALLIGATOR] = em_23;
        gGameTable.enemy_init_table[EM_LICKER_GREY] = em_24;
        gGameTable.enemy_init_table[EM_SPIDER] = em_25;
        gGameTable.enemy_init_table[EM_SPIDER_BABY] = em_26;
        gGameTable.enemy_init_table[EM_G_EMBRYO] = em_27;
        gGameTable.enemy_init_table[EM_G_ADULT] = em_28;
        gGameTable.enemy_init_table[EM_COCKROACH] = em_29;
        gGameTable.enemy_init_table[EM_TYRANT_1] = em_2A;
        gGameTable.enemy_init_table[EM_TYRANT_2] = em_2B;
        gGameTable.enemy_init_table[EM_2C] = em_2C;
        gGameTable.enemy_init_table[EM_ZOMBIE_ARMS] = em_2D;
        gGameTable.enemy_init_table[EM_IVY] = em_2E;
        gGameTable.enemy_init_table[EM_VINES] = em_2F;
        gGameTable.enemy_init_table[EM_BIRKIN_1] = em_30;
        gGameTable.enemy_init_table[EM_BIRKIN_2] = em_31;

        gGameTable.enemy_init_table[EM_BIRKIN_4] = em_33;
        gGameTable.enemy_init_table[EM_BIRKIN_5] = em_34;

        gGameTable.enemy_init_table[EM_36] = em_36;
        gGameTable.enemy_init_table[EM_37] = em_37;
        gGameTable.enemy_init_table[EM_38] = em_38;
        gGameTable.enemy_init_table[EM_IVY_POISON] = em_2E;
        gGameTable.enemy_init_table[EM_MOTH] = em_3A;
        gGameTable.enemy_init_table[EM_MAGGOTS] = em_3B;
        gGameTable.enemy_init_table[EM_3E] = em_3E;
        gGameTable.enemy_init_table[EM_3F] = em_3F;

        gGameTable.enemy_init_table[0x35] = em_30;

        for (auto i = 0; i < 96; i++)
        {
            gGameTable.enemy_init_table[96 + i] = gGameTable.enemy_init_table[i];
        }
    }

    [[nodiscard]] bool is_enemy_dead(uint8_t globalId)
    {
        auto fgEnemy = FlagGroup::Enemy2;
        if (gGameTable.current_stage < 3 && !check_flag(FlagGroup::Status, FG_STATUS_BONUS))
        {
            fgEnemy = FlagGroup::Enemy;
        }
        return globalId != 0xFF && check_flag(fgEnemy, globalId);
    }

    [[nodiscard]] static EnemyFunc get_enemy_init_func(uint8_t type, int slot)
    {
        auto index = (slot * 96) + type;
        return reinterpret_cast<EnemyFunc>(gGameTable.enemy_init_table[index]);
    }

    [[nodiscard]] static bool is_zombie(uint8_t type)
    {
        return type >= EM_ZOMBIE_COP && type <= EM_ZOMBIE_RANDOM;
    }

    // 0x004B20B0
    void em_bin_load(uint8_t type)
    {
        auto func = get_enemy_init_func(type, 0);
        if (func == nullptr)
            return;

        // Share same init function for all zombies
        auto sharedType = is_zombie(type) ? EM_ZOMBIE_COP : type;

        auto entries = gGameTable.enemy_init_entries;
        int slot;
        for (slot = 0; slot < 2; slot++)
        {
            if (entries[slot].type == sharedType || entries[slot].enabled == 0)
                break;
        }

        entries[slot].type = sharedType;
        entries[slot].enabled = 1;
        gGameTable.enemy_init_map[type] = get_enemy_init_func(type, slot);
    }

    // 0x004E3AB0
    [[nodiscard]] uint8_t em_kind_search(uint8_t id)
    {
        return interop::call<uint8_t, uint8_t>(0x004E3AB0, id);
    }

    // 0x004B1660
    void* emd_load(int id, Entity* entity, void* buffer)
    {
        return interop::call<void*, int, Entity*, void*>(0x004B1660, id, entity, buffer);
    }

    // 0x004C1270
    void* partswork_set(Entity* entity, void* parts)
    {
        return interop::call<void*, Entity*, void*>(0x004C1270, entity, parts);
    }

    // 0x004C12B0
    void* partswork_link(Entity* entity, void* packetTop, void* kan, int mode)
    {
        return interop::call<void*, Entity*, void*, void*, int>(0x004C12B0, entity, packetTop, kan, mode);
    }

    // 0x004DFD20
    void* sa_dat_set(Entity* entity, void* arg1)
    {
        return interop::call<void*, Entity*, void*>(0x004DFD20, entity, arg1);
    }

    // 0x004C52A0
    void* mirror_model_cp(Entity* entity, void* arg1)
    {
        using sig = void* (*)(Entity*, void*);
        auto p = (sig)0x004C52A0;
        return p(entity, arg1);
    }

    // 0x00443F40
    int* mem_ck_parts_work(int workNo, int id)
    {
        return interop::call<int*, int, int>(0x00443F40, workNo, id);
    }

    // 0x004CC680
    static void oba_ck_em(EnemyEntity* enemy)
    {
        interop::call<void, EnemyEntity*>(0x004CC680, enemy);
    }

    // 0x004CC730
    static void sca_ck_em(EnemyEntity* enemy, int a1)
    {
        interop::call<void, EnemyEntity*, int>(0x004CC730, enemy, a1);
    }

    // 0x00445840
    void sub_445840(void* a0, int a1)
    {
        interop::call<void, void*, int>(0x00445840, a0, a1);
    }

    // 0x004C0E40
    static void ko_joint_trans2(Entity* entity, PartsW* part, uint16_t beFlg, Mat16* workm)
    {
        interop::call<void, Entity*, PartsW*, uint16_t, Mat16*>(0x004C0E40, entity, part, beFlg, workm);
    }

    // 0x004C5230
    static void bomb_parts_sort_gt(void* a0, void* a1, UnknownA3* a2)
    {
        auto a = a2->var_A0 & 0x7FFF;
        auto b = (a2->var_A0 >> 15) * 30;
        auto c = word_52DA48[a + b];
        sub_445840(a1, c);
    }

    // 0x004517F0
    static void em_zombie(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x004517F0)(enemy);
    }

    enum
    {
        EM_20_ROUTINE_INIT,
        EM_20_ROUTINE_NORMAL,
        EM_20_ROUTINE_HURT,
        EM_20_ROUTINE_DIE,
        EM_20_ROUTINE_4,
        EM_20_ROUTINE_5,
        EM_20_ROUTINE_6,
        EM_20_ROUTINE_DEAD,
    };

    // 0x0045F470
    static void em_20_dead(EnemyEntity* enemy, void*, int)
    {
        if (enemy->routine_1 == 0)
        {
            enemy->be_flg |= 2;
            enemy->routine_1 = 1;
        }

        auto kage = enemy->pKage_work;
        if (enemy->var_221 == 0)
        {
            enemy->var_221 = 1;
            auto colour = gGameTable.blood_censor ? 0xBF10BF : 0xBFBF10;
            if (kage != nullptr)
            {
                kage->var_1C = (kage->var_1C & 0xFF000000) | colour;
                kage->var_44 = (kage->var_44 & 0xFF000000) | colour;
            }
            enemy->timer3 = 90;
        }
        else if (enemy->var_221 != 1)
        {
            return;
        }

        if (kage != nullptr)
        {
            kage->var_04 += 8;
            kage->var_06 += 8;
        }

        enemy->timer3--;
        if (enemy->timer3 == 0)
            enemy->var_221 = 2;
    }

    static EnemyRoutineFunc em_20_routines[] = { (EnemyRoutineFunc)0x0045C140,
                                                 (EnemyRoutineFunc)0x0045C420,
                                                 (EnemyRoutineFunc)0x0045E430,
                                                 (EnemyRoutineFunc)0x0045EE20,
                                                 (EnemyRoutineFunc)0x00492990,
                                                 nullptr,
                                                 nullptr,
                                                 em_20_dead

    };

    // 0x0045C0A0
    static void em_20(EnemyEntity* enemy)
    {
        if (check_flag(FlagGroup::Stop, FG_STOP_02))
            return;

        if ((enemy->damage_cnt & 0x7F) != 0)
            enemy->damage_cnt--;

        if (enemy->var_232 != 0)
            enemy->var_232--;

        if (enemy->routine_0 < std::size(em_20_routines))
            em_20_routines[enemy->routine_0](enemy, enemy->pKan_t_ptr, enemy->pSeq_t_ptr);
        oba_ck_em(enemy);
        sca_ck_em(enemy, 1024);

        if ((enemy->pos.x == enemy->old_pos.x && enemy->pos.z == enemy->old_pos.z) || enemy->at_em_no != 0xFF)
            enemy->var_230++;
        else
            enemy->var_230 = 0;
    }

    // 0x0045FC10
    static void em_21(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x0045FC10)(enemy);
    }

    // 0x00464630
    static void em_22(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x00464630)(enemy);
    }

    // 0x00468760
    static void em_23(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x00468760)(enemy);
    }

    // 0x0046CA80
    static void em_24(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x0046CA80)(enemy);
    }

    // 0x0046F000
    static void em_25(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x0046F000)(enemy);
    }

    // 0x00473620
    static void em_26(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x00473620)(enemy);
    }

    // 0x00474210
    static void em_27(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x00474210)(enemy);
    }

    // 0x00476EF0
    static void em_28(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x00476EF0)(enemy);
    }

    // 0x0047BF60
    static void em_29(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x0047BF60)(enemy);
    }

    // 0x0047D330
    static void em_2A(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x0047D330)(enemy);
    }

    // 0x00480900
    static void em_2B(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x00480900)(enemy);
    }

    // 0x00483C90
    static void em_2C(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x00483C90)(enemy);
    }

    // 0x00483E30
    static void em_2D(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x00483E30)(enemy);
    }

    // 0x00484B90
    static void em_2E(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x00484B90)(enemy);
    }

    // 0x004889D0
    static void em_2F(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x004889D0)(enemy);
    }

    // 0x0048A320
    static void em_30(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x0048A320)(enemy);
    }

    // 0x004900E0
    static void em_31(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x004900E0)(enemy);
    }

    // 0x00496930
    static void em_33(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x00496930)(enemy);
    }

    // 0x0049B5D0
    static void em_34(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x0049B5D0)(enemy);
    }

    // 0x004A4560
    static void em_36(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x004A4560)(enemy);
    }

    // 0x004A8770
    static void em_37(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x004A8770)(enemy);
    }

    // 0x004AC8E0
    static void em_38(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x004AC8E0)(enemy);
    }

    // 0x004ACC70
    static void em_3A(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x004ACC70)(enemy);
    }

    // 0x004B0370
    static void em_3B(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x004B0370)(enemy);
    }

    // 0x004B1210
    static void em_3E(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x004B1210)(enemy);
    }

    // 0x004B13D0
    static void em_3F(EnemyEntity* enemy)
    {
        ((EnemyFunc)0x004B13D0)(enemy);
    }

    void enemy_init_hooks()
    {
        interop::writeJmp(0x004B1DD0, em_move_tbl_set);
        interop::writeJmp(0x004C5230, bomb_parts_sort_gt);
    }
}
