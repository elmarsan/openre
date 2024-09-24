#define _CRT_SECURE_NO_WARNINGS

#include "room.h"
#include "audio.h"
#include "camera.h"
#include "enemy.h"
#include "file.h"
#include "interop.hpp"
#include "marni.h"
#include "openre.h"
#include "player.h"
#include "rdt.h"
#include "sce.h"
#include <cstdint>
#include <cstring>

using namespace openre;
using namespace openre::audio;
using namespace openre::camera;
using namespace openre::enemy;
using namespace openre::file;
using namespace openre::player;
using namespace openre::rdt;
using namespace openre::sce;

namespace openre::room
{
    // 0x0043FF40
    static int32_t tim_buffer_to_surface(uint32_t* timPtr, uint32_t page, int mode)
    {
        return interop::call<int32_t>(0x0043FF40, timPtr, page, mode);
    }

    // 0x004B8080
    static void esp_init_r()
    {
        interop::call<void>(0x004B8080);
    }

    // 0x0043F550
    static uint32_t unload_texture_page(int page)
    {
        return interop::call<uint32_t, int>(0x0043F550, page);
    }

    // 0x004B4480
    static void rbj_set()
    {
        interop::call<void>(0x004B4480);
    }

    // 0x004DD0C0
    static void psp_init0()
    {
        interop::call<void>(0x004DD0C0);
    }

    // 0x004DD0E0
    static void psp_init1()
    {
        interop::call<void>(0x004DD0E0);
    }

    // 0x005023D0
    static void st_room_set()
    {
        interop::call<void>(0x005023D0);
    }

    // 0x00441710
    static void flush_surfaces()
    {
        interop::call<void>(0x00441710);
    }

    // 0x00502190
    static void st_chenge_pl(int a1)
    {
        interop::call<void, int>(0x00502190, a1);
    }

    // 0x004DE7B0
    static void set_room()
    {
        int i, j;

        while (true)
        {
            switch (gGameTable.ctcb->var_0D)
            {
            case 0:
            {
                strcpy(gGameTable.room_path, "Pl0\\Rdt\\room1000.rdt");
                static const char* font1 = "common\\data\\font1.tim";
                static const char* font2 = "common\\data\\font1.adt";                

                if (gGameTable.graphics_ptr_data == 1)
                {
                    std::memcpy(gGameTable.stage_font_name, font1, 22);
                }
                else
                {
                    std::memcpy(gGameTable.stage_font_name, font2, 22);
                }

                gGameTable.pEm = &gGameTable.pl;
                if (get_player_num() == 1)
                {
                    ++gGameTable.room_path[2];
                    ++gGameTable.room_path[15];
                }
                gGameTable.room_path[12] = gStageSymbols[(gGameTable.dword_98E798 & 0xFF) + gGameTable.current_stage];
                gGameTable.room_path[13] += gGameTable.current_room / 16;

                const auto mod = gGameTable.current_room % 16;
                if (mod)
                {
                    gGameTable.room_path[14] = mod + 87;
                }
                else
                {
                    gGameTable.room_path[14] = mod + 48;
                }
                // if (get_player_num() == 1)
                //{
                //     ++gGameTable.room_path[15];
                // }
                switch (gGameTable.current_stage)
                {
                case 0:
                {
                    if (gGameTable.current_room == 2)
                    {
                        set_registry_flag(0, 48);
                    }
                    break;
                }
                case 1:
                {
                    switch (gGameTable.current_room)
                    {
                    case 0:
                    {
                        set_registry_flag(0, 48);
                        break;
                    }
                    case 2:
                    {
                        set_registry_flag(0, 49);
                        break;
                    }
                    case 4:
                    {
                        set_registry_flag(0, 51);
                        break;
                    }
                    case 7:
                    {
                        set_registry_flag(0, 50);
                        break;
                    }
                    case 8:
                    {
                        set_registry_flag(0, 53);
                        break;
                    }
                    case 10:
                    {
                        set_registry_flag(0, 52);
                        break;
                    }
                    case 11:
                    {
                        set_registry_flag(0, 55);
                        [[fallthrough]];
                    }
                    case 27:
                    {
                        set_registry_flag(0, 55);
                        break;
                    }
                    default: break;
                    }
                }
                case 2:
                {
                    if (gGameTable.current_room == 5)
                    {
                        set_registry_flag(0, 57);
                    }
                    break;
                }
                case 3:
                {
                    if (gGameTable.current_room == 5)
                    {
                        set_registry_flag(0, 59);
                    }
                    break;
                }
                case 4:
                {
                    if (gGameTable.current_room == 0)
                    {
                        set_registry_flag(0, 58);
                    }
                    break;
                }
                default: break;
                }

                gGameTable.dword_98E798 = 0;
                gGameTable.fg_rbj_set = 0;
                // fg_status &= 0xFFF04000;
                gGameTable.pEm->pOn_om = 0;
                gGameTable.pEm->status_flg = (gGameTable.pEm->status_flg & 0xF9 << 16 | gGameTable.pEm->status_flg & 0xff);
                gGameTable.mem_top = reinterpret_cast<void*>(gGameTable.dword_988620);
                gGameTable.rdt = reinterpret_cast<Rdt*>(gGameTable.dword_988620);
                gGameTable.ctcb->var_0D = 10;
                goto LABEL_35;
            }
            case 1:
            {
                goto LABEL_62;
            }
            case 2:
            {
                goto LABEL_63;
            }
            case 3:
            {
                if (gGameTable.current_stage == gGameTable.byte_989E7D)
                {
                    dword_68A204->var_0D = 5;
                    goto LABEL_44;
                }

                gGameTable.byte_989E7D = gGameTable.current_stage & 0xff;
                if (gGameTable.stage_bk == gGameTable.current_stage)
                {
                    gGameTable.ctcb->var_0D = 4;
                    task_sleep(1);
                    return;
                }
                void* fBuff = file_alloc(0x20014);
                gGameTable.stage_font_name[16] += gGameTable.byte_989E7D;
                size_t adtSize = {};
                switch (gGameTable.graphics_ptr_data)
                {
                case 1:
                {
                    if (!read_file_into_buffer(gGameTable.stage_font_name, fBuff, 4))
                    {
                        file_error();
                        return;
                    }
                    break;
                }
                case 0: [[fallthrough]];
                case 2:
                {
                    if (!load_adt(gGameTable.stage_font_name, &adtSize, 4))
                    {
                        file_error();
                        return;
                    }
                    break;
                }
                }
                tim_buffer_to_surface(reinterpret_cast<uint32_t*>(fBuff), 9, 1);
                file_alloc(0);
                gGameTable.stage_bk = gGameTable.byte_989E7D;

                gGameTable.ctcb->var_0D = 4;
                task_sleep(1);
                return;
            }
            case 4: [[fallthrough]];
            case 5:
            {
                gGameTable.word_989EE8 = 3333;
                osp_read();
                gGameTable.byte_689C64 = 1;
                gGameTable.rdt_size = read_file_into_buffer(gGameTable.room_path, gGameTable.rdt, 8);
                if (gGameTable.rdt_size)
                {
                    file_error();
                    return;
                }

                gGameTable.rdt_top_ptr = rdt_get_offset<void*>(RdtOffsetKind::EDT);
                // do
                //{
                //     if (*rdtOffset)
                //     {
                //         *rdtOffset = (char*)*rdtOffset + rdtHeader;
                //         rdtHeader = (int)lpBuffer;
                //         rdtOffset = (LPVOID*)rdt_p_top;
                //     }
                //     rdt_p_top = (int)++rdtOffset;
                // } while ((unsigned int)rdtOffset < rdtHeader + 100);

                gGameTable.rdt_nCount = 0;
                if (gGameTable.rdt->header.num_cuts)
                {
                }

                //////////////////////////////////
                //////////////////////////////////
                //////////////////////////////////
                cut_change(gGameTable.current_cut & 0xff);
                esp_init_r();
                gGameTable.ctcb->var_0D = 6;
            LABEL_84:
                snd_room_load();
                if (!gGameTable.ctcb->var_13)
                {
                    unload_texture_page(17);
                    sce_model_init();
                    snd_bgm_play_ck();
                    // if (lpBuffer[4])
                    if (gGameTable.rdt->header.num_doors)
                    {
                        gGameTable.mem_top = reinterpret_cast<void*>(gGameTable.rdt->header.num_doors);
                    }
                    gGameTable.dword_98862C = gGameTable.player_work;
                    gGameTable.pl.routine_0 = 0;
                    gGameTable.pl.routine_1 = 0;
                    gGameTable.pl.routine_2 = 0;
                    gGameTable.pl.routine_3 = 0;
                    player_move(&gGameTable.pl);
                    sce_scheduler_set();
                    gGameTable.ctcb->var_0D = 7;
                LABEL_88:
                    snd_load_em();
                    if (!gGameTable.ctcb->var_13)
                    {
                        rbj_set();
                        psp_init0();
                        gGameTable.ctcb->var_0D = 8;
                    LABEL_90:
                        marni::out();
                        if (gGameTable.ctcb->var_13)
                        {
                            em_init_move();
                            psp_init1();
                            gGameTable.ctcb->var_13 = 9;
                        LABEL_92:
                            if (gGameTable.byte_99270F)
                            {
                                task_sleep(1);
                            }
                            else
                            {
                                st_room_set();
                                flush_surfaces();
                                gGameTable.ctcb->var_0D = 0;
                            }
                        }
                    }
                }
                return;
            }
            case 6:
            {
                goto LABEL_84;
            }
            case 7:
            {
                goto LABEL_88;
            }
            case 8:
            {
                goto LABEL_90;
            }
            case 9:
            {
                goto LABEL_92;
            }
            case 10:
            {
            LABEL_35:
                snd_bgm_set();
                if (gGameTable.ctcb->var_13)
                {
                    return;
                }
                sub_4450C0(0);

                gGameTable.dword_98862C = gGameTable.enemies[0];
                gGameTable.enemy_count = 0;
                std::memset(gGameTable.splayer_work, reinterpret_cast<int>(gGameTable.unknown_98E544), 33);
                i = 33;
                j = i;
                do
                {
                    --i;
                    --j;
                } while (j);
                gGameTable.rdt_nCount = 0;
                i = 32;
                do
                {
                    --i;
                    gGameTable.pOm.be_flg = 0;
                } while (i);
                if (gGameTable.pEm->id == gGameTable.next_pld)
                {
                    gGameTable.ctcb->var_0D = 2;
                LABEL_44:
                    if (dword_68A204->var_0D > 10)
                    {
                        return;
                    }
                    continue;
                }
                if (gGameTable.next_pld < 12)
                {
                    if (gGameTable.next_pld & 1)
                    {
                        if (check_flag(FlagGroup::Zapping, FG_ZAPPING_6))
                        {
                            gGameTable.next_pld = 9;
                        }
                    }
                    else
                    {
                        if (check_flag(FlagGroup::Zapping, FG_ZAPPING_5))
                        {
                            gGameTable.next_pld = 8;
                        }
                        if (check_flag(FlagGroup::Zapping, FG_ZAPPING_15))
                        {
                            gGameTable.next_pld = 10;
                        }
                    }
                }
                gGameTable.dword_689BDC = gGameTable.pEm->id & 0xff;
                gGameTable.pEm->id = gGameTable.next_pld & 0xff;
                st_chenge_pl(gGameTable.next_pld);
                player_set(gGameTable.pEm);
                if (!dword_68A204->var_13)
                {
                    gGameTable.pEm->routine_0 = 0;
                    gGameTable.pEm->routine_1 = 0;
                    gGameTable.pEm->routine_2 = 0;
                    gGameTable.pEm->routine_3 = 0;

                    if (gGameTable.next_pld == 14 || gGameTable.next_pld == 15)
                    {
                        // TODO: casting? or set word to signed word
                        gGameTable.word_98EAE6 = static_cast<uint16_t>(gGameTable.pl.life);
                        gGameTable.byte_98E9AB = gGameTable.poison_timer;
                        gGameTable.word_98E9AC = gGameTable.poison_status;
                        gGameTable.poison_timer = 0;
                        gGameTable.poison_status = 0;
                        gGameTable.pl.life = gGameTable.pl.max_life;
                    }
                    else if ((gGameTable.dword_689C1C & 0xff) > 12)
                    {
                        gGameTable.pl.life = gGameTable.word_98E9B6;
                        gGameTable.poison_timer = gGameTable.byte_98E9AB;
                        gGameTable.poison_status = gGameTable.word_98E9AC;
                        gGameTable.pl.life = gGameTable.pl.max_life;
                    }

                    gGameTable.byte_691F7B = 1;
                    gGameTable.ctcb->var_0D = 1;

                LABEL_62:
                    snd_load_core(gGameTable.next_pld, 1);
                    if (dword_68A204->var_13 == 0)
                    {
                    LABEL_63:
                        gGameTable.byte_99270F = 0;
                        dword_68A204->var_13 = 3;
                        task_sleep(1);
                    }
                }
                return;
            }
            default: return;
            }
        }
    }

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