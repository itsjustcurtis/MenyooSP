#pragma once

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <mutex>
#include <Windows.h>
#include <Psapi.h>

typedef int64_t  s64;
typedef int32_t  s32;
typedef int16_t  s16;
typedef int8_t   s8;
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;
typedef void* pVoid;

struct gmAddress
{
    u64 Value = 0;

    gmAddress(u64 value) : Value(value) {}
    gmAddress() : gmAddress(0) {}

    static void GetModuleBaseAndSize(u64* lpBase, u64* lpSize)
    {
        MODULEINFO moduleInfo = {};
        HMODULE module = GetModuleHandleA(NULL);
        GetModuleInformation(GetCurrentProcess(), module, &moduleInfo, sizeof(MODULEINFO));

        if (lpBase) *lpBase = (u64)(moduleInfo.lpBaseOfDll);
        if (lpSize) *lpSize = (u64)(moduleInfo.SizeOfImage);
    }

    static gmAddress Scan(const char* patternStr, const char* debugName = nullptr)
    {
        static u64 moduleSize;
        static u64 moduleBase;
        static bool initialized = false;
        static std::mutex initMutex;

        if (!initialized && initMutex.try_lock())
        {
            GetModuleBaseAndSize(&moduleBase, &moduleSize);
            initialized = true;
            initMutex.unlock();
        }

        s16 pattern[256];
        u8 patternSize = 0;
        for (size_t i = 0; i < strlen(patternStr); i += 3)
        {
            const char* cursor = patternStr + i;

            if (cursor[0] == '?')
                pattern[patternSize] = -1;
            else
                pattern[patternSize] = static_cast<s16>(strtol(cursor, nullptr, 16));

            if (cursor[1] == ' ')
                i--;

            patternSize++;
        }

        u8 scanSize = patternSize;
        if (scanSize % 2 == 0)
            scanSize /= 2;
        else
            scanSize = patternSize / 2 + 1;

        for (u64 i = 0; i < moduleSize; i += 1)
        {
            const u8* modulePos = (u8*)(moduleBase + i);
            for (u8 j = 0; j < scanSize; j++)
            {
                s16 lExpected = pattern[j];
                s16 lActual = modulePos[j];

                if (lExpected != -1 && lActual != lExpected) goto miss;

                s16 rExpected = pattern[patternSize - j - 1];
                s16 rActual = modulePos[patternSize - j - 1];

                if (rExpected != -1 && rActual != rExpected) goto miss;
            }
            return { moduleBase + i };
        miss:;
        }

        return 0;
    }

    gmAddress GetAt(s32 offset) const { return Value != 0 ? Value + offset : 0; }
    gmAddress GetRef(s32 offset = 0) const { return Value != 0 ? Value + offset + 4 + *(s32*)(Value + offset) : 0; }
    gmAddress GetCall() const { return GetRef(1); }

    template<typename T>
    T To() const
    {
        return reinterpret_cast<T>(Value);
    }

    template<typename T>
    T* ToFunc() const { return To<T*>(); }

    gmAddress& operator=(u64 value) { Value = value; return *this; }
    operator u64() const { return Value; }
    operator pVoid() const { return (pVoid)Value; }
    operator bool() const { return Value != 0; }
};
