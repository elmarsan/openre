#include "marni.h"
#include "interop.hpp"
#include "re2.h"

#include <ddraw.h>
#include <windows.h>

namespace openre::marni
{
    // 0x00443620
    void mapping_tmd(int workNo, Md1* pTmd, int id)
    {
        interop::call<void, int, Md1*, int>(0x00443620, workNo, pTmd, id);
    }

    // 0x004DBFD0
    void out() {}

    // 0x00432BB0
    void unload_door_texture()
    {
        interop::call(0x00432BB0);
    }

    // 0x00442E40
    bool sub_442E40()
    {
        using sig = bool (*)();
        auto p = (sig)0x00442E40;
        return p();
    }

    // 0x0043F550
    void unload_texture_page(int page)
    {
        interop::call<void, int>(0x0043F550, page);
    }

    // 0x00432C60
    void door_disp0(int doorId, int a1, int a2, int a3)
    {
        interop::call<void, int, int, int>(0x00432C60, doorId, a1, a2, a3);
    }

    // 0x00432CD0
    void door_disp1(int doorId)
    {
        interop::call<void, int>(0x00432CD0, doorId);
    }

    // 0x00404FA0
    static void clear_buffers(Marni* marni)
    {
        interop::call<void, Marni*>(0x00404FA0, marni);
    }

    // 0x00403F30
    static int init_all(Marni* marni)
    {
        return interop::call<int, Marni*>(0x00403F30, marni);
    }

    // 0x00402940
    static int restore_surfaces(Marni* marni)
    {
        return interop::call<int, Marni*>(0x00402940, marni);
    }

    // 0x00406450
    static void marni_move(Marni* marni)
    {
        HWND window = (HWND)marni->hWnd;
        RECT rect = {};
        rect.left = marni->window_rect[0];
        rect.top = marni->window_rect[1];
        rect.right = marni->window_rect[2];
        rect.bottom = marni->window_rect[3];

        POINT point0 = {};
        ClientToScreen(window, &point0);
        POINT point1 = {};
        point1.x = marni->resolutions[marni->modes].width;
        point1.y = marni->resolutions[marni->modes].height;
        ClientToScreen(window, &point1);

        SetRect(&rect, point0.x, point0.y, point1.x, point1.y);
        marni->window_rect[0] = rect.left;
        marni->window_rect[1] = rect.top;
        marni->window_rect[2] = rect.right;
        marni->window_rect[3] = rect.bottom;
    }

    // 0x00403220
    static int marni_change_mode(Marni* marni, uint32_t width, uint32_t height, int fullscreen)
    {
        if ((marni->gpu_flag & 0x200) == 0)
        {
            return 0;
        }
        if (marni->gpu_flag & 0x2000)
        {
            marni->aspect_x = 1;
            marni->aspect_y = 1;
        }
        else
        {
            marni->aspect_x = width / marni->render_width;
            marni->aspect_y = height / marni->render_height;
        }
        marni->size_x_old = marni->size_x;
        marni->size_y_old = marni->size_y;
        marni->is_fullscreen_old = marni->bbp;
        marni->field_8C7EC0 = (marni->gpu_flag >> 10) & 1;
        marni->size_x = width;
        marni->size_y = height;
        marni->bbp = fullscreen;
        clear_buffers(marni);
        if (!init_all(marni))
        {
            out();
            out();
            marni->size_x = marni->size_x_old;
            marni->size_y = marni->size_y_old;
            marni->bbp = marni->is_fullscreen_old;
            out();
            clear_buffers(marni);
            if (!init_all(marni))
            {
                out();
                clear_buffers(marni);
                return 0;
            }
        }

        if (!restore_surfaces(marni))
        {
            out();
            out();
            marni->size_x = marni->size_x_old;
            marni->size_y = marni->size_y_old;
            marni->bbp = marni->is_fullscreen_old;
            out();
            clear_buffers(marni);
            if (!init_all(marni) || restore_surfaces(marni))
            {
                out();
                clear_buffers(marni);
                return 0;
            }
        }

        marni->gpu_flag |= 2;
        return 1;
    }

    void marni_init_hooks()
    {
        interop::hookThisCall(0x00406450, &marni_move);
        interop::hookThisCall(0x00403220, &marni_change_mode);
    }
}
