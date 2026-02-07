#pragma once
#include <string>
#include "Scripting/Model.h"
#include "../source/Menu/Routine.h"
#include "Scripting/GTAped.h"

typedef int INT;
typedef signed char INT8;
typedef unsigned char UINT8;
typedef unsigned long DWORD;
typedef unsigned long Hash;

namespace GTAmodel {
    class Model;
}

namespace sub::BodyguardMenu
{
    extern int health;
    extern int armor;
    extern bool godmode;

    extern std::string _searchStr;

    void BodyguardSpawn();

    namespace BodyguardManagement
    {
        extern std::vector<Ped> s_bodyguards;
        static constexpr size_t MAX_BODYGUARDS = 7;
        void AddOption_BodyguardPed(const std::string& text, const GTAmodel::Model& model);
    }
}
