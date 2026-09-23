#include "Keybinds.h"

#include "..\macros.h"

#include "Menu.h"
#include "submenu_enum.h"

#include "..\Natives\natives2.h"
#include "..\Scripting\enums.h"
#include "..\Util\keyboard.h"
#include "..\Util\FileLogger.h"

#include <algorithm>
#include <set>
#include <string>
#include <vector>

namespace Keybinds
{
	GamepadLabelStyle gamepadLabelStyle = GamepadLabelStyle::Xbox;

	Context ResolveContext(Context context)
	{
		if (context != Context::Auto) return context;
		return Menu::usingControllerInput ? Context::Gamepad : Context::Keyboard;
	}

	// ── Registry ──────────────────────────────────────────────────────

	namespace
	{
		// KeybindEntry { id, label, 
			// description, 
			// keyboardKey1, keyboardKey2, gamepadButton1, gamepadButton2, category, useDisabledControls }
		// useDisabledControls = true means gamepad reads use IS_DISABLED_CONTROL_* (work over the menu), otherwise use IS_CONTROL_PRESSED/IS_CONTROL_JUST_PRESSED (work over the game world) when false
		const std::vector<KeybindEntry> g_registry
		{
			{ "menu_open", "Open Menu",
				"Opens Menyoo while it is closed, closes it while it is open.",
				(UINT16)VirtualKey::F8, NoBind, (UINT16)INPUT_FRONTEND_RB, (UINT16)INPUT_FRONTEND_LEFT, "General" },

			{ "spooner_mode", "Spooner Mode",
				"Toggles Spooner Mode on and off.",
				(UINT16)VirtualKey::F9, NoBind, (UINT16)INPUT_FRONTEND_RB, (UINT16)INPUT_FRONTEND_RIGHT, "Spooner" },

			{ "no_clip", "Noclip / FreeCam",
				"Activates the FreeCam while in Noclip.",
				(UINT16)VirtualKey::F3, NoBind, (UINT16)INPUT_FRONTEND_X, (UINT16)INPUT_FRONTEND_LS, "FreeCam", false },

			{ "freecam_hasten", "Hasten",
				"Speeds the FreeCam up by the hasten multiplier while held.",
				(UINT16)VirtualKey::LeftShift, NoBind, (UINT16)INPUT_FRONTEND_RB, NoBind, "FreeCam", true },

			{ "freecam_slow", "Slow Down",
				"Crawls the FreeCam at the slow speed while held.",
				(UINT16)VirtualKey::LeftControl, NoBind, NoBind, NoBind, "FreeCam", true },

			{ "freecam_height_lock", "Height Lock",
				"Toggles FreeCam height lock.",
				(UINT16)VirtualKey::Tab, NoBind, NoBind, NoBind, "FreeCam", false },

			{ "freecam_fov_mod", "FOV Modifier",
				"Hold and scroll to adjust the FreeCam field of view.",
				(UINT16)VirtualKey::Space, NoBind, NoBind, NoBind, "FreeCam", false },

			{ "stop_animation", "Stop Ped Animation",
				"Stops the running animation on the targeted ped.",
				(UINT16)VirtualKey::J, NoBind, NoBind, NoBind, "General" },

			{ "respawn", "Skip Respawn Screen",
				"Skips the death / arrest respawn screen before it times out.",
				NoBind, NoBind, (UINT16)INPUT_LOOK_BEHIND, NoBind, "General" },

			{ "category_jump", "Navigate Categories",
				"Opens the category navigator when a submenu has more than one category.",
				(UINT16)VirtualKey::G, NoBind, (UINT16)INPUT_SPECIAL_ABILITY, NoBind, "Menus" },

			{ "teleport_waypoint", "Teleport to Waypoint",
				"Teleports the player to the active map waypoint.",
				(UINT16)VirtualKey::T, NoBind, (UINT16)INPUT_FRONTEND_RLEFT, NoBind, "Teleport" },

			{ "tow_rope", "Tow Rope",
				"Extends or shortens the physics rope to the vehicle behind.",
				(UINT16)VirtualKey::K, NoBind, (UINT16)INPUT_FRONTEND_LS, NoBind, "Vehicles", false },

			{ "vehicle_boost", "Vehicle Boost",
				"Hold for a speed boost while driving. Enable it in Vehicle Options.",
				(UINT16)VirtualKey::E, NoBind, (UINT16)INPUT_VEH_HORN, NoBind, "Vehicles", false },

			{ "menu_action", "Menu Action Key",
				"Contextual action key used across menus: deletes items, toggles favourites, freezes stations.",
				(UINT16)VirtualKey::B, NoBind, (UINT16)INPUT_SCRIPT_RLEFT, NoBind, "Menus" },

			{ "spooner_marker", "Place Spooner Marker",
				"Places a marker at the Spooner camera aim point.",
				(UINT16)VirtualKey::M, NoBind, (UINT16)INPUT_FRONTEND_DOWN, NoBind, "Spooner" },

			{ "spooner_edit_mode", "Editor Mode Cycle",
				"Cycles the Spooner entity editor through Disabled / Keyboard / Gizmo.",
				(UINT16)VirtualKey::B, NoBind, NoBind, NoBind, "Spooner" },

			{ "spooner_sensitivity_up", "Editor Sensitivity Up",
				"Increases the Spooner editor movement / rotation sensitivity.",
				(UINT16)VirtualKey::OEMPlus, NoBind, NoBind, NoBind, "Spooner" },

			{ "spooner_sensitivity_down", "Editor Sensitivity Down",
				"Decreases the Spooner editor movement / rotation sensitivity.",
				(UINT16)VirtualKey::OEMMinus, NoBind, NoBind, NoBind, "Spooner" },

			{ "spooner_edit_transform", "Editor Transform Mode Cycle",
				"Cycles Position / Rotation / Scale while editing a Spooner entity.",
				(UINT16)VirtualKey::R, NoBind, NoBind, NoBind, "Spooner" },

			{ "spooner_camera_lock", "Editor Camera Lock",
				"Locks or unlocks the camera while editing a Spooner entity.",
				(UINT16)VirtualKey::V, NoBind, NoBind, NoBind, "Spooner" },

			{ "spooner_local_space", "Toggle World / Local Space",
				"Toggles between world and local space while editing a Spooner entity.",
				(UINT16)VirtualKey::L, NoBind, NoBind, NoBind, "Spooner" },

			{ "spooner_edit_copy", "Copy Entity",
				"Copies the hovered entity while the Spooner cursor is active, or the selected entity while editing.",
				(UINT16)VirtualKey::C, NoBind, (UINT16)INPUT_FRONTEND_RIGHT, NoBind, "Spooner" },

			{ "spooner_cursor_remove", "Delete Entity (cursor)",
				"Deletes the entity under the Spooner cursor.",
				(UINT16)VirtualKey::Delete, NoBind, (UINT16)INPUT_FRONTEND_LEFT, NoBind, "Spooner" },

			{ "spooner_cursor_add_to_db", "Add Entity to Database (cursor)",
				"Adds the entity under the Spooner cursor to the Spooner database.",
				(UINT16)VirtualKey::Up, NoBind, (UINT16)INPUT_FRONTEND_UP, NoBind, "Spooner" },

			{ "favourite_recategorize", "Change Category",
				"Moves a favourite entry to another category while browsing favourite lists.",
				(UINT16)VirtualKey::C, NoBind, (UINT16)INPUT_SCRIPT_RUP, NoBind, "Menus" },

			{ "vehicle_weapons_fire", "Fire Vehicle Weapons",
				"Hold to fire the enabled vehicle weapons. Enable them in Vehicle Options.",
				(UINT16)VirtualKey::Add, NoBind, (UINT16)INPUT_FRONTEND_LS, NoBind, "Vehicles", false },

			{ "vehicle_hydraulics", "Car Hydraulics",
				"Hold and steer to bounce the car on its hydraulics.",
				(UINT16)VirtualKey::LeftShift, NoBind, (UINT16)INPUT_FRONTEND_LS, NoBind, "Vehicles", true },

			{ "breathe_ptfx", "Breathe Effect",
				"Hold to breathe the selected particle effect. Enable it in Player Options > Breathe Stuff.",
				(UINT16)VirtualKey::J, NoBind, (UINT16)INPUT_FRONTEND_LS, NoBind, "Player", false },

			{ "superman_boost", "Superman Forward Boost",
				"Boosts the player forward while skydiving with Fly Manual enabled.",
				(UINT16)VirtualKey::Add, NoBind, (UINT16)INPUT_FRONTEND_RB, NoBind, "Player", false },

			{ "superman_freeze", "Superman Hover / Brake",
				"Freezes the player in place while held, resumes on release.",
				(UINT16)VirtualKey::Subtract, NoBind, (UINT16)INPUT_FRONTEND_RDOWN, NoBind, "Player", false },

			{ "superman_ascend", "Superman Ascend",
				"Pushes the flying player upward while Fly Manual is enabled.",
				(UINT16)VirtualKey::Numpad7, NoBind, (UINT16)INPUT_FRONTEND_RT, NoBind, "Player", false },

			{ "superman_descend", "Superman Descend",
				"Pushes the flying player downward while Fly Manual is enabled.",
				(UINT16)VirtualKey::Numpad1, NoBind, (UINT16)INPUT_FRONTEND_LT, NoBind, "Player", false },
		};
	}

	const std::vector<KeybindEntry>& Registry() { return g_registry; }

	const std::vector<std::string>& Categories()
	{
		static const std::vector<std::string> categories = [] {
			std::vector<std::string> result;
			for (const auto& entry : g_registry)
				if (std::find(result.begin(), result.end(), entry.Category()) == result.end())
					result.push_back(entry.Category());
			return result;
		}();
		return categories;
	}

	const KeybindEntry* FindEntry(const std::string& id)
	{
		for (const auto& entry : g_registry)
			if (entry.Id() == id)
				return &entry;
		return nullptr;
	}

	// ── Gamepad input reads ────────────────────────────────────────────

	namespace
	{
		// IS_DISABLED_CONTROL_* natives work over the menu, IS_CONTROL_* natives work over the game world. Use the former for menu navigation, the latter for in-game actions.
		bool GamepadPressed(bool useDisabledControls, UINT16 button)
		{
			return useDisabledControls
				? IS_DISABLED_CONTROL_PRESSED(ControlIndex(button), button)
				: IS_CONTROL_PRESSED(ControlIndex(button), button);
		}

		bool GamepadJustPressed(bool useDisabledControls, UINT16 button)
		{
			return useDisabledControls
				? IS_DISABLED_CONTROL_JUST_PRESSED(ControlIndex(button), button)
				: IS_CONTROL_JUST_PRESSED(ControlIndex(button), button);
		}

		bool GamepadJustReleased(bool useDisabledControls, UINT16 button)
		{
			return useDisabledControls
				? IS_DISABLED_CONTROL_JUST_RELEASED(ControlIndex(button), button)
				: IS_CONTROL_JUST_RELEASED(ControlIndex(button), button);
		}
	}

	// ── Entry methods ──────────────────────────────────────────────────

	// class constructor
	KeybindEntry::KeybindEntry(const std::string& id, const std::string& label, const std::string& description,
			UINT16 keyboardKey1, UINT16 keyboardKey2, UINT16 gamepadButton1, UINT16 gamepadButton2,
			const std::string& category, bool useDisabledControls)
		: m_id(id), m_label(label), m_description(description),
		m_keyboardKey1(keyboardKey1), m_keyboardKey2(keyboardKey2),
		m_gamepadButton1(gamepadButton1), m_gamepadButton2(gamepadButton2),
		m_category(category), m_useDisabledControls(useDisabledControls)
	{
	}
	
	// getters
	const std::string& KeybindEntry::Id() const { return m_id; }
	const std::string& KeybindEntry::Label() const { return m_label; }
	const std::string& KeybindEntry::Description() const { return m_description; }
	const std::string& KeybindEntry::Category() const { return m_category; }

	bool KeybindEntry::WasPressedThisFrame(Context context) const
	{
		if (IsRebinding()) return false;
		context = ResolveContext(context);

		if (context == Context::Keyboard)
		{
			if (m_keyboardKey1 == NoBind) return false; 							// no primary key = no press possible
			if (m_keyboardKey2 == NoBind) return IsKeyJustUp(m_keyboardKey1); 		// single-key bind: press edge on the primary
			return get_key_pressed(m_keyboardKey1) && IsKeyJustUp(m_keyboardKey2); 	// combo bind: primary held + secondary edge
		}
		else if (context == Context::Gamepad)
		{
			if (m_gamepadButton1 == NoBind) return false; 	// no primary button = no press possible
			if (m_gamepadButton2 == NoBind) return GamepadJustPressed(m_useDisabledControls, m_gamepadButton1); // single-button bind: press edge on the primary
			return GamepadPressed(m_useDisabledControls, m_gamepadButton1) && GamepadJustPressed(m_useDisabledControls, m_gamepadButton2); // combo bind: primary held + secondary edge
		}
		return false; // ResolveContext only yields Keyboard / Gamepad, so this is unreachable
	}

	bool KeybindEntry::WasReleasedThisFrame(Context context) const
	{
		if (IsRebinding()) return false;
		context = ResolveContext(context);

		if (context == Context::Keyboard)
		{
			// Keyboard press edges are key-up events, so this mirrors WasPressedThisFrame.
			if (m_keyboardKey1 == NoBind) return false; 							// no primary key = no release possible
			if (m_keyboardKey2 == NoBind) return IsKeyJustUp(m_keyboardKey1); 		// single-key bind: release edge is the press edge
			return get_key_pressed(m_keyboardKey1) && IsKeyJustUp(m_keyboardKey2); 	// combo bind: primary held + secondary released
		}
		else if (context == Context::Gamepad)
		{
			if (m_gamepadButton1 == NoBind) return false; 	// no primary button = no release possible
			if (m_gamepadButton2 == NoBind) return GamepadJustReleased(m_useDisabledControls, m_gamepadButton1); // single-button bind: release edge on the primary
			return GamepadPressed(m_useDisabledControls, m_gamepadButton1) && GamepadJustReleased(m_useDisabledControls, m_gamepadButton2); // combo bind: primary held + secondary released
		}
		return false; // ResolveContext only yields Keyboard / Gamepad, so this is unreachable
	}

	bool KeybindEntry::IsHeld(Context context) const
	{
		if (IsRebinding()) return false;
		context = ResolveContext(context);

		if (context == Context::Keyboard)
		{
			if (m_keyboardKey1 == NoBind) return false; // no primary key = not held
			if (m_keyboardKey2 != NoBind && !get_key_pressed(m_keyboardKey2)) return false; // combo: secondary must be held
			return get_key_pressed(m_keyboardKey1); // primary held
		}
		else if (context == Context::Gamepad)
		{
			if (m_gamepadButton1 == NoBind) return false; // no primary button = not held
			if (m_gamepadButton2 != NoBind && !GamepadPressed(m_useDisabledControls, m_gamepadButton2)) return false; // combo: secondary must be held
			return GamepadPressed(m_useDisabledControls, m_gamepadButton1); // primary held
		}
		return false; // ResolveContext only yields Keyboard / Gamepad, so this is unreachable
	}

	std::string KeybindEntry::Glyph(Context context) const
	{
		context = ResolveContext(context);

		if (context == Context::Keyboard)
		{
			if (m_keyboardKey1 == NoBind) return UnboundGlyph;
			std::string text = KeyboardKeyName(m_keyboardKey1);
			if (m_keyboardKey2 != NoBind)
				text += " + " + KeyboardKeyName(m_keyboardKey2);
			return text;
		}
		else if (context == Context::Gamepad)
		{
			if (m_gamepadButton1 == NoBind) return UnboundGlyph;
			std::string text = GamepadButtonName(m_gamepadButton1);
			if (m_gamepadButton2 != NoBind)
				text += " + " + GamepadButtonName(m_gamepadButton2);
			return text;
		}
		return UnboundGlyph; // not really reachable
	}

	// ── Menu-facing helpers ────────────────────────────────────────────

	namespace
	{
		// log an unknown bind id only once per session, to avoid spamming the log with repeated calls to the same id
		void LogUnknownIdOnce(const char* apiName, const std::string& id)
		{
			static std::set<std::string> loggedIds;
			if (loggedIds.insert(id).second)
				addlog(ige::LogType::LOG_ERROR,
					std::string("Keybinds::") + apiName + "(\"" + id + "\"): unknown bind id - action disabled");
		}
	}

	bool HasConflict(const KeybindEntry& entry, Context context)
	{
		if (IsRebinding()) return false;
		context = ResolveContext(context);

		const bool usingKeyboard = context == Context::Keyboard;
		const auto first = [&](const KeybindEntry& e) {
			if (usingKeyboard) return e.KeyboardKey1();
			return e.GamepadButton1();
		};
		const auto second = [&](const KeybindEntry& e) {
			if (usingKeyboard) return e.KeyboardKey2();
			return e.GamepadButton2();
		};

		// A conflict is a bind in the same category whose whole key/button set matches this entry's;
		if (first(entry) == NoBind) return false;
		for (const auto& other : g_registry)
		{
			if (&other == &entry) continue; // skip self
			if (other.Category() != entry.Category()) continue; // cross-category duplicates are expected and not a conflict
			if (first(entry) == first(other) && second(entry) == second(other))
				return true;
		}
		return false;
	}

	std::string ColorizeRowText(const KeybindEntry& entry, Context context)
	{
		std::string text = entry.Glyph(context);
		if (HasConflict(entry, context)) 	// red text for a conflicting bind
			return "~r~" + text + "~s~";
		if (text == UnboundGlyph) 			// "—", unbound
			return "~c~" + text + "~s~";
		return text;
	}

	void AddBindIB(const std::string& id, const std::string& text)
	{
		const KeybindEntry* entry = FindEntry(id);
		if (entry == nullptr)
		{
			LogUnknownIdOnce("AddBindIB", id);
			return;
		}
		if (Menu::usingControllerInput)
		{
			if (entry->GamepadButton1() == NoBind) return; // no primary button = nothing to add
			if (entry->GamepadButton2() == NoBind)
				Menu::add_IB((ControllerInput)entry->GamepadButton1(), text); // single-button bind: add the primary
			else
				Menu::add_IB((ControllerInput)entry->GamepadButton1(), (ControllerInput)entry->GamepadButton2(), text); // combo bind: add both members
		}
		else
		{
			if (entry->KeyboardKey1() == NoBind) return; // no primary key = nothing to add
			if (entry->KeyboardKey2() == NoBind)
				Menu::add_IB((VirtualKey::VirtualKey)entry->KeyboardKey1(), text); // single-key bind: add the primary
			else
				Menu::add_IB((VirtualKey::VirtualKey)entry->KeyboardKey1(), (VirtualKey::VirtualKey)entry->KeyboardKey2(), text); // combo bind: add both members
		}
	}

	void AddBindIB(const std::string& id, bool active, const std::string& onText, const std::string& offText)
	{
		AddBindIB(id, active ? onText : offText);
	}

	// ── id-based convenience API ────────────────────────────────────────

	bool WasPressedThisFrame(const std::string& id, Context context)
	{
		const KeybindEntry* entry = FindEntry(id);
		if (entry == nullptr)
		{
			LogUnknownIdOnce("WasPressedThisFrame", id);
			return false;
		}
		return entry->WasPressedThisFrame(context);
	}

	bool WasReleasedThisFrame(const std::string& id, Context context)
	{
		const KeybindEntry* entry = FindEntry(id);
		if (entry == nullptr)
		{
			LogUnknownIdOnce("WasReleasedThisFrame", id);
			return false;
		}
		return entry->WasReleasedThisFrame(context);
	}

	bool IsHeld(const std::string& id, Context context)
	{
		const KeybindEntry* entry = FindEntry(id);
		if (entry == nullptr)
		{
			LogUnknownIdOnce("IsHeld", id);
			return false;
		}
		return entry->IsHeld(context);
	}

	std::string GetGlyph(const std::string& id, Context context)
	{
		const KeybindEntry* entry = FindEntry(id);
		if (entry != nullptr) return entry->Glyph(context);
		LogUnknownIdOnce("GetGlyph", id);
		return UnboundGlyph;
	}

	bool IsKeybindSubmenu()
	{
		return Menu::activeSubmenu == SUB::SETTINGS_KEYBINDS_KEYBOARD ||
			Menu::activeSubmenu == SUB::SETTINGS_KEYBINDS_GAMEPAD;
	}
}