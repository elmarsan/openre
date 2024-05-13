#define USE_ORIGINAL_FILEIO

#include "file.h"
#include "openre.h"
#include <windows.h>

#include <iostream>

using namespace openre;

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
    static HANDLE file_open_handle(const char* path, int mode)
    {
        using sig = HANDLE (*)(const char*, int);
        auto p = (sig)0x00509020;
        return p(path, mode);
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

    // 0x00441670
    static void* alloc0(const size_t size)
    {
        using sig = void* (*)(const size_t);
        auto p = (sig)0x00441670;
        return p(size);
    }

    // 0x004416B0
    static void* alloc1(const size_t size)
    {
        using sig = void* (*)(const size_t);
        auto p = (sig)0x004416B0;
        return p(size);
    }

    // 0x00509620
    static void* sub_509620(const char* path)
    {
        using sig = void* (*)(const char*);
        auto p = (sig)0x00509620;
        return p(path);
    }

    // 0x0043C390
    static int decompress_adt()
    {
        using sig = int (*)();
        auto p = (sig)0x0043C390;
        return p();
    }

    // 0x0043C590
    int load_adt(const char* path, void* buffer, int mode)
    {
        std::cout << "load_adt" << std::endl;
        gGameTable.dword_99DAC8 = 0;
        if (gErrorCode == 11)
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
        gGameTable.dword_99DAA8 = *reinterpret_cast<uint32_t*>(buffer);
        gGameTable.lpBuffer0 = alloc0(0x8000);
        gGameTable.lpBuffer1 = alloc1(0x8000);
        auto hFile = file_open_handle(path, mode);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            auto fileSize = GetFileSize(hFile, NULL);
            char buff[4];

            if (!ReadFile(hFile, buff, 4, nullptr, nullptr))
            {
                alloc1(0);
                alloc0(0);
                CloseHandle(hFile);
                sub_509620(path);
                gErrorCode = 11;
                return 0;
            }
            if (!ReadFile(hFile, gGameTable.lpBuffer0, 0x8000, reinterpret_cast<LPDWORD>(gGameTable.num_bytes_to_read), nullptr))
            {
                alloc1(0);
                alloc0(0);
                CloseHandle(hFile);
                sub_509620(path);
                gErrorCode = 11;
                return 0;
            }
            decompress_adt();
            alloc1(0);
            alloc0(0);
            update_timer();
            CloseHandle(hFile);
            return gGameTable.dword_99DAB4;
        }
        alloc1(0);
        alloc0(0);
        sub_509620(path);
        gErrorCode = 11;
        return 0;
    }

    void file_init_hooks()
    {
        std::cout << "file_init_hooks" << std::endl;
        /* interop::writeJmp(0x004E5020, &sub_4E5020); */
    }
}
