// Minimal shim of MinHook API used by d3dhook/Hook.h
#pragma once

#include <windows.h>

typedef int MH_STATUS;
#define MH_OK 0
#define MH_ALL_HOOKS ((LPVOID)~(uintptr_t)0)

#ifdef __cplusplus
extern "C" {
#endif

MH_STATUS MH_Initialize(void);
MH_STATUS MH_CreateHook(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal);
MH_STATUS MH_EnableHook(LPVOID pTarget);
MH_STATUS MH_DisableHook(LPVOID pTarget);
MH_STATUS MH_RemoveHook(LPVOID pTarget);
MH_STATUS MH_Uninitialize(void);

#ifdef __cplusplus
}
#endif
