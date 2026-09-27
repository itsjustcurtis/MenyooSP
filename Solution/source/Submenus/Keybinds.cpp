/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
*/

#include "Keybinds.h"

#include "..\Menu\submenu_enum.h"
#include "..\Menu\MenuCategory.h"

namespace sub
{
	namespace
	{
		void DrawBindCategories(Keybinds::Context context)
		{
			for (const std::string& category : Keybinds::Categories())
			{
				int count = 0;
				for (const auto& entry : Keybinds::Registry())
					if (entry.Category() == category) count++;
				std::string catLabel = std::string("— ~b~") + category + "~s~ ~c~(" + std::to_string(count) + " binds)~s~";

				if (!MenuCategory::AddCategory(catLabel))
					continue;
				for (const auto& entry : Keybinds::Registry())
				{
					if (entry.Category() != category) continue;
					bool pressed = false;
					bool rebindingThis = Keybinds::IsRebinding(&entry, context);
					AddKeybindOption(entry.Label(), Keybinds::ColorizeRowText(entry, context), entry.Description(), pressed, rebindingThis);
					if (pressed) Keybinds::StartRebind(entry, context);
				}
			}
		}
	}

	void KeybindsMenu()
	{
		AddTitle("Keybinds");
		AddOption("Keyboard", null, nullFunc, SUB::SETTINGS_KEYBINDS_KEYBOARD);
		AddOption("Controller", null, nullFunc, SUB::SETTINGS_KEYBINDS_GAMEPAD);
		AddOptionDescription("Rebind the keys and buttons used by Menyoo.");
		int styleIdx = AddTexterCycler("Gamepad Labels", (int)Keybinds::gamepadLabelStyle, { "Xbox", "PlayStation" });
		if (styleIdx != (int)Keybinds::gamepadLabelStyle)
		{
			Keybinds::gamepadLabelStyle = (Keybinds::GamepadLabelStyle)styleIdx;
			Keybinds::Save();
		}
	}

	void KeybindsKeyboard()
	{
		AddTitle("Keybinds - Keyboard");
		DrawBindCategories(Keybinds::Context::Keyboard);
	}

	void KeybindsGamepad()
	{
		AddTitle("Keybinds - Controller");
		DrawBindCategories(Keybinds::Context::Gamepad);
	}
}

#include "..\Menu\submenu_switch.h"

REGISTER_SUBMENU(SETTINGS_KEYBINDS, sub::KeybindsMenu)
REGISTER_SUBMENU(SETTINGS_KEYBINDS_KEYBOARD, sub::KeybindsKeyboard)
REGISTER_SUBMENU(SETTINGS_KEYBINDS_GAMEPAD, sub::KeybindsGamepad)