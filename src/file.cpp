#define USE_ORIGINAL_FILEIO

#include "file.h"
#include "openre.h"
#include <iostream.h>
#include <windows.h>

namespace openre::file
{
    // 0x00508DC0
    void file_error()
    {
        using sig = void (*)();
        auto p = (sig)0x00508DC0;
        p();
    }

    // 0x00509020
    static HANDLE file_open_handle(const char* path, int a1)
    {
        using sig = HANDLE (*)(const char*, int);
        auto p = (sig)0x00509020;
        return p(path, a1);
    }

    // 0x00502D40
    size_t read_file_into_buffer(const char* path, void* buffer, size_t length)
    {
#ifdef USE_ORIGINAL_FILEIO
        using sig = uint32_t (*)(const char*, void*, size_t);
        auto p = (sig)0x00502D40;
        return p(path, buffer, length);
#else
        size_t result = 0;
        auto hFile = sub_509020(path, length);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            auto fileSize = GetFileSize(hFile, NULL);
            DWORD bytesRead = 0;
            if (ReadFile(hFile, buffer, fileSize, &bytesRead, nullptr) && bytesRead == fileSize)
            {
                result = bytesRead;
            }
            CloseHandle(hFile);
        }
        if (result == 0)
        {
            gErrorCode = 11;
        }
        return result;
#endif
    }

    // 0x00509540
    uint32_t read_partial_file_into_buffer(const char* path, void* buffer, size_t offset, size_t length, size_t unk)
    {
        using sig = uint32_t (*)(const char*, void*, size_t, size_t, size_t);
        auto p = (sig)0x00509540;
        return p(path, buffer, offset, length, unk);
    }

    // 0x0043C590
    int load_adt(const char* path, void* buffer, int mode)
    {
        std::cout << "load_adt" << std::endl;
        gGameTable.dword_99DAC8 = 0;
        if (gGameTable.error_no == 11)
        {
            return 0;
        }
        gGameTable.dword_671404 = 0;
        gGameTable.dword_671408 = 0;
        gGameTable.dword_67140C = 0;
        gGameTable.dword_671410 = 0;
        gGameTable.dword_671414 = 0;
        gGameTable.dword_671418 = 0;
        gGameTable.dword_524E08 = 8;
        gGameTable.dword_524E0C = 0x4000;
        gGameTable.dword_99DAB4 = 0;
        gGameTable.dword_99DAB8 = 0;
        gGameTable.dword_99DAB0 = 0;
        /* gGameTable.dword_99DAA8 = (int)pBuffer; */
        return 0;
    }

    void file_init_hooks()
    {
        interop::writeJmp(0x004E5020, &sub_4E5020);
    }
}
