#include "re2.h"
#include <cstddef>
#define _CRT_SECURE_NO_WARNINGS

#include "sce.h"
#include "audio.h"
#include "file.h"
#include "interop.hpp"
#include "marni.h"
#include "openre.h"
#include "player.h"
#include "rdt.h"
#include "room.h"
#include "camera.h"

#include <cstdint>
#include <cstring>

using namespace openre::file;
using namespace openre::audio;
using namespace openre::marni;
using namespace openre::sce;
using namespace openre::player;
using namespace openre::rdt;
using namespace openre::camera;

namespace openre::room
{
    static char* _rdtPathBuffer = (char*)0x689C20;
    static uint32_t& dword_98E798 = *((uint32_t*)0x98E798);
    static const char* _stageSymbols = "123456789abcdefg";
    static uint32_t& dword_988620 = *((uint32_t*)0x988620);
    static uint32_t& dword_989E68 = *((uint32_t*)0x989E68);
    static uint16_t& word_98E78C = *((uint16_t*)0x98E78C);
    static uint32_t& dword_98861C = *((uint32_t*)0x98861C);
    static uint8_t& byte_99270F = *((uint8_t*)0x99270F);
    static uint8_t& byte_989E7D = *((uint8_t*)0x989E7D);
    static void*& byte_98861C = *((void**)0x98861C);
    static uint16_t& word_989EE8 = *((uint16_t*)0x989EE8);
    static uint32_t& dword_98862C = *((uint32_t*)0x98862C);
    static uint8_t& byte_989EEB = *((uint8_t*)0x989EEB);
    static void** dword_98A110 = (void**)0x98A110;
    static char* dword_98E544 = (char*)0x98E544;
    static uint32_t& _rdtnCount = *((uint32_t*)0x689C10);
    static uint16_t& word_98A616 = *((uint16_t*)0x98A616);
    static uint16_t& word_98A61A = *((uint16_t*)0x98A61A);
    static ObjectEntity*& dword_98E51C = *((ObjectEntity**)0x98E51C);
    static ObjectEntity* _om = (ObjectEntity*)0x98A61C;
    static uint16_t& word_98EB24 = *((uint16_t*)0x98EB24);
    static uint32_t& dword_68059C = *((uint32_t*)0x68059C);
    static uint32_t& dword_988628 = *((uint32_t*)0x988628);

    // osp stuff
    static uint8_t* _ospBuffer = (uint8_t*)0x698840;
    static uint8_t& _ospMaskFlag = *((uint8_t*)0x6998C0);

    // 0x004B4480
    static uint8_t rbj_set()
    {
        using sig = uint8_t (*)();
        auto p = (sig)0x004B4480;
        return p();
    }

    // 0x004DD0C0
    static int psp_init0()
    {
        using sig = uint8_t (*)();
        auto p = (sig)0x004DD0C0;
        return p();
    }
    
    // 0x004B8080
    static void Esp_init_R()
    {
        using sig = void (*)();
        auto p = (sig)0x004B8080;
        p();
    }
    
    // 0x004DD0E0
    static void psp_init1()
    {
        using sig = uint8_t (*)();
        auto p = (sig)0x004DD0E0;
        p();
    }

    // 0x004EC7D0
    static char snd_room_load()
    {
        using sig = char (*)();
        auto p = (sig)0x004EC7D0;
        return p();
    }

    // TODO: Easy to decompile
    // 0x005023D0
    static int st_room_set()
    {
        using sig = int (*)();
        auto p = (sig)0x005023D0;
        return p();
    }

    static void set_registry_flg(int index, int sub)
    {
        *((uint32_t*)&dword_68059C + index + (sub >> 5)) |= 0x80000000 >> (sub & 0x1F);
    }

    // TODO: move to osp.h ??
    // 0x004DD360
    static void read_osp()
    {
        _ospMaskFlag = 1;
        auto eax = (gGameTable.current_stage * 32) + gGameTable.current_room;
        auto edx = (eax * 33) * 128;
        auto bytesRead = read_partial_file_into_buffer("common\\bin\\osp.bin", _ospBuffer, edx, 4224, 4);
        if (bytesRead == 0)
        {
            gErrorCode = bytesRead;
            _ospMaskFlag = 0;
        }
    }

    // 0x004450C0
    static void sub_4450C0()
    {
        using sig = void (*)();
        auto p = (sig)0x004450C0;
        p();
    }

    // 0x0043DF40
    static void sub_43DF40()
    {
        using sig = void (*)();
        auto p = (sig)0x0043DF40;
        p();
    }

    // TODO: Where this go???
    // 0x0043F550
    static int UnloadTexturePage(int page)
    {
        using sig = int (*)(int);
        auto p = (sig)0x0043F550;
        return p(page);
    }

    // TODO: move to player.h
    static uint8_t get_player_num()
    {
        return check_flag(FlagGroup::Status, FG_STATUS_PLAYER) ? 1 : 0;
    }

    // TODO: move to rdt.h
    static void get_rdt_path(char* buffer, uint8_t player, uint8_t stage, uint8_t room)
    {
        auto stageSym = _stageSymbols[(dword_98E798 & 0xFF) + gGameTable.current_stage];
        std::sprintf(buffer, "Pl%d\\Rdt\\room%c%02x%d.rdt", player, stageSym, room, player);
    }

    // 0x004DE7B0
    static void set_room()
    {
        while (true)
        {
            switch (dword_68A204->var_0D)
            {
            case 0:
            {
                auto buffer2 = (char*)0x00689C40;
                switch (gGameTable.graphicsPtr)
                {
                case 0:
                case 2: std::strcpy(buffer2, "common\\data\\font1.adt"); break;
                case 1: std::strcpy(buffer2, "common\\data\\font1.tim"); break;
                default:
                    get_rdt_path(
                        _rdtPathBuffer,
                        get_player_num(),
                        (dword_98E798 & 0xFF) + gGameTable.current_stage,
                        gGameTable.current_room & 0xFF);

                    switch (gGameTable.current_stage)
                    {
                    case 0:
                        if (gGameTable.current_room == 22)
                        {
                            set_registry_flg(0, 56);
                        }
                        break;
                    case 1:
                        switch (gGameTable.current_room)
                        {
                        case 0: set_registry_flg(0, 48); break;
                        case 2: set_registry_flg(0, 49); break;
                        case 4: set_registry_flg(0, 51); break;
                        case 7: set_registry_flg(0, 50); break;
                        case 8: set_registry_flg(0, 53); break;
                        case 10: set_registry_flg(0, 52); break;
                        case 11: set_registry_flg(0, 55); goto LABEL_27;
                        case 27:
                        LABEL_27:
                            set_registry_flg(0, 54);
                            break;
                        default: goto LABEL_34;
                        }
                        break;
                    case 2:
                        if (gGameTable.current_room == 5)
                        {
                            set_registry_flg(0, 57);
                        }
                        break;
                    case 3:
                        if (gGameTable.current_room == 5)
                        {
                            set_registry_flg(0, 59);
                        }
                        break;
                    case 4:
                        if (gGameTable.current_room == 0)
                        {
                            set_registry_flg(0, 58);
                        }
                        break;
                    }
                }

                word_98E78C = 0;
                dword_989E68 = 0;
                gGameFlags &= 0xFFF04000;
                gGameTable.player_work->pOn_om = 0;
                gGameTable.player_work->status_flg &= 0xF9FF;
                _memTop = dword_988620;
                dword_98861C = dword_988620;
                dword_68A204->var_0D = 10;
                goto LABEL_35;
            }
            case 1:  
                goto LABEL_62;
            case 2:
                dword_68A204->var_0D = 3;
                byte_99270F = 0;
                task_sleep(1);
                break;
            case 3:
                if (gGameTable.current_stage == byte_989E7D)
                {
                    dword_68A204->var_0D = 5;
                }
                else
                {
                    // loc_4DEBC4
                }
                break;
            case 4:
            case 5:
                /* word_989EE8 = 3333; */
                /* Osp_cache(); */
                /* byte_689C64 = 1; */
                /* rdt_size = BufferizeFile(room_path, lpBuffer, 8u); */
                /* if (!rdt_size) */
                /*     goto LABEL_73; */
                word_989EE8 = 3333;
                read_osp();
                if (read_file_into_buffer(_rdtPathBuffer, byte_98861C, 8) == 0)
                {
                    file_error();
                }
                
                // RdtHeader
                /* v11 = lpBuffer; */
                // RdtOffset
                /* v12 = lpBuffer + 2; */

                auto rdt_p_top = rdt_get_offset<void*>(RdtOffsetKind::EDT);
                for (int i = 0; i < 23; i++) 
                {
                    /* gGameTable.rdt->offsets[i] = 0; */

                }
                /* rdt_p_top = (int)(lpBuffer + 2); */
                /* do */
                /* { */
                /*     if (*v12) */
                /*     { */
                /*         *v12 = (char*)*v12 + (_DWORD)v11; */
                /*         v11 = lpBuffer; */
                /*         v12 = (LPVOID*)rdt_p_top; */
                /*     } */
                /*     rdt_p_top = (int)++v12; */
                /* } while (v12 < v11 + 25); */

                /* v13 = 0; */
                /* rdt_nCount = 0; */
                if (gGameTable.rdt->header.num_cuts != 0)
                {

                }
                /* if (*((_BYTE*)v11 + 1)) */
                /* { */
                /*     do */
                /*     { */
                /*         v14 = (char*)v11[9] + 32 * v13 + 28; */
                /*         *v14 += v11; */
                /*         v11 = lpBuffer; */
                /*         v13 = rdt_nCount + 1; */
                /*         rdt_nCount = v13; */
                /*     } while (v13 < *((unsigned __int8*)lpBuffer + 1)); */
                /* } */
                /* v15 = 0; */
                /* Mem_top = (LPVOID*)((char*)Mem_top + rdt_size); */
                /* rdt_nCount = 0; */

                if (gGameTable.rdt->header.num_models != 0)
                {

                }
                /* if (*((_BYTE*)v11 + 2)) */
                /* { */
                /*     do */
                /*     { */
                /*         *((_DWORD*)v11[12] + 2 * v15) += v11; */
                /*         v16 = (char*)lpBuffer[12] + 8 * rdt_nCount + 4; */
                /*         *v16 += lpBuffer; */
                /*         v11 = lpBuffer; */
                /*         v15 = rdt_nCount + 1; */
                /*         rdt_nCount = v15; */
                /*     } while (v15 < *((unsigned __int8*)lpBuffer + 2)); */
                /* } */


                /* Cut_change(scd_var_cut); */
                /* Esp_init_R(); */
                /* *(_BYTE*)(pCtcb + 13) = 6; */
                cut_change(gGameTable.current_cut); 
                Esp_init_R();
                dword_68A204->var_13 = 6;
            LABEL_84:
                snd_room_load();
                if (dword_68A204->var_13 == 0)
                {
                    UnloadTexturePage(17);
                    sce_model_init();
                    snd_bgm_play_ck();
                    auto vb = rdt_get_offset<void*>(RdtOffsetKind::VB);
                    if (vb)
                    {
                        gGameTable.mem_top = vb;
                    }
                    // TODO: Check two following instructions
                    /* dword_988628 = &pPl; */
                    /* dword_988628 = &gGameTable.player_work; */
                    gGameTable.player_work->routine_0 = 0;
                    player_move(gGameTable.player_work);
                    sce_scheduler_set();
                    dword_68A204->var_13 = 7;
                }

            LABEL_88:
                snd_load_em();
                if (dword_68A204->var_13 == 0)
                {
                    rbj_set();
                    psp_init0();
                    dword_68A204->var_13 = 8;
                }
            LABEL_90:
                out();
                if (dword_68A204->var_13 == 0)
                {
                    // em_init_move();
                    psp_init1();
                    dword_68A204->var_13 = 9;

                LABEL_92:
                    if (byte_99270F)
                    {
                        task_sleep(1);
                    }
                    else
                    {
                        st_room_set();
                        flush_surfaces();
                        dword_68A204->var_13 = 0;
                    }
                }
                /* break; */
                return;
            case 10:
            LABEL_35:
                snd_bgm_set();
                if (dword_68A204->var_13 == 0)
                {
                    sub_4450C0();
                    dword_98862C = 0x0098A114;
                    byte_989EEB = 0;
                    for (auto i = 0; i < 33; i++)
                    {
                        dword_98A110[i] = dword_98E544;
                    }
                    _rdtnCount = 0;
                    word_98A616 = 0;
                    word_98A61A = 0;
                    sub_43DF40();
                    dword_98E51C = _om;
                    _rdtnCount = 32;
                    for (auto i = 0; i < 32; i++)
                    {
                        _om->be_flg = 0;
                    }
                    if (gGameTable.player_work->id == word_98EB24)
                    {
                        dword_68A204->var_0D = 2;
                    }
                    else
                    {
                        // loc_4DEA4E
                    }
                }
            LABEL_44:
                /* v0 = pCtcb; */
                /* v1 = *(unsigned __int8 *)(pCtcb + 13); */
                /* if ( v1 > 0xA ) */
                /*     return; */
                /* continue; */

            LABEL_60:

            LABEL_62:

            LABEL63:

                /* break; */
            /* default: return; */
            }
        }
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
