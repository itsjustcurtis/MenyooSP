/*
THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
http://dev-c.com
(C) Alexander Blade 2015
*/
/*
* ALTERED SOURCE
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*/
#include "main.h"

//#include "macros.h"

#include "Util\keyboard.h"
#include "Memory\GTAmemory.h"
#include "Util\FileLogger.h"
#include "Menu\Routine.h"
#include "Menu\Menu.h"
#include "Menu\MenuConfig.h"

#include <Windows.h>
#include <Psapi.h>
#include <sstream>

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		//g_MainModule = hInstance;
		g_MainModule = GetModuleHandle(NULL);
		MenuConfig::ConfigInit();

		if (!GetModuleInformation(GetCurrentProcess(), g_MainModule, &g_MainModuleInfo, sizeof(g_MainModuleInfo)))
			addlog(ige::LogType::LOG_INIT, "Unable to get MODULEINFO from GTA5.exe", __FILENAME__);
		else
		{
			std::ostringstream moduleinfostream;
			moduleinfostream << "MODULEINFO: lpBaseofDll=" << g_MainModuleInfo.lpBaseOfDll
				<< ", SizeOfImage=" << g_MainModuleInfo.SizeOfImage
				<< ", EntryPoint=" << g_MainModuleInfo.EntryPoint;

			addlog(ige::LogType::LOG_INFO, moduleinfostream.str(), __FILENAME__);
		}
		addlog(ige::LogType::LOG_TRACE, "Attempting to Setup Hooks", __FILENAME__);
		setupHooks();
		addlog(ige::LogType::LOG_TRACE, "Initialising GTAmemory", __FILENAME__);
		GTAmemory::Init();

		addlog(ige::LogType::LOG_TRACE, "Registering Script ThreadMenyooMain", __FILENAME__);
		scriptRegister(hInstance, ThreadMenyooMain);
		addlog(ige::LogType::LOG_TRACE, "Registering Script Thread_menu_loops2", __FILENAME__);
		scriptRegisterAdditionalThread(hInstance, Thread_menu_loops2);

		addlog(ige::LogType::LOG_TRACE, "Registering Keyboard Handler", __FILENAME__);
		keyboardHandlerRegister(OnKeyboardMessage);
		addlog(ige::LogType::LOG_TRACE, "Finished Registering scripts", __FILENAME__);

		break;

	case DLL_PROCESS_DETACH:
		scriptUnregister(hInstance);
		keyboardHandlerUnregister(OnKeyboardMessage);
		
		break;
	}
	return TRUE;
}

