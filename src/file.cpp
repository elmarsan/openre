#define USE_ORIGINAL_FILEIO

#include "file.h"
#include "interop.hpp"
#include "openre.h"

#include <iostream>
#include <strsafe.h>
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

    void PrintLastError()
    {
        DWORD errorMessageID = GetLastError();
        if (errorMessageID == 0)
            return; // No error message has been recorded

        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            errorMessageID,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPSTR>(&messageBuffer),
            0,
            nullptr);
        std::cout << "Error: " << messageBuffer << std::endl;

        LocalFree(messageBuffer);
    }

    // 0x0043C590
    /* static int load_adt_file(const char* path, void* buffer, int mode) */
    /* { */
    /*     std::cout << "Let's break it\n"; */
    /*     return 0; */
    /* } */

    // 0x0043C590
    static int load_adt_file(const char* path, void* pBuffer, int mode)
    {
        std::cout << "load_adt_file: " << path << std::endl;
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
        gGameTable.dword_99DAA8 = *reinterpret_cast<uint32_t*>(pBuffer);
        gGameTable.lpBuffer0 = alloc0(0x8000);
        auto resAlloc = reinterpret_cast<uint32_t*>(alloc1(0x8000));
        std::cout << "Alloc1: " << std::hex << resAlloc << std::endl;
        std::cout << "Alloc1: " << std::hex << &resAlloc << std::endl;

        std::cout << gGameTable.lpBuffer1 << std::endl;

        HANDLE hFile = file_open_handle(path, mode);
        /* gGameTable.hObject = &hFile; */

        std::cout << "Handle opened and assigned to global hFile\n";
        if (hFile == INVALID_HANDLE_VALUE)
        {
            std::cout << "INVALID_HANDLE_VALUE" << std::endl;
        }

        /* ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED
         * lpOverlapped) */
        char Buffer[4];
        auto res = ReadFile(hFile, Buffer, 4, LPDWORD(&pBuffer), nullptr);
        std::cout << "Header size: " << res << std::endl;
        if (!res)
        {
            std::cout << "** reading header\n";
            /* CloseHandle(gGameTable.hObject); */
            return 0;
        }

        std::cout << "Header\n";
        
        /* if (!ReadFile(gGameTable.hObject, gGameTable.lpBuffer0, 0x8000, &bytesRead, nullptr) && bytesRead == 0x8000) */

        /* DWORD bytesRead = gGameTable.num_bytes_to_read; */
        auto fileSize = GetFileSize(hFile, NULL);
        DWORD bytesRead = 0;
        auto res2 = ReadFile(hFile, gGameTable.lpBuffer0, 0x8000, &bytesRead, nullptr);
        gGameTable.num_bytes_to_read = bytesRead;
        std::cout << "ADT size: " << bytesRead << std::endl;
        if (!res2) 
        {
            std::cout << "** reading ADT\n";
            return 0;
        }

        std::cout << "ADT\n";
        decompress_adt();
        alloc1(0);
        alloc0(0);

        if (!hFile) 
        {
            std::cout << "La putaaaaaaaaaaaaaaaaaaaaaa\n";
        }

        /* CloseHandle(hFile); */

        std::cout << "Decompress\n";

        update_timer();
        return gGameTable.dword_99DAB4;
    }
    /*     gGameTable.hObject = hFile; */

    /*     if (hFile == INVALID_HANDLE_VALUE) */
    /*     { */
    /*         /1* goto LABEL_7; *1/ */
    /*         std::cout << "Chaoooos\n"; */
    /*     } */

    /*     /1* if (ReadFile(hFile, buffer, fileSize, &bytesRead, nullptr) && bytesRead == fileSize) *1/ */
    /*     DWORD bytesRead = 0; */
    /*     char Buffer[4]; */
    /*     std::cout << std::hex << pBuffer << std::endl; */
    /*     if (!ReadFile(hFile, Buffer, 4, &bytesRead, nullptr) && bytesRead == 4) */
    /*     { */
    /*         CloseHandle(gGameTable.hObject); */
    /*         goto LABEL_7; */
    /*     } */
    /*     std::cout << "check\n"; */
    /*     if (!ReadFile(gGameTable.hObject, gGameTable.lpBuffer0, 0x8000, &bytesRead, nullptr) && bytesRead == 0x8000) */
    /*     { */
    /*     LABEL_7: */
    /*         alloc1(0); */
    /*         alloc1(1); */
    /*         if (gGameTable.hObject != INVALID_HANDLE_VALUE) */
    /*         { */
    /*             CloseHandle(gGameTable.hObject); */
    /*         } */
    /*         goto LABEL_10; */
    /*     } */
    /*     std::cout << "2 check\n"; */
    /*     decompress_adt(); */
    /*     std::cout << "3 check\n"; */
    /*     alloc1(0); */
    /*     alloc0(1); */
    /*     if (!gGameTable.hObject) */
    /*     { */
    /*     LABEL_10: */
    /*         sub_509620(path); */
    /*         gErrorCode = 11; */
    /*         return 0; */
    /*     } */
    /*     std::cout << "4 check\n"; */
    /*     CloseHandle(gGameTable.hObject); */
    /*     update_timer(); */
    /*     return gGameTable.dword_99DAB4; */
    /* } */

    void file_init_hooks()
    {
        std::cout << "file_init_hooks" << std::endl;
        /* interop::writeJmp(0x004E5020, &sub_4E5020); */

        /* interop::writeJmp(0x00502190, &partner_switch); */
        interop::writeJmp(0x0043C590, &load_adt_file);
    }
}
