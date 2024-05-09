#pragma once

#include "re2.h"

namespace openre::rdt
{
    enum RdtOffsetKind
    {
        RDT_OFFSET_KIND_EDT,            // 0
        RDT_OFFSET_KIND_VH,             // 1
        RDT_OFFSET_KIND_VB,             // 2
        RDT_OFFSET_KIND_VH_TRIAL,       // 3
        RDT_OFFSET_KIND_VB_TRIAL,       // 4
        RDT_OFFSET_KIND_OVA,            // 5
        RDT_OFFSET_KIND_SCA,            // 6
        RDT_OFFSET_KIND_RID,            // 7
        RDT_OFFSET_KIND_RVD,            // 8
        RDT_OFFSET_KIND_LIT,            // 9
        RDT_OFFSET_KIND_MODELS,         // 10
        RDT_OFFSET_KIND_FLR,            // 11
        RDT_OFFSET_KIND_BLK,            // 12
        RDT_OFFSET_KIND_MSG_JA,         // 13
        RDT_OFFSET_KIND_MSG_EN,         // 14
        RDT_OFFSET_KIND_SCROLL,         // 15
        RDT_OFFSET_KIND_SCD_INIT,       // 16
        RDT_OFFSET_KIND_SCD_MAIN,       // 17
        RDT_OFFSET_KIND_ESP_IDS,        // 18
        RDT_OFFSET_KIND_ESP_EFF_TABLE,  // 19
        RDT_OFFSET_KIND_EFF,            // 20
        RDT_OFFSET_KIND_MODEL_TEXTURES, // 21
        RDT_OFFSET_KIND_RBJ,            // 22
    };

    template<typename T> T* rdt_get_offset(int kind)
    {
        return static_cast<T*>(rdt_get_offset<void>(kind));
    }

    template<> void* rdt_get_offset(int kind);
}
