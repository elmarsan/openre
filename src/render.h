#pragma once

#include <windows.h>

namespace openre::render
{
    int init_instance(HINSTANCE hInstance, int hPrevInstance);

    void render_init_hooks();
};