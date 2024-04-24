#pragma once

#include <cstdint>

struct Vec32;

namespace openre::audio
{
    void bgm_set_entry(uint32_t arg0);
    void bgm_set_control(uint32_t arg0);
    void snd_se_on(int a0);
    void snd_se_on(int a0, const Vec32& a1);
    void snd_bgm_set();
    void snd_load_core(uint8_t id, uint8_t a1);
    void snd_load_em();
    void bgm_init_hooks();
}
