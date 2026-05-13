#pragma once

#include "MinHook.h"

class Hook
{
public:
    static void Init()
    {
        MH_Initialize();
    }

    static void Shutdown()
    {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_RemoveHook(MH_ALL_HOOKS);
        MH_Uninitialize();
    }

    static void Seal()
    {
        MH_EnableHook(MH_ALL_HOOKS);
    }

    template<typename TFrom, typename TTo, typename TBackup>
    static bool Create(TFrom from, TTo to, TBackup* backup, const char* debugName = nullptr)
    {
        if (MH_CreateHook((LPVOID)from, (LPVOID)to, (LPVOID*)backup) != MH_OK)
            return false;
        return MH_EnableHook((LPVOID)from) == MH_OK;
    }

    template<typename TFrom, typename TTo>
    static bool Create(TFrom from, TTo to, const char* debugName = nullptr)
    {
        if (MH_CreateHook((LPVOID)from, (LPVOID)to, nullptr) != MH_OK)
            return false;
        return MH_EnableHook((LPVOID)from) == MH_OK;
    }

    template<typename TFrom>
    static void Remove(TFrom from, const char* debugName = nullptr)
    {
        if ((LPVOID)from == nullptr)
            return;
        MH_DisableHook((LPVOID)from);
        MH_RemoveHook((LPVOID)from);
    }
};
