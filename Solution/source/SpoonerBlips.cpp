#include "Menu/Menu.h"
#include "Scripting/GTAblip.h"
#include "Menu/Routine.h"
#include "BlipManagement.h"

#include <vector>

namespace sub
{

namespace Spooner::Submenus
{
	void Sub_Blip_Management()
	{
		AddTitle("Blip Management");

		AddOption("Add Blip", null, nullFunc, SUB::SPOONER_BLIPS_ADD_SELECT);
	}

    void Sub_Blip_Select()
    {
        AddTitle("Select Blip Type");

        bool bAddNewRadialBlipPressed = false;
        AddTickol("Create Radial Blip", true, bAddNewRadialBlipPressed, bAddNewRadialBlipPressed, TICKOL::SMALLNEWSTAR);
        if (bAddNewRadialBlipPressed)
        {
			auto& spoocam = SpoonerMode::spoonerModeCamera;
			if (!spoocam.IsActive())
			{
				GTAentity myPed = PLAYER_PED_ID();
				Vector3 myPos = myPed.Position_get();
				SelectedBlip = BlipManagement::AddBlip(myPos, Vector3(0, 0, myPed.Heading_get()));
			}
			else
			{
				Vector3 spawnPos = spoocam.RaycastForCoord(Vector2(0.0f, 0.0f), 0, 120.0f, 30.0f + SpoonerBlip().m_scale / 2);
				spawnPos.z += SpoonerBlip().m_scale / 2;
				SelectedBlip = BlipManagement::AddBlip(spawnPos, Vector3(0, 0, spoocam.Rotation_get().z));
			}
            Menu::SetSub_delayed = SUB::SPOONER_BLIPS_RADIALINBLIP;
        }

        AddOption("Attach Blip to Entity", null, nullFunc, SUB::SPOONER_BLIPS_ADD_ENTITY);

        AddOption("Create Coord Blip", null, nullFunc, SUB::SPOONER_BLIPS_ADD_COORD);
    }

}
}