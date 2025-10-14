#pragma once
#include <string>
#include "Scripting/Model.h"
#include "../source/Menu/Routine.h"
#include "Scripting/GTAped.h"

typedef int INT;
typedef signed char INT8;
typedef unsigned char UINT8;
typedef unsigned long DWORD, Hash;

namespace GTAmodel {
	class Model;
}
namespace sub::BodyguardMenu
{
	namespace BodyguardManagement
	{
		extern std::string& _searchStr;
		void BodyguardSpawn();
		void AddBodyguard_Ped(const std::string& text, const GTAmodel::Model& model);
	}
}