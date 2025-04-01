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

    // 0x004065C0
    static void marni_resize(Marni* marni, HWND window, uint32_t message, WPARAM wParam, LPARAM lParam)
    {
        interop::call<void, Marni*, HWND, uint32_t, WPARAM, LPARAM>(0x004065C0, marni, window, message, wParam, lParam);
    }

    // 0x004064D0
    static void marni_destroy(Marni* marni)
    {
        interop::call<void, Marni*>(0x004064D0, marni);
    }

    // 0x00401F10
    static void marni_syskeydown(Marni* marni)
    {
        interop::call<void, Marni*>(0x00401F10, marni);
    }

    // 0x004063D0
    static int marni_message(Marni* marni, HWND window, uint32_t message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_SIZE:
        {
            if (!marni->is_gpu_busy)
            {
                marni_resize(marni, window, message, wParam, lParam);
            }
            break;
        }
        case WM_DESTROY:
        {
            marni_destroy(marni);
            break;
        }
        case WM_MOVE:
        {
            marni_move(marni);
            break;
        }
        case WM_SYSKEYDOWN:
        {
            if (marni->gpu_flag & 0x4000)
            {
                marni_syskeydown(marni);
            }
            break;
        }
        }

        return 1;
    }

    void marni_init_hooks()
    {
        interop::hookThisCall(0x00406450, &marni_move);
        interop::hookThisCall(0x004063D0, &marni_message);
    }
}
