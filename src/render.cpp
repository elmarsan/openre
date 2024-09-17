#include "render.h"
#include "interop.hpp"
#include "openre.h"
#include <iostream>

namespace openre::render
{
    // 0x00441A00
    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        return interop::call<LRESULT, HWND, UINT, WPARAM, LPARAM>(0x00441A00, hWnd, msg, wParam, lParam);
    }

    // 0x00441DC0
    int init_instance(HINSTANCE hInstance, int hPrevInstance)
    {
        std::cout << "init_instance\n";
        gGameTable.h_instance = hInstance;

        if (!hPrevInstance)
        {
            WNDCLASSA wndClass{};
            wndClass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
            wndClass.lpfnWndProc = WndProc;
            wndClass.cbClsExtra = 0;
            wndClass.cbWndExtra = 0;
            wndClass.hInstance = hInstance;
            wndClass.hIcon = LoadIconA(hInstance, (LPCSTR)0xa3);
            wndClass.hCursor = LoadCursorA(0, (LPCSTR)0x7f00);
            wndClass.hbrBackground = (HBRUSH)GetStockObject(4);
            wndClass.lpszMenuName = 0;
            wndClass.lpszClassName = gGameTable.caption;
            RegisterClassA(&wndClass);
        }

        DWORD winStyle = 0x2ac0000;

        RECT rect;
        rect.left = 0;
        rect.right = 640;
        rect.top = 0;
        rect.bottom = 480;
        AdjustWindowRect(&rect, winStyle, 0);

        gGameTable.h_wnd = CreateWindowExA(
            0,                      // dwExStyle
            gGameTable.caption,     // lpClassName
            gGameTable.caption,     // lpWindowName
            winStyle,               // dwStyle
            0x8000000,              // X
            0x8000000,              // Y
            rect.right - rect.left, // nWidth
            rect.bottom - rect.top, // nHeight
            0,                      // hWndParent
            0,                      // hMenu
            hInstance,              // hInstance
            0                       // lpParam
        );

        auto hWnd = reinterpret_cast<HWND>(gGameTable.h_wnd);
        ShowWindow(hWnd, SW_NORMAL);
        SetForegroundWindow(hWnd);
        UpdateWindow(hWnd);
        return 1;
    }

    void render_init_hooks()
    {
        interop::writeJmp(0x00441DC0, &init_instance);
    }
};