#pragma once

#include "re2.h"

namespace openre::rdt
{
    enum class RdtOffsetKind
    {
        EDT,            // 0
        VH,             // 1
        VB,             // 2
        VH_TRIAL,       // 3
        VB_TRIAL,       // 4
        OVA,            // 5
        SCA,            // 6
        RID,            // 7
        RVD,            // 8
        LIT,            // 9
        MODELS,         // 10
        FLR,            // 11
        BLK,            // 12
        MSG_JA,         // 13
        MSG_EN,         // 14
        SCROLL,         // 15
        SCD_INIT,       // 16
        SCD_MAIN,       // 17
        ESP_IDS,        // 18
        ESP_EFF_TABLE,  // 19
        EFF,            // 20
        MODEL_TEXTURES, // 21
        RBJ,            // 22
    };

    template<typename T> T* rdt_get_offset(RdtOffsetKind kind)
    {
        return static_cast<T*>(rdt_get_offset<void>(kind));
    }

    template<> void* rdt_get_offset(RdtOffsetKind kind);
}
