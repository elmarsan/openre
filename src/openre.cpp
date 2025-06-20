#include "openre.h"
#include "audio.h"
#include "camera.h"
#include "door.h"
#include "enemy.h"
#include "entity.h"
#include "error.h"
#include "file.h"
#include "hud.h"
#include "input.h"
#include "interop.hpp"
#include "item.h"
#include "itembox.h"
#include "marni.h"
#include "math.h"
#include "player.h"
#include "rdt.h"
#include "re2.h"
#include "scd.h"
#include "sce.h"
#include "scheduler.h"
#include "tim.h"
#include "title.h"

#include <cstring>
#include <windows.h>

using namespace openre;
using namespace openre::audio;
using namespace openre::door;
using namespace openre::enemy;
using namespace openre::file;
using namespace openre::hud;
using namespace openre::math;
using namespace openre::player;
using namespace openre::rdt;
using namespace openre::scd;
using namespace openre::sce;
using namespace openre::input;
using namespace openre::camera;
using namespace openre::title;
using namespace openre::itembox;
using namespace openre::error;

namespace openre
{
    bool gClassicRebirthEnabled;

    GameTable& gGameTable = *((GameTable*)0x00000000);
    uint32_t& gGameFlags = *((uint32_t*)0x989ED0);
    uint16_t& gCurrentStage = *((uint16_t*)0x98EB14);
    uint16_t& gCurrentRoom = *((uint16_t*)0x98EB16);
    uint16_t& gCurrentCut = *((uint16_t*)0x98EB18);
    uint16_t& gLastCut = *((uint16_t*)0x98EB1A);
    uint32_t& gErrorCode = *((uint32_t*)0x680580);
    PlayerEntity& gPlayerEntity = *((PlayerEntity*)0x00989EF0);
    uint16_t& gPoisonStatus = *((uint16_t*)0x0098A108);
    uint8_t& gPoisonTimer = *((uint8_t*)0x0098A10A);
    uint32_t& _memTop = *((uint32_t*)0x988624);

    static uint8_t* _ospBuffer = (uint8_t*)0x698840;
    static uint8_t& _ospMaskFlag = *((uint8_t*)0x6998C0);
    static PlayerEntity*& _em = *((PlayerEntity**)0x689C60);
    static uint8_t& byte_989E7E = *((uint8_t*)0x989E7E);

    static const char* windowTitle = "BIOHAZARD(R) 2 PC";

    // 0x00509C90
    static uint8_t get_player_num()
    {
        return check_flag(FlagGroup::Status, FG_STATUS_PLAYER) ? 1 : 0;
    }

    // 0x0043DF40
    static void sub_43DF40()
    {
        using sig = void (*)();
        auto p = (sig)0x0043DF40;
        p();
    }

    // 0x004CA2F9
    void mess_print(int x, int y, const uint8_t* str, short a4)
    {
        using sig = void (*)(int, int, const uint8_t*, short);
        auto p = (sig)0x004CA2F9;
        p(x, y, str, a4);
    }

    // 0x004427E0
    void update_timer()
    {
        auto time = timeGetTime();
        gGameTable.timer_current = time;
        gGameTable.timer_last = time;
        gGameTable.timer_10 = time * 10;
    }

    // 0x004FAF80
    uint32_t check_room_no(uint32_t stage, uint32_t room)
    {
        return interop::call<uint32_t, uint32_t, uint32_t>(0x004FAF80, stage, room);
    }

    // 0x00509CE0
    bool cutscene_active()
    {
        return check_flag(FlagGroup::Status, FG_STATUS_CUTSCENE);
    }

    // 0x004DD360
    static void read_osp()
    {
        _ospMaskFlag = 1;
        auto eax = (gCurrentStage * 32) + gCurrentRoom;
        auto edx = (eax * 33) * 128;
        auto bytesRead = read_partial_file_into_buffer("common\\bin\\osp.bin", _ospBuffer, edx, 4224, 4);
        if (bytesRead == 0)
        {
            gErrorCode = bytesRead;
            _ospMaskFlag = 0;
        }
    }

    // 0x004B2A90
    uint8_t rnd()
    {
        auto hi = (uint16_t)gGameTable.dword_988610 >> 7;
        auto lo = (uint16_t)(258 * gGameTable.dword_988610) >> 8;
        gGameTable.dword_988610 = lo | (hi << 8);
        return lo;
    }

    // 0x004DF4D0
    uint8_t rnd_area()
    {
        auto blk = rdt_get_offset<uint16_t>(RdtOffsetKind::BLK);
        auto v = *blk;
        if (v == 0)
            return 0xFF;

        return rnd() % v;
    }

    // 0x00502DB0
    void set_view(const Vec32p& pVp, const Vec32p& pVr)
    {
        using sig = void (*)(const Vec32p&, const Vec32p&);
        auto p = (sig)0x00502DB0;
        p(pVp, pVr);
    }

    // 0x004C4690
    void bg_set_mode(int mode, int rgb)
    {
        using sig = void (*)(int, int);
        auto p = (sig)0x004C4690;
        p(mode, rgb);
    }

    // 0x00451570
    void set_geom_screen(int prj)
    {
        gGameTable.global_prj = prj;
    }

    bool check_flag(FlagGroup group, uint32_t index)
    {
        auto addr = gGameTable.flag_groups[static_cast<uint32_t>(group)];
        return bitarray_get(addr, index) != 0;
    }

    bool check_flags(FlagGroup group, std::vector<uint32_t> indexes)
    {
        for (auto index : indexes)
        {
            if (!check_flag(group, index))
            {
                return false;
            }
        }
        return true;
    }

    void set_flag(FlagGroup group, uint32_t index, bool value)
    {
        auto addr = gGameTable.flag_groups[static_cast<uint32_t>(group)];
        bitarray_set(addr, index, value);
    }

    static uint16_t st0_xa_leon[96]
        = { 8, 73, 6,  8, 50, 23, 2, 59, 20, 5, 40, 30, 6, 59, 18, 10, 30, 8,  0,  0,  80, 3,  0, 47,
            1, 71, 9,  3, 47, 32, 9, 22, 17, 4, 46, 32, 9, 39, 16, 10, 16, 14, 9,  55, 16, 5,  0, 40,
            7, 54, 26, 5, 70, 9,  2, 0,  59, 8, 0,  25, 9, 0,  22, 6,  0,  30, 10, 0,  16, 7,  0, 27,
            4, 0,  46, 1, 0,  71, 9, 71, 9,  6, 30, 29, 8, 25, 25, 7,  27, 27, 0,  0,  15, 85, 6, 13 };

    static uint16_t st1_xa_claire[160]
        = { 27, 27, 0,  0,  15, 85, 6,  13, 0,  23, 13, 65, 17, 5,  62, 29, 3,  69, 21, 15, 70, 8,  2,  0,  69, 5,  0,
            62, 13, 82, 9,  6,  59, 32, 14, 0,  16, 10, 0,  32, 15, 34, 9,  15, 78, 7,  14, 75, 12, 10, 62, 29, 7,  52,
            37, 7,  0,  52, 12, 76, 14, 13, 23, 21, 9,  80, 10, 1,  71, 18, 9,  0,  40, 8,  46, 41, 4,  0,  63, 3,  0,
            69, 0,  0,  91, 15, 24, 10, 10, 32, 30, 14, 62, 13, 14, 48, 14, 15, 0,  12, 13, 44, 21, 14, 16, 16, 9,  40,
            40, 12, 0,  26, 15, 43, 9,  6,  0,  59, 12, 26, 25, 2,  69, 22, 11, 0,  30, 15, 52, 9,  15, 12, 12, 14, 32,
            16, 4,  63, 27, 8,  0,  46, 1,  0,  71, 15, 61, 9,  11, 30, 29, 12, 51, 25, 11, 59, 27, 0,  0,  0 };

    // 0x00500E00
    static void stage_0()
    {
        auto isClaire = check_flag(FlagGroup::Status, FG_STATUS_PLAYER);
        gGameTable.dword_98883C = isClaire ? &st1_xa_claire[0] : &st0_xa_leon[0];
        task_exit();
    }

    // 0x00500E20
    static void stage_1()
    {
        interop::call(0x00500E20);
    }

    // 0x00500E40
    static void stage_2()
    {
        interop::call(0x00500E40);
    }

    // 0x00500E60
    static void stage_3()
    {
        interop::call(0x00500E60);
    }

    // 0x00500E80
    static void stage_4()
    {
        interop::call(0x00500E80);
    }

    // 0x00500EA0
    static void stage_5()
    {
        interop::call(0x00500EA0);
    }

    // 0x00500EC0
    static void stage_6()
    {
        interop::call(0x00500EC0);
    }

    // 0x004DEF00
    void set_stage()
    {
        gGameTable.dword_988620 = (uint32_t)&gGameTable.work_buffer;

        switch (gGameTable.current_stage)
        {
        case 0: task_execute(2, stage_0); break;
        case 1: task_execute(2, stage_1); break;
        case 2: task_execute(2, stage_2); break;
        case 3: task_execute(2, stage_3); break;
        case 4: task_execute(2, stage_4); break;
        case 5: task_execute(2, stage_5); break;
        case 6: task_execute(2, stage_6); break;
        }

        task_sleep(1);
    }

    enum
    {
        INITIAL_INVENTORY_LEON_OFFSET = 0,
        INITIAL_INVENTORY_CLAIRE_OFFSET = 11,
        INITIAL_INVENTORY_HUNK_OFFSET = 22,
        INITIAL_INVENTORY_TOFU_OFFSET = 33,
        INITIAL_INVENTORY_LEON_EX_BATTLE_OFFSET = 44,
        INITIAL_INVENTORY_CLAIRE_EX_BATTLE_OFFSET = 55,
        INITIAL_INVENTORY_ADA_EX_BATTLE_OFFSET = 66,
        INITIAL_INVENTORY_CHRIS_EX_BATTLE_OFFSET = 77,
    };

    static const InventoryDef _initialInventory[89] = {
        // Leon
        { ITEM_TYPE_HANDGUN_LEON, 18, 0 },
        { ITEM_TYPE_KNIFE, 1, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_LIGHTER, 1, 0 },
        // Claire
        { ITEM_TYPE_HANDGUN_CLAIRE, 13, 0 },
        { ITEM_TYPE_KNIFE, 1, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_LOCKPICK, 1, 0 },
        // Hunk
        { ITEM_TYPE_HANDGUN_LEON, 18, 0 },
        { ITEM_TYPE_SHOTGUN, 5, 0 },
        { ITEM_TYPE_MAGNUM, 8, 0 },
        { ITEM_TYPE_AMMO_HANDGUN, 150, 0 },
        { ITEM_TYPE_AMMO_SHOTGUN, 15, 0 },
        { ITEM_TYPE_AMMO_MAGNUM, 8, 0 },
        { ITEM_TYPE_HERB_GB, 1, 0 },
        { ITEM_TYPE_HERB_GB, 1, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_GVIRUS, 1, 0 },
        // Tofu
        { ITEM_TYPE_KNIFE, 0, 0 },
        { ITEM_TYPE_HERB_G, 1, 0 },
        { ITEM_TYPE_HERB_G, 1, 0 },
        { ITEM_TYPE_HERB_B, 1, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_GVIRUS, 1, 0 },
        // Extreme battle Leon
        { ITEM_TYPE_HANDGUN_LEON, 18, 0 },
        { ITEM_TYPE_SHOTGUN, 5, 0 },
        { ITEM_TYPE_MAGNUM, 8, 0 },
        { ITEM_TYPE_INK_RIBBON, 5, 0 },
        { ITEM_TYPE_FIRST_AID_SPRAY, 1, 0 },
        { ITEM_TYPE_HERB_B, 1, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_LIGHTER, 1, 0 },
        // Extreme battle Claire
        { ITEM_TYPE_GRENADE_LAUNCHER_EXPLOSIVE, 6, 0 },
        { ITEM_TYPE_AMMO_FLAME_ROUNDS, 6, 0 },
        { ITEM_TYPE_AMMO_ACID_ROUNDS, 6, 0 },
        { ITEM_TYPE_INK_RIBBON, 5, 0 },
        { ITEM_TYPE_FIRST_AID_SPRAY, 1, 0 },
        { ITEM_TYPE_HERB_B, 1, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_LOCKPICK, 1, 0 },
        // Extreme battle Ada
        { ITEM_TYPE_SUB_MACHINE_GUN, 100, 1 },
        { ITEM_TYPE_SUB_MACHINE_GUN, 100, 2 },
        { ITEM_TYPE_HANDGUN_COLT_SAA, 6, 0 },
        { ITEM_TYPE_INK_RIBBON, 5, 0 },
        { ITEM_TYPE_BOWGUN, 18, 0 },
        { ITEM_TYPE_HERB_GRB, 1, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_PHOTO_ADA, 1, 0 },
        // Extreme battle Chris
        { ITEM_TYPE_ROCKET_LAUNCHER, 5, 1 },
        { ITEM_TYPE_ROCKET_LAUNCHER, 5, 2 },
        { ITEM_TYPE_CUSTOM_SHOTGUN, 7, 0 },
        { ITEM_TYPE_INK_RIBBON, 5, 0 },
        { ITEM_TYPE_BERETTA, 1, 0 },
        { ITEM_TYPE_FIRST_AID_SPRAY, 1, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_NONE, 0, 0 },
        { ITEM_TYPE_LIGHTER, 1, 0 },
    };

    // 0x00500EE0
    void stage_init_item()
    {
        uint32_t initialInventoryOffset = INITIAL_INVENTORY_LEON_OFFSET;
        bool extremeBattleMode = check_flag(FlagGroup::System, FG_SYSTEM_EX_BATTLE);
        bool isLeon = check_flag(FlagGroup::Status, FG_STATUS_PLAYER) == 0;
        bool isClaire = !isLeon;

        auto& inventory = gGameTable.inventory;
        auto& itembox = gGameTable.itembox;

        if (extremeBattleMode)
        {
            switch (gGameTable.word_98EB20)
            {
            case 0: initialInventoryOffset = INITIAL_INVENTORY_LEON_EX_BATTLE_OFFSET; break;
            case 1: initialInventoryOffset = INITIAL_INVENTORY_CLAIRE_EX_BATTLE_OFFSET; break;
            case 2: initialInventoryOffset = INITIAL_INVENTORY_ADA_EX_BATTLE_OFFSET; break;
            case 3: initialInventoryOffset = INITIAL_INVENTORY_CHRIS_EX_BATTLE_OFFSET; break;
            }
        }
        // Normal, arrange and 4th initial inventory offsets
        else
        {
            if (isClaire)
            {
                initialInventoryOffset = INITIAL_INVENTORY_CLAIRE_OFFSET;
            }
            else if (gGameTable.pl.id == PLD_HUNK)
            {
                initialInventoryOffset = INITIAL_INVENTORY_HUNK_OFFSET;
            }
            else if (gGameTable.pl.id == PLD_TOFU)
            {
                initialInventoryOffset = INITIAL_INVENTORY_TOFU_OFFSET;
            }
        }

        auto* itemDef = &_initialInventory[(FULL_INVENTORY_SIZE - 1) + initialInventoryOffset];
        for (uint32_t i = FULL_INVENTORY_SIZE; i > 0; i--)
        {
            set_inventory_item(i - 1, itemDef->Type, itemDef->Quantity, itemDef->Part);
            itemDef--;
        }

        gGameTable.byte_691F6A = _initialInventory[initialInventoryOffset].Type;
        gGameTable.byte_691F68 = 0;
        if (!extremeBattleMode && gGameTable.cheat0 <= 9 && gGameTable.super_hard_switch)
        {
            if (isLeon)
            {
                inventory[0].Quantity = INVENTORY_INFINITE_QUANTITY;
                inventory[0].Part = 0;
                gGameTable.byte_691F68 = 0;

                switch (gGameTable.cheat0)
                {
                case 0:
                {
                    inventory[0].Type = ITEM_TYPE_HANDGUN_LEON;
                    gGameTable.byte_691F6A = ITEM_TYPE_HANDGUN_LEON;
                    break;
                }
                case 1:
                {
                    inventory[0].Type = ITEM_TYPE_CUSTOM_HANDGUN;
                    gGameTable.byte_691F6A = ITEM_TYPE_CUSTOM_HANDGUN;
                    break;
                }
                case 2:
                {
                    inventory[0].Type = ITEM_TYPE_MAGNUM;
                    gGameTable.byte_691F6A = ITEM_TYPE_MAGNUM;
                    break;
                }
                case 3:
                {
                    inventory[0].Type = ITEM_TYPE_CUSTOM_MAGNUM;
                    gGameTable.byte_691F6A = ITEM_TYPE_CUSTOM_MAGNUM;
                    break;
                }
                case 4:
                {
                    inventory[0].Type = ITEM_TYPE_SHOTGUN;
                    gGameTable.byte_691F6A = ITEM_TYPE_SHOTGUN;
                    break;
                }
                case 5:
                {
                    inventory[0].Type = ITEM_TYPE_CUSTOM_SHOTGUN;
                    gGameTable.byte_691F6A = ITEM_TYPE_CUSTOM_SHOTGUN;
                    break;
                }
                case 6:
                {
                    set_inventory_item(0, ITEM_TYPE_FLAMETHROWER, INVENTORY_INFINITE_QUANTITY, 1);
                    set_inventory_item(1, ITEM_TYPE_FLAMETHROWER, INVENTORY_INFINITE_QUANTITY, 2);
                    gGameTable.byte_691F6A = ITEM_TYPE_FLAMETHROWER;
                    break;
                }
                case 7:
                {
                    set_inventory_item(0, ITEM_TYPE_SUB_MACHINE_GUN, INVENTORY_INFINITE_QUANTITY, 1);
                    set_inventory_item(1, ITEM_TYPE_SUB_MACHINE_GUN, INVENTORY_INFINITE_QUANTITY, 2);
                    gGameTable.byte_691F6A = ITEM_TYPE_SUB_MACHINE_GUN;
                    break;
                }
                case 8:
                {
                    set_inventory_item(0, ITEM_TYPE_ROCKET_LAUNCHER, INVENTORY_INFINITE_QUANTITY, 1);
                    set_inventory_item(1, ITEM_TYPE_ROCKET_LAUNCHER, INVENTORY_INFINITE_QUANTITY, 2);
                    gGameTable.byte_691F6A = ITEM_TYPE_ROCKET_LAUNCHER;
                    break;
                }
                case 9:
                {
                    set_inventory_item(0, ITEM_TYPE_GATLING_GUN, INVENTORY_INFINITE_QUANTITY, 1);
                    set_inventory_item(1, ITEM_TYPE_GATLING_GUN, INVENTORY_INFINITE_QUANTITY, 2);
                    gGameTable.byte_691F6A = ITEM_TYPE_GATLING_GUN;
                    break;
                }
                }
            }
            // Claire
            else
            {
                inventory[0].Quantity = -1;
                inventory[0].Part = 0;
                gGameTable.byte_691F68 = 0;

                switch (gGameTable.cheat0)
                {
                case 0:
                {
                    inventory[0].Type = ITEM_TYPE_HANDGUN_CLAIRE;
                    gGameTable.byte_691F6A = ITEM_TYPE_HANDGUN_CLAIRE;
                    break;
                }
                case 1:
                {
                    inventory[0].Type = ITEM_TYPE_GRENADE_LAUNCHER_EXPLOSIVE;
                    gGameTable.byte_691F6A = ITEM_TYPE_GRENADE_LAUNCHER_EXPLOSIVE;
                    break;
                }
                case 2:
                {
                    inventory[0].Type = ITEM_TYPE_GRENADE_LAUNCHER_FLAME;
                    gGameTable.byte_691F6A = ITEM_TYPE_GRENADE_LAUNCHER_FLAME;
                    break;
                }
                case 3:
                {
                    inventory[0].Type = ITEM_TYPE_GRENADE_LAUNCHER_ACID;
                    gGameTable.byte_691F6A = ITEM_TYPE_GRENADE_LAUNCHER_ACID;
                    break;
                }
                case 4:
                {
                    inventory[0].Type = ITEM_TYPE_BOWGUN;
                    gGameTable.byte_691F6A = ITEM_TYPE_BOWGUN;
                    break;
                }
                case 5:
                {
                    inventory[0].Type = ITEM_TYPE_HANDGUN_COLT_SAA;
                    gGameTable.byte_691F6A = ITEM_TYPE_HANDGUN_COLT_SAA;
                    break;
                }
                case 6:
                {
                    set_inventory_item(0, ITEM_TYPE_SPARKSHOT, INVENTORY_INFINITE_QUANTITY, 1);
                    set_inventory_item(1, ITEM_TYPE_SPARKSHOT, INVENTORY_INFINITE_QUANTITY, 2);
                    gGameTable.byte_691F6A = ITEM_TYPE_SPARKSHOT;
                    break;
                }
                case 7:
                {
                    set_inventory_item(0, ITEM_TYPE_SUB_MACHINE_GUN, INVENTORY_INFINITE_QUANTITY, 1);
                    set_inventory_item(1, ITEM_TYPE_SUB_MACHINE_GUN, INVENTORY_INFINITE_QUANTITY, 2);
                    gGameTable.byte_691F6A = ITEM_TYPE_SUB_MACHINE_GUN;
                    break;
                }
                case 8:
                {
                    set_inventory_item(0, ITEM_TYPE_ROCKET_LAUNCHER, INVENTORY_INFINITE_QUANTITY, 1);
                    set_inventory_item(1, ITEM_TYPE_ROCKET_LAUNCHER, INVENTORY_INFINITE_QUANTITY, 2);
                    gGameTable.byte_691F6A = ITEM_TYPE_ROCKET_LAUNCHER;
                    break;
                }
                case 9:
                {
                    set_inventory_item(0, ITEM_TYPE_GATLING_GUN, INVENTORY_INFINITE_QUANTITY, 1);
                    set_inventory_item(1, ITEM_TYPE_GATLING_GUN, INVENTORY_INFINITE_QUANTITY, 2);
                    gGameTable.byte_691F6A = ITEM_TYPE_GATLING_GUN;
                    break;
                }
                }
            }
        }

        if (extremeBattleMode)
        {
            if (gGameTable.word_98EB20 >= 2)
            {
                gGameTable.byte_691F68 = 2;
                gGameTable.byte_691F6A = _initialInventory[initialInventoryOffset + 2].Type;
            }

            // Level determines the amount of ink ribbons
            if (gGameTable.ex_battle_mode == EX_BATTLE_MODE_LEVEL_2)
            {
                inventory[3].Quantity = 3;
            }
            else if (gGameTable.ex_battle_mode == EX_BATTLE_MODE_LEVEL_3)
            {
                inventory[3].Quantity = 1;
            }
            else
            {
                inventory[3].Quantity = 5;
            }
        }

        if (check_flag(FlagGroup::System, FG_SYSTEM_12))
        {
            set_inventory_item(0, ITEM_TYPE_SUB_MACHINE_GUN, INVENTORY_INFINITE_QUANTITY, 1);
            set_inventory_item(1, ITEM_TYPE_SUB_MACHINE_GUN, INVENTORY_INFINITE_QUANTITY, 2);
            gGameTable.byte_691F6A = ITEM_TYPE_SUB_MACHINE_GUN;
            gGameTable.byte_691F68 = 0;
        }

        std::memset(&gGameTable.itembox, 0, sizeof(ItemboxItem) * 64);
        gGameTable.inventory_size = 8;
        gGameTable.dword_98E9C4 = 0;
        bitarray_set(&gGameTable.dword_98E9C4, 0x12);
        bitarray_clr(gGameTable.fg_common, 0x7E);

        std::memset(&gGameTable.inventory_files, 0xFF, 24);

        if (extremeBattleMode)
        {
            set_itembox_item(2, ITEM_TYPE_KNIFE, 1, 0);
        }

        if (check_flag(FlagGroup::Status, FG_STATUS_EASY))
        {
            if (!check_flag(FlagGroup::System, FG_SYSTEM_10) && check_flag(FlagGroup::Status, FG_STATUS_SCENARIO))
            {
                if (isClaire)
                {
                    gGameTable.word_53E1B0 = 1;
                    gGameTable.inventory_files[0] = ITEM_TYPE_FILE_ROOKIE_CLAIRE;
                }
                else
                {
                    gGameTable.word_53E1AC = 1;
                    gGameTable.inventory_files[0] = ITEM_TYPE_FILE_ROOKIE_LEON;
                }
            }

            set_itembox_item(1, ITEM_TYPE_FIRST_AID_SPRAY, 1, 0);
            set_itembox_item(2, ITEM_TYPE_FIRST_AID_SPRAY, 1, 0);
            set_itembox_item(3, ITEM_TYPE_FIRST_AID_SPRAY, 1, 0);
        }

        if (check_flag(FlagGroup::System, FG_SYSTEM_EASY))
        {
            if (check_flag(FlagGroup::System, FG_SYSTEM_12))
            {
                set_inventory_item(0, ITEM_TYPE_SUB_MACHINE_GUN, INVENTORY_INFINITE_QUANTITY, 1);
                set_inventory_item(1, ITEM_TYPE_SUB_MACHINE_GUN, INVENTORY_INFINITE_QUANTITY, 2);
                gGameTable.byte_691F6A = ITEM_TYPE_SUB_MACHINE_GUN;
                gGameTable.byte_691F68 = 0;

                set_itembox_item(0, ITEM_TYPE_GATLING_GUN, INVENTORY_INFINITE_QUANTITY, 3);
                set_itembox_item(4, ITEM_TYPE_ROCKET_LAUNCHER, INVENTORY_INFINITE_QUANTITY, 3);
                set_itembox_item(5, ITEM_TYPE_KNIFE, 1, 0);

                if ((int32_t)gGameTable.fg_status >= 0)
                {
                    set_itembox_item(6, ITEM_TYPE_HANDGUN_LEON, 18, 0);
                }
                else
                {
                    set_itembox_item(6, ITEM_TYPE_HANDGUN_CLAIRE, 13, 0);
                }

                set_itembox_item(7, ITEM_TYPE_AMMO_HANDGUN, 120, 0);
            }
            else
            {
                set_inventory_item(3, ITEM_TYPE_AMMO_HANDGUN, 120, 0);
            }
        }

        if (gGameTable.cheat1 != 0xFF)
        {
            set_inventory_item(3, ITEM_TYPE_NONE, 0, 0);
            set_inventory_item(0, ITEM_TYPE_GATLING_GUN, INVENTORY_INFINITE_QUANTITY, 3);

            if (check_flag(FlagGroup::System, FG_SYSTEM_12))
            {
                set_itembox_item(4, ITEM_TYPE_ROCKET_LAUNCHER, 2, 3);
                set_itembox_item(5, ITEM_TYPE_GATLING_GUN, 100, 3);
                set_itembox_item(13, ITEM_TYPE_KNIFE, 1, 0);
            }

            itembox[12].Quantity = 100;
            itembox[12].Part = 3;

            if ((int32_t)gGameTable.fg_status >= 0)
            {
                set_itembox_item(6, ITEM_TYPE_HANDGUN_LEON, 18, 0);
                set_itembox_item(8, ITEM_TYPE_MAGNUM, 8, 0);
                set_itembox_item(9, ITEM_TYPE_CUSTOM_MAGNUM, 8, 0);
                set_itembox_item(10, ITEM_TYPE_SHOTGUN, 5, 0);
                set_itembox_item(11, ITEM_TYPE_CUSTOM_SHOTGUN, 7, 0);
                itembox[12].Type = ITEM_TYPE_SPARKSHOT;
            }
            else
            {
                set_itembox_item(6, ITEM_TYPE_HANDGUN_CLAIRE, 13, 0);
                set_itembox_item(8, ITEM_TYPE_GRENADE_LAUNCHER_FLAME, 6, 0);
                set_itembox_item(9, ITEM_TYPE_GRENADE_LAUNCHER_ACID, 6, 0);
                set_itembox_item(10, ITEM_TYPE_BOWGUN, 18, 0);
                set_itembox_item(11, ITEM_TYPE_HANDGUN_COLT_SAA, 6, 0);
                itembox[12].Type = ITEM_TYPE_SPARKSHOT;
            }

            set_itembox_item(7, ITEM_TYPE_AMMO_HANDGUN, 120, 0);
        }

        if (!extremeBattleMode)
        {
            if (gGameTable.cheat0 > 9)
            {
                return;
            }

            if (gGameTable.super_hard_switch)
            {
                if (gGameTable.cheat0 > 5)
                {
                    set_itembox_item(1, ITEM_TYPE_KNIFE, 1, 0);
                }

                itembox[0].Type = isClaire ? ITEM_TYPE_HANDGUN_CLAIRE : ITEM_TYPE_HANDGUN_LEON;
                itembox[0].Quantity = 13;
            }
        }
    }

    // 0x004C89B2
    void show_message(int a0, int a1, int a2, int a3)
    {
        interop::call<void, int, int, int, int>(0x004C89B2, a0, a1, a2, a3);
    }

    void* work_alloc(size_t len)
    {
        auto mem = gGameTable.mem_top;
        gGameTable.mem_top = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(mem) + len);
#ifdef DEBUG
        // Fill allocated memory with standard MS uninitialised byte
        // helps track what bytes have not been initialised.
        std::memset(mem, 0xCD, len);
#endif
        return mem;
    }

    // 0x004428F0
    int set_game_seconds(int a0)
    {
        return interop::call<int, int>(0x004428F0, a0);
    }

    // 0x0050AA00
    void* operator_new(const size_t size)
    {
        return interop::call<void*, size_t>(0x0050AA00, size);
    }

    // 0x0050AA10
    void operator_delete(void* memoryBlock)
    {
        interop::call<void*>(0x0050AA10, memoryBlock);
    }

    // 0x004E97C0
    void vsync() {}

    // 0x004C4000
    static void init_main()
    {
        interop::call(0x004C4000);
    }

    // 0x004CAD29
    static void moji_init()
    {
        interop::call(0x004CAD29);
    }

    // 0x00508B20
    static void trans_work_init()
    {
        interop::call(0x00508B20);
    }

    // 0x004C3C60
    static void line_work_init()
    {
        gGameTable.dword_991F70 = 0;
        gGameTable.dword_991F74 = 0;
    }

    // 0x00502D70
    static void memclr(uint32_t* address, int count)
    {
        do
        {
            *address++ = 0;
            --count;
        } while (count > 0);
    }

    // 0x00508B50
    static void task_null()
    {
        task_exit();
    }

    // 0x004C4D70
    static void init_global()
    {
        memclr(&gGameTable.fg_status, 0x1419);
        gGameTable.fg_message = 0;
        gGameTable.fg_system &= 0x1000899;
        gGameTable.fade_table[0].kido = -1;
        gGameTable.fade_table[0].kido = -1;
        gGameTable.fade_table[1].kido = -1;
        gGameTable.fade_table[2].kido = -1;
        gGameTable.fade_table[3].kido = -1;
        gGameTable.last_cut = -1;
        gGameTable.byte_98E9AA = gGameTable.byte_98F1B6;
        gGameTable.dword_9885AC = 0xFFFF0000;
        gGameTable.dword_9885D0 = 0xFFFF0000;
    }

    static uint8_t input_mapping[32]
        = { 0x68, 0x62, 0x64, 0x66, 0, 0x56, 0, 0x41, 0, 0,    0x58, 0x43, 0x11, 0x5A, 0,    0,
            0,    0,    0,    0,    0, 0,    0, 0,    0, 0x26, 0x28, 0x25, 0x27, 0x0D, 0x20, 0x1B };

    // 0x0043B950
    static void init_input()
    {
        for (uint32_t i = 0; i < 32; i++)
        {
            gGameTable.input.mapping[i] = input_mapping[i];
        }
    }

    // 0x004C3F10
    static void init_system()
    {
        init_input();
        scheduler_init();
        set_flag(FlagGroup::System, FG_SYSTEM_20, true);
        init_global();
        gGameTable.byte_9888D8 = 0;
        gGameTable.sfx_vol = 100;
        gGameTable.bgm_vol = 100;
        gGameTable.byte_9888D9 = 0;
        gGameTable.byte_98F1B6 = 0;
        gGameTable.dword_98F1AC = 0;
        gGameTable.dword_98F1B0 = 15728960;
        moji_init();
        trans_work_init();
        line_work_init();
        task_execute(0, init_main);
        task_execute(1, task_null);
        task_execute(2, task_null);
    }

    // 0x004D0F30
    static void pad_set()
    {
        interop::call(0x004D0F30);
    }

    // 0x00441870
    static void movie_set(int id)
    {
        interop::call(0x00441870);
    }

    // 0x00507C60
    static void trans_pointer_set()
    {
        interop::call(0x00507C60);
    }

    // 0x004CAE34
    static void moji_mode_init()
    {
        interop::call(0x004CAE34);
    }

    // 0x004C8CCA
    static void moji_trans_main()
    {
        interop::call(0x004C8CCA);
    }

    // 0x004C4AF0
    static void system_trans()
    {
        interop::call(0x004C4AF0);
    }

    // 0x004C4460
    static void swap_cbuff()
    {
        interop::call(0x004C4460);
    }

    // 0x004EEDF0
    static void cd_system_control()
    {
        interop::call(0x004EEDF0);
    }

    // 0x004C3C70
    static void psx_main()
    {
        if (!gGameTable.systemInitialized)
        {
            init_system();
            gGameTable.systemInitialized = 1;
        }

        gGameTable.byte_98F1B8 = 0;

        auto idx = *reinterpret_cast<uint32_t*>(&gGameTable.byte_9888D8) & 0xFF;

        gGameTable.dword_986520 = (uint32_t)&gGameTable.g_table + (idx << 5);
        gGameTable.dword_988524 = (uint32_t)&gGameTable.byte_986524 + (idx << 12);
        gGameTable.dword_9885A8 = (uint32_t)&gGameTable.byte_988528 + (idx << 6);
        auto mainOffset = (((idx * 8) + idx) * 2 + idx) * 8;
        gGameTable.dword_98F070 = (uint32_t)&gGameTable.main + mainOffset;

        pad_set();

        if (
            // clang-format off
        gGameTable.censorship_off
        && check_flag(FlagGroup::System, FG_SYSTEM_1)
        && !gGameTable.byte_98F07B
        && !gGameTable.byte_991F80
        && !check_flag(FlagGroup::Stop, FG_STOP_DISABLE_INPUT)
        && check_flags(FlagGroup::System, { FG_SYSTEM_DOOR_TRANSITION, FG_SYSTEM_10, FG_SYSTEM_BGM_DISABLED, FG_SYSTEM_22 })
        && check_flags(FlagGroup::Status, { FG_STATUS_11, FG_STATUS_CUTSCENE })
            // clang-format on
        )
        {
            if (gGameTable.pause)
            {
                movie_set(1);
                if (gGameTable.dword_9885F8 & 2 || gGameTable.vk_press & 0x20)
                {
                    marni::out();
                    gGameTable.pause = 0;
                    set_game_seconds(gGameTable.dword_689800);
                }
                vsync();
                return;
            }

            if (gGameTable.dword_9885F8 & 2 || gGameTable.vk_press & 0x20)
            {
                gGameTable.pause = 1;
                update_timer();
                gGameTable.dword_689800 = set_game_seconds(1);
                auto v0 = 16 * gGameTable.byte_9888D8;
                gGameTable.byte_52D8E7[v0] = 2;
                marni::add_tile(&gGameTable.curtain2[v0], 5, 0);
                // marni::prim14
                interop::call<void, int, int, int, int, int>(0x004C8603, 135, 107, 0, 0x4000, gGameTable.pause);
                marni::out();
            }
        }

        trans_pointer_set();
        moji_mode_init();
        scheduler();
        set_flag(FlagGroup::Status, FG_STATUS_INTERACT, false);
        set_flag(FlagGroup::Status, FG_STATUS_11, false);

        if (check_flag(FlagGroup::System, FG_SYSTEM_15))
        {
            if (hud_fade_status(0))
            {
                hud_fade_adjust(0, 31, 526344, 0);
            }

            auto tileIdx = gGameTable.byte_9888D8;
            auto& tile = gGameTable.fade_table->tiles[tileIdx];
            tile.code = 2;
            tile.tag = gGameTable.fade_table->hrate & 3;
            marni::add_tile(&tile, 0, 0);

            if (--gGameTable.fade_table->kido < 0)
            {
                set_flag(FlagGroup::System, FG_SYSTEM_15, false);
                return;
            }
        }
        else
        {
            if (!gGameTable.can_draw)
            {
                moji_trans_main();
            }

            system_trans();
            swap_cbuff();
            cd_system_control();
            gGameTable.vsync_rate = gGameTable.byte_98F07A;
        }
    }

    static void load_init_table(void* tempBuffer, uint8_t index)
    {
        if (read_file_into_buffer("common\\data\\init_tbl.dat", tempBuffer, 4) == 0)
        {
            file_error();
            return;
        }

        auto src = &((uint8_t*)tempBuffer)[index * 1944];
        std::memcpy(&gGameTable.table_start, src, 1944);
    }

    // 0x004B7860
    static void load_init_table_1()
    {
        load_init_table((void*)0x00999AE0, byte_989E7E);
    }

    // 0x004DE650
    static void load_init_table_2()
    {
        load_init_table((void*)0x008BD880, 5);
    }

    // 0x00505B20
    static void load_init_table_3()
    {
        _memTop = 0x008FF8A0;
        load_init_table((void*)0x008BD880, byte_989E7E);
    }

    void snd_se_walk(int, int, PlayerEntity* pEm) {}

    // 0x00509CF0
    bool ck_installkey()
    {
        return true;
    }

    // 0x00432080
    static void rsrc_release()
    {
        interop::call(0x00432080);
    }

    // 0x00433830
    static void ssclose()
    {
        interop::call(0x00433830);
    }

    // 0x00431000
    static void font_create()
    {
        interop::call(0x00431000);
    }

    // 0x004310A0
    static void font_delete()
    {
        DeleteObject(gGameTable.hFont);
    }

    // 0x00441780
    static void movie_kill()
    {
        if (gGameTable.movie_playing)
        {
            marni::movie_kill(gGameTable.pMarni);
            marni::syskeydown(gGameTable.pMarni);
            gGameTable.movie_playing = 0;
        }
    }

    // 0x00441DA0
    static void wnd_activate()
    {
        gGameTable.pause_game = 1;
        set_game_seconds(gGameTable.dword_6805C4);
        marni::out();
    }

    // 0x00441D60
    static void wnd_deactivate()
    {
        gGameTable.pause_game = 0;
        if (gGameTable.movie_r0 >= 2)
        {
            movie_kill();
            gGameTable.movie_r0 = 5;
        }
        gGameTable.dword_6805C4 = set_game_seconds(1);
        marni::out();
    }

    // 0x00442800
    static INT_PTR CALLBACK about_dialog(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        return interop::stdcall<INT_PTR, HWND, UINT, WPARAM, LPARAM>(0x00442800, hWnd, msg, wParam, lParam);
    }

    // 0x00442750
    static void screenshot()
    {
        interop::call(0x00442750);
    }

    // 0x00442C60
    static void cursor_op()
    {
        interop::call(0x00442C60);
    }

    // 0x00441A00
    LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        auto marni = gGameTable.pMarni;
        if (marni != nullptr)
        {
            auto result = marni::message(marni, hWnd, Msg, (void*)wParam, (void*)lParam);
            if (result == 0)
            {
                return 0;
            }
        }
        gGameTable.vk_press &= 0x1F;
        switch (Msg)
        {
        case WM_CREATE: input_init(&gGameTable.input); break;
        case WM_DESTROY:
            gGameTable.hwnd = nullptr;
            rsrc_release();
            ssclose();
            font_delete();
            PostQuitMessage(0);
            return 0;
        case WM_ACTIVATE: wnd_activate(); break;
        case WM_ACTIVATEAPP:
            if (wParam)
                wnd_activate();
            else
                wnd_deactivate();
            break;
        case WM_KILLFOCUS: input_pause(&gGameTable.input); break;
        case WM_CLOSE: marni::kill(); return DefWindowProc(hWnd, Msg, wParam, lParam);
        case WM_KEYUP: input_wmkeyup(&gGameTable.input, wParam); break;
        case WM_KEYDOWN:
            if (lParam & 0x40000000) // last key state?
                break;
            gGameTable.byte_689ABC = 1;
            gGameTable.vk_press |= 0x80;
            switch (wParam)
            {
            case VK_SNAPSHOT:
                screenshot();
                SetFocus(hWnd);
                break;
            case VK_F1: DialogBoxParamA((HINSTANCE)gGameTable.hInstance, (LPCSTR)0xA6, hWnd, about_dialog, 0); break;
            case VK_F4:
                gGameTable.vk_press |= 1; // inventory
                SetFocus(hWnd);
                break;
            case VK_F5:
                gGameTable.vk_press |= 2; // options
                SetFocus(hWnd);
                break;
            case VK_F7: marni::config_flip_filter(&gGameTable.marni_config); break;
            case VK_F8:
                if (!gGameTable.byte_68059B && gGameTable.tasks[1].fn != (void*)0x004BF760 && !gGameTable.movie_r0) // gallery
                {
                    if (marni::change_resolution(gGameTable.pMarni))
                    {
                        gGameTable.byte_680591 = 120;
                        cursor_op();
                        gGameTable.is_480p = gGameTable.pMarni->xsize != 320;
                        font_create();
                    }
                    else
                    {
                        marni::out("???", "winmain.cpp");
                    }
                }
                break;
            case VK_F9:
                gGameTable.vk_press |= 0x40; // exit to menu
                break;
            default:
                input_wmkeydown(&gGameTable.input, wParam);
                SetFocus(hWnd);
                break;
            }
            break;
        default: return DefWindowProc(hWnd, Msg, wParam, lParam);
        }
        return 0;
    }

    // 0x00441910
    static int cheat_line_cmd0(LPSTR lpCmdLine, int a1)
    {
        return interop::call<int, LPSTR, int>(0x00441910, lpCmdLine, a1);
    }

    // 0x00441890
    static int cheat_line_cmd1(LPSTR lpCmdLine, int a1, int a2)
    {
        return interop::call<int, LPSTR, int, int>(0x00441890, lpCmdLine, a1, a2);
    }

    // 0x0050AA60
    static void config_read()
    {
        marni::config_read_all(&gGameTable.marni_config);
        marni::config_flush_all(&gGameTable.marni_config);
    }

    // 0x0050AA80
    static void config_write()
    {
        marni::config_flush_all(&gGameTable.marni_config);
    }

    // 0x004310B0
    static void save_reset()
    {
        interop::call(0x004310B0);
    }

    // 0x00441880
    static void make_font()
    {
        interop::call(0x00441880);
    }

    // 0x00442920
    static void draw_monitor_effect(int a0)
    {
        interop::call(0x00442920);
    }

    // 0x004DD3B0
    static void psp_trans()
    {
        interop::call(0x004DD3B0);
    }

    // 0x004CD090
    static void om_trans()
    {
        interop::call(0x004CD090);
    }

    // 0x004310F0
    static void merge_surface_gdi()
    {
        interop::call(0x004310F0);
    }

    // 0x004CAF90
    static void movie()
    {
        interop::call(0x004CAF90);
    }

    // 0x00440250
    static void reset_geom()
    {
        interop::call(0x00440250);
    }

    // 0x00442A50
    static void reset_screen()
    {
        interop::call(0x00442A50);
    }

    static int win_exit(uint32_t error)
    {
        static const char* aHighColor16bit = (const char*)0x00525098;
        static const char* aInNIN = (const char*)0x0052506C;

        switch (error)
        {
        case ERROR_0: [[fallthrough]];
        case ERROR_1: [[fallthrough]];
        case ERROR_2: [[fallthrough]];
        case ERROR_11: [[fallthrough]];
        case ERROR_18: [[fallthrough]];
        case ERROR_255: break;

        case ERROR_FAILED_TO_INITIALIZE_DIRECTX:
        {
            MessageBoxA(0, "Failed to initialize DIRECTX(R).", windowTitle, MB_ICONEXCLAMATION);
            break;
        }
        case ERROR_INSERT_DISC:
        {
            MessageBoxA(0, "Please insert BIOHAZARD(R) 2 PC DISC", windowTitle, MB_ICONEXCLAMATION);
            break;
        }
        case ERROR_17:
        {
            MessageBoxA(0, aHighColor16bit, windowTitle, MB_ICONEXCLAMATION);
            break;
        }
        case ERROR_19:
        {
            MessageBoxA(0, aInNIN, windowTitle, MB_ICONEXCLAMATION);
            break;
        }
        default:
        {
            MessageBoxA(0, "Fatal error.", windowTitle, MB_ICONEXCLAMATION);
            break;
        }
        }

        marni::config_shutdown();
        if (gGameTable.hMutex)
        {
            CloseHandle((HANDLE)gGameTable.hMutex);
        }

        return error;
    }

    // 0x00441DC0
    static bool init_instance(HINSTANCE hInstance, HINSTANCE hPrevInstance)
    {
        gGameTable.hInstance = hInstance;
        if (!hPrevInstance)
        {
            WNDCLASSA wndClass = {};
            wndClass.lpfnWndProc = WndProc;
            wndClass.cbClsExtra = 0;
            wndClass.cbWndExtra = 0;
            wndClass.hInstance = hInstance;
            wndClass.hIcon = LoadIconA(hInstance, (LPCSTR)0xA3);
            wndClass.hCursor = LoadCursorA(0, (LPCSTR)0x7F00);
            wndClass.hbrBackground = (HBRUSH)GetStockObject(4);
            wndClass.lpszMenuName = 0;
            wndClass.lpszClassName = windowTitle;
            RegisterClassA(&wndClass);
        }

        DWORD windowStyleFlags = WS_CLIPCHILDREN | WS_BORDER | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;

        RECT windowRect;
        windowRect.left = 0;
        windowRect.right = 640;
        windowRect.top = 0;
        windowRect.bottom = 480;
        AdjustWindowRect(&windowRect, windowStyleFlags, 0);

        gGameTable.hwnd = (void*)CreateWindowExA(
            0,
            windowTitle,
            windowTitle,
            windowStyleFlags,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            NULL,
            NULL,
            hInstance,
            NULL);

        auto window = (HWND)gGameTable.hwnd;

        ShowWindow(window, SW_NORMAL);
        SetForegroundWindow(window);
        UpdateWindow(window);

        return true;
    }

    // 0x00441ED0
    int win_main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
    {
        const char* mutexName = "bio2.658b45ea117473d4.game";
        constexpr uint32_t frameRateTable[4] = { 166, 333, 666, 166 };

        auto time = 0;
        auto frameTime = 0;
        auto targetFrameRate = 0;
        uint32_t currentTime = 0;

        gGameTable.hMutex = OpenMutexA(MUTEX_ALL_ACCESS, 0, mutexName);
        if (gGameTable.hMutex)
        {
            return win_exit(ERROR_18);
        }
        gGameTable.hMutex = CreateMutexA(0, 0, mutexName);

        marni::out();
        config_read();

        gGameTable.cheat0 = cheat_line_cmd0(lpCmdLine, 9);
        gGameTable.cheat1 = cheat_line_cmd1(lpCmdLine, 11, 1);
        if (cheat_line_cmd1(lpCmdLine, 12, 1) != -1)
        {
            gGameTable.ushinabe = 1;
        }
        SystemParametersInfoA(SPI_GETSCREENSAVEACTIVE, FALSE, &gGameTable.byte_680590, 0);
        if (gGameTable.byte_680590)
        {
            SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, 0, FALSE, SPIF_SENDWININICHANGE);
        }

        if (init_instance(hInstance, hPrevInstance))
        {
            auto window = (HWND)gGameTable.hwnd;
            ImmAssociateContext(window, NULL);

            auto marniPtr = (Marni*)operator_new(sizeof(Marni));
            gGameTable.pMarni = marni::init(marniPtr, window, 320, 240);
            if (!gGameTable.pMarni->is_gpu_active || !marni::request_display_mode_count(gGameTable.pMarni))
            {
                win_exit(ERROR_FAILED_TO_INITIALIZE_DIRECTX);
                DestroyWindow(window);
                window = 0;
            }

            cursor_op();
            gGameTable.pMarni->gpu_flag |= marni::GpuFlags::GPU_3;
            marni::set_gpu_flag();
            if (gGameTable.pMarni->gpu_flag & marni::GpuFlags::GPU_13)
            {
                gGameTable.graphics_ptr_data = 1;
            }
            else
            {
                gGameTable.graphics_ptr_data = (gGameTable.pMarni->gpu_flag & marni::GpuFlags::GPU_3) ? 0 : 2;
            }
            update_timer();

            while (true)
            {
                while (true)
                {
                    MSG msg;
                    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
                    {
                        if (msg.message == WM_QUIT)
                        {
                            marni::kill();
                            config_write();
                            if (gGameTable.byte_680592 == 1)
                            {
                                gGameTable.byte_680592 = 0;
                                ShowCursor(true);
                            }

                            SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, gGameTable.byte_680592, 0, 2);
                            return 0;
                        }

                        TranslateMessage(&msg);
                        DispatchMessageA(&msg);
                    }

                    lpCmdLine = (LPSTR)0x1040201;

                    if (gGameTable.timer_r0 == 0)
                    {
                        break;
                    }
                    if (gGameTable.timer_r0 == 1)
                    {
                        currentTime = gGameTable.timer_current;
                        goto LABEL_64;
                    }

                    if (gGameTable.timer_r0 == 2)
                    {
                        gGameTable.timer_current = timeGetTime();
                        if (10 * (gGameTable.timer_current - gGameTable.timer_last)
                            >= frameRateTable[gGameTable.vsync_rate / 2])
                        {
                            gGameTable.timer_r0 = 1;
                        }
                    }
                }

                // Window is not active
                if (!gGameTable.pause_game)
                {
                    continue;
                }

                // Playing movie
                if (gGameTable.movie_r0)
                {
                    marni::clear_otags(gGameTable.pMarni);
                    reset_geom();
                    gGameTable.pMarni->gpu_flag &= ~marni::GpuFlags::GPU_3;
                    movie();
                    marni::clear(gGameTable.pMarni);
                    marni::marni_movie_update(gGameTable.pMarni);
                    continue;
                }
                if (gGameTable.reset_r0)
                {
                    marni::clear_otags(gGameTable.pMarni);
                    reset_geom();
                    gGameTable.pMarni->gpu_flag |= marni::GpuFlags::GPU_3;
                    reset_screen();
                    marni::clear(gGameTable.pMarni);
                    marni::draw(gGameTable.pMarni);
                    marni::flip(gGameTable.pMarni);
                }
                else
                {
                    marni::clear_otags(gGameTable.pMarni);
                    reset_geom();
                    gGameTable.byte_6805B4 = 0;
                    gGameTable.pMarni->gpu_flag &= ~marni::GpuFlags::GPU_3;
                    gGameTable.timer_r2 = 0;
                    save_reset();
                    if (gGameTable.byte_680597 & 1)
                    {
                        gGameTable.byte_680597 |= 2;
                    }
                    psx_main();
                    if (++gGameTable.timer_frame > 100)
                    {
                        gGameTable.timer_frame = 100;
                    }
                    gGameTable.frame_current += *((uint8_t*)&lpCmdLine + gGameTable.vsync_rate / 2);
                    if (gGameTable.frame_current > 60)
                    {
                        gGameTable.frame_current = 0;
                        ++gGameTable.game_seconds;
                    }
                    currentTime = gGameTable.timer_current;
                    if ((gGameTable.timer_current & 0x80000000) < (gGameTable.timer_last & 0x80000000))
                    {
                        update_timer();
                    }
                    gGameTable.timer_10 += frameRateTable[gGameTable.vsync_rate / 2];
                    if (gGameTable.timer_r1)
                    {
                        goto LABEL_54;
                    }

                    time = timeGetTime();
                    currentTime = time;
                    gGameTable.timer_current = time;
                    frameTime = 10 * (time - gGameTable.timer_last);
                    targetFrameRate = frameRateTable[gGameTable.vsync_rate / 2];

                    if (frameTime >= targetFrameRate)
                    {
                        if (gGameTable.timer_r2 == 0 && frameTime > 3 * targetFrameRate)
                        {
                            gGameTable.timer_10 += targetFrameRate * (frameTime / targetFrameRate - 2);
                        }

                    LABEL_54:
                        gGameTable.timer_r1 = 0;
                        if ((gGameTable.vsync_rate && gGameTable.timer_frame >= 15) || gGameTable.timer_frame >= 30)
                        {
                            if (gGameTable.timer_10 + 10000 < 10 * currentTime)
                            {
                                update_timer();
                                currentTime = gGameTable.timer_current;
                            }
                        }

                        if (10 * currentTime > gGameTable.timer_10)
                        {
                            gGameTable.timer_r1 = 1;
                            gGameTable.timer_r0 = 0;
                            continue;
                        }

                    LABEL_64:
                        gGameTable.timer_frame = 0;
                        gGameTable.timer_last = currentTime;
                        make_font();
                        if (gGameTable.pMarni)
                        {
                            if (gGameTable.movie_idx)
                            {
                                gGameTable.movie_idx--;
                            }
                            else
                            {
                                if (!gGameTable.byte_6805B4 && !gGameTable.byte_680598)
                                {
                                    gGameTable.pMarni->gpu_flag |= marni::GpuFlags::GPU_3;
                                }
                                // 0x004BF760: gallery function
                                if ((uint32_t)gGameTable.tasks[1].fn == 0x004BF760)
                                {
                                    gGameTable.byte_680593 = gGameTable.byte_680592;
                                    gGameTable.byte_680592 |= 1;
                                    marni::set_gpu_flag();
                                    gGameTable.byte_680592 = gGameTable.byte_680593;
                                }
                                else
                                {
                                    marni::set_gpu_flag();
                                    gGameTable.scaler.type = 15872;
                                    if (gGameTable.pMarni->xsize == 640)
                                    {
                                        gGameTable.scaler.rate_x = 2.0f;
                                        gGameTable.scaler.rate_y = 2.0f;
                                    }
                                    else
                                    {
                                        gGameTable.scaler.rate_x = 1.0f;
                                        gGameTable.scaler.rate_y = 1.0f;
                                    }
                                    gGameTable.scaler.prj = gGameTable.global_prj;
                                    gGameTable.scaler.rgb0 = gGameTable.global_rgb;
                                    gGameTable.scaler.c_x = gGameTable.global_cx + 160;
                                    gGameTable.scaler.c_y = gGameTable.global_cy + 120;
                                    marni::add_primitive_scaler(gGameTable.pMarni, &gGameTable.scaler, 4095);
                                }

                                marni::clear(gGameTable.pMarni);
                                marni::draw(gGameTable.pMarni);

                                if (gGameTable.can_draw)
                                {
                                    draw_monitor_effect(gGameTable.can_draw);
                                    marni::clear_otags(gGameTable.pMarni);
                                    psp_trans();
                                    om_trans();
                                    moji_trans_main();
                                    marni::draw(gGameTable.pMarni);
                                    gGameTable.can_draw = 0;
                                }

                                merge_surface_gdi();
                                marni::font_trans(&gGameTable.marni_font, &gGameTable.pMarni->surface0);
                                marni::flip(gGameTable.pMarni);
                            }
                        }

                        gGameTable.timer_r0 = 0;
                        continue;
                    }

                    gGameTable.timer_r0 = 2;
                }
            }
        }

        return 0;
    }
}

void onAttach()
{
    uint8_t b{};
    interop::readMemory(0x401E40, &b, sizeof(b));
    gClassicRebirthEnabled = (b == 0xE9);

    interop::writeJmp(0x004B7860, load_init_table_1);
    interop::writeJmp(0x004DE650, load_init_table_2);
    interop::writeJmp(0x00505B20, load_init_table_3);
    interop::writeJmp(0x004B2A90, rnd);
    interop::writeJmp(0x00509CF0, ck_installkey);
    interop::writeJmp(0x00441A00, WndProc);
    interop::writeJmp(0x004C3C70, psx_main);
    interop::writeJmp(0x00441ED0, win_main);

    scheduler_init_hooks();
    title_init_hooks();
    door_init_hooks();
    scd_init_hooks();
    sce_init_hooks();
    player_init_hooks();
    bgm_init_hooks();
    hud_init_hooks();
    camera_init_hooks();
    enemy_init_hooks();
    file_init_hooks();
    math_init_hooks();
    tim::tim_init_hooks();
    if (!gClassicRebirthEnabled)
    {
        input_init_hooks();
        marni::init_hooks();
    }
}

extern "C" {
__declspec(dllexport) BOOL /* WINAPI */
openre_main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize once for each new process.
        // Return FALSE to fail DLL load.
        onAttach();
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;

    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        break;
    }
    return TRUE; // Successful DLL_PROCESS_ATTACH.
}
}
