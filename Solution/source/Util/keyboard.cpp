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
#include "keyboard.h"

#include <Windows.h>

#define NOW_PERIOD 100
#define MAX_DOWN 5000 // ms
#define KEYS_SIZE 255


bool get_key_pressed(DWORD key)
{
	return (GetAsyncKeyState(key) & 0x8000) != 0;
}


struct {
	DWORD time;
	BOOL isWithAlt;
	BOOL wasDownBefore;
	BOOL isUpNow;
} keyStates[KEYS_SIZE];


void OnKeyboardMessage(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow)
{
	if (key < KEYS_SIZE)
	{
		keyStates[key].time = GetTickCount();
		keyStates[key].isWithAlt = isWithAlt;
		keyStates[key].wasDownBefore = wasDownBefore;
		keyStates[key].isUpNow = isUpNow;
	}
}


bool IsKeyDown(DWORD key)
{
	return (key < KEYS_SIZE) ? ((GetTickCount() < keyStates[key].time + MAX_DOWN) && !keyStates[key].isUpNow) : false;
}

bool IsKeyJustUp(DWORD key, bool exclusive)
{
	bool b = (key < KEYS_SIZE) ? (GetTickCount() < keyStates[key].time + NOW_PERIOD && keyStates[key].isUpNow) : false;
	if (b && exclusive)
		ResetKeyState(key);
	return b;
}



void ResetKeyState(DWORD key)
{
	if (key < KEYS_SIZE)
		memset(&keyStates[key], 0, sizeof(keyStates[0]));
}


// Game
std::string VkCodeToStr(UINT8 key)
{
	// I can just use GetKeyNameTextA ?
	switch (key)
	{
	default: return ""; break;
	case VirtualKey::Back:  return ("Backspace"); break;
	case VirtualKey::Tab:  return ("Tab"); break;
	case VirtualKey::Return:  return ("Enter"); break;
	case VirtualKey::Shift:  return ("Shift"); break;
	case VirtualKey::Control:  return ("Ctrl"); break;
	case VirtualKey::Menu:  return ("Alt"); break;
	case VirtualKey::Pause:  return ("Pause"); break;
	case VirtualKey::CapsLock:  return ("Caps Lock"); break;
	case VirtualKey::NumLock:  return ("Num Lock"); break;
	case VirtualKey::ScrollLock:  return ("Scroll Lock"); break;
	case VirtualKey::Prior:  return ("PageUp"); break;
	case VirtualKey::Next:  return ("PageDown"); break;
	case VirtualKey::Snapshot:  return ("PrintScreen"); break;
	case VirtualKey::Insert:  return ("Insert"); break;
	case VirtualKey::Help:  return ("Help"); break;
	case VirtualKey::N0:  return ("0"); break;
	case VirtualKey::N1:  return ("1"); break;
	case VirtualKey::N2:  return ("2"); break;
	case VirtualKey::N3:  return ("3"); break;
	case VirtualKey::N4:  return ("4"); break;
	case VirtualKey::N5:  return ("5"); break;
	case VirtualKey::N6:  return ("6"); break;
	case VirtualKey::N7:  return ("7"); break;
	case VirtualKey::N8:  return ("8"); break;
	case VirtualKey::N9:  return ("9"); break;
	case VirtualKey::A:  return ("A"); break;
	case VirtualKey::B: return ("B"); break;
	case VirtualKey::C:  return ("C"); break;
	case VirtualKey::D:  return ("D"); break;
	case VirtualKey::E:  return ("E"); break;
	case VirtualKey::F:  return ("F"); break;
	case VirtualKey::G:  return ("G"); break;
	case VirtualKey::H:  return ("H"); break;
	case VirtualKey::I:  return ("I"); break;
	case VirtualKey::J:  return ("J"); break;
	case VirtualKey::K:  return ("K"); break;
	case VirtualKey::L:  return ("L"); break;
	case VirtualKey::M:  return ("M"); break;
	case VirtualKey::N:  return ("N"); break;
	case VirtualKey::O:  return ("O"); break;
	case VirtualKey::P:  return ("P"); break;
	case VirtualKey::Q:  return ("Q"); break;
	case VirtualKey::R:  return ("R"); break;
	case VirtualKey::S:  return ("S"); break;
	case VirtualKey::T:  return ("T"); break;
	case VirtualKey::U:  return ("U"); break;
	case VirtualKey::V:  return ("V"); break;
	case VirtualKey::W:  return ("W"); break;
	case VirtualKey::X:  return ("X"); break;
	case VirtualKey::Y:  return ("Y"); break;
	case VirtualKey::Z:  return ("Z"); break;
	case VirtualKey::LeftWindows:  return ("Windows"); break;
	case VirtualKey::RightWindows:  return ("Windows"); break;
	case VirtualKey::Application:  return ("Menu"); break;
	case VirtualKey::Numpad0:  return ("Numpad 0"); break;
	case VirtualKey::Numpad1:  return ("Numpad 1"); break;
	case VirtualKey::Numpad2:  return ("Numpad 2"); break;
	case VirtualKey::Numpad3:  return ("Numpad 3"); break;
	case VirtualKey::Numpad4:  return ("Numpad 4"); break;
	case VirtualKey::Numpad5:  return ("Numpad 5"); break;
	case VirtualKey::Numpad6:  return ("Numpad 6"); break;
	case VirtualKey::Numpad7:  return ("Numpad 7"); break;
	case VirtualKey::Numpad8:  return ("Numpad 8"); break;
	case VirtualKey::Numpad9:  return ("Numpad 9"); break;
	case VirtualKey::Multiply:  return ("Numpad *"); break;
	case VirtualKey::Add:  return ("Numpad +"); break;
	case VirtualKey::Separator:  return ("Numpad ,"); break;
	case VirtualKey::Subtract:  return ("Numpad -"); break;
	case VirtualKey::Decimal:  return ("Numpad ."); break;
	case VirtualKey::Divide:  return ("Numpad /"); break;
	case VirtualKey::Up:  return ("Up"); break;
	case VirtualKey::Down:  return ("Down"); break;
	case VirtualKey::Left:  return ("Left"); break;
	case VirtualKey::Right:  return ("Right"); break;
	case VirtualKey::Delete:  return ("Delete"); break;
	case VirtualKey::End:  return ("End"); break;
	case VirtualKey::Home:  return ("Home"); break;
	case VirtualKey::Escape:  return ("ESC"); break;
	case VirtualKey::F1:  return ("F1"); break;
	case VirtualKey::F2:  return ("F2"); break;
	case VirtualKey::F3:  return ("F3"); break;
	case VirtualKey::F4:  return ("F4"); break;
	case VirtualKey::F5:  return ("F5"); break;
	case VirtualKey::F6:  return ("F6"); break;
	case VirtualKey::F7:  return ("F7"); break;
	case VirtualKey::F8:  return ("F8"); break;
	case VirtualKey::F9:  return ("F9"); break;
	case VirtualKey::F10:  return ("F10"); break;
	case VirtualKey::F11:  return ("F11"); break;
	case VirtualKey::F12:  return ("F12"); break;
	case VirtualKey::F13:  return ("F13"); break;
	case VirtualKey::F14:  return ("F14"); break;
	case VirtualKey::F15:  return ("F15"); break;
	case VirtualKey::F16:  return ("F16"); break;
	case VirtualKey::F17:  return ("F17"); break;
	case VirtualKey::F18:  return ("F18"); break;
	case VirtualKey::F19:  return ("F19"); break;
	case VirtualKey::F20:  return ("F20"); break;
	case VirtualKey::F21:  return ("F21"); break;
	case VirtualKey::F22:  return ("F22"); break;
	case VirtualKey::F23:  return ("F23"); break;
	case VirtualKey::F24:  return ("F24"); break;
	case VirtualKey::LeftShift:  return ("Left Shift"); break;
	case VirtualKey::RightShift:  return ("Right Shift"); break;
	case VirtualKey::LeftControl:  return ("Left Ctrl"); break;
	case VirtualKey::RightControl:  return ("Right Ctrl"); break;
	case VirtualKey::LeftMenu:  return ("Left Alt"); break;
	case VirtualKey::RightMenu:  return ("Right Alt"); break;
	case VirtualKey::Space:  return ("Space"); break;
	case VirtualKey::OEM1:  return (";:"); break;
	case VirtualKey::OEMPlus:  return ("="); break;
	case VirtualKey::OEMComma:  return (","); break;
	case VirtualKey::OEMMinus:  return ("-"); break;
	case VirtualKey::OEMPeriod:  return ("."); break;
	case VirtualKey::OEM2:  return ("/?"); break;
	case VirtualKey::OEM3:  return ("`~"); break;
	case VirtualKey::OEM4:  return ("[{"); break;
	case VirtualKey::OEM5:  return ("\\|"); break;
	case VirtualKey::OEM6:  return ("]}"); break;
	case VirtualKey::OEM7:  return ("'"); break;
	case VirtualKey::OEM8:  return ("`"); break;
	case VirtualKey::OEM102:  return ("\\|"); break;
	}
}


