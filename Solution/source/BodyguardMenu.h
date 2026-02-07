#pragma once

#include <string>
#include "Menu/Routine.h"

using TargetPed = int;

TargetPed g_WeaponMenuPedOverride = 0;
namespace sub
{ 
void BodyguardMainMenu();
}
namespace sub::BodyguardMenu
{
    extern int armor;
    extern int health;
    extern bool godmode;

}
