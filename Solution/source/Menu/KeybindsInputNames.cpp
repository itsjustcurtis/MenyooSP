#include "Keybinds.h"

#include "..\Scripting\enums.h"
#include "..\Util\keyboard.h"

#include <cctype>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace Keybinds
{
	namespace
	{
		std::string HexKey(UINT16 vk)
		{
			std::stringstream ss;
			ss << "0x" << std::hex << std::uppercase << vk;
			return ss.str();
		}

		std::string TitleCase(std::string name)
		{
			for (size_t i = 0; i < name.size(); ++i)
			{
				if (name[i] == '_') name[i] = ' ';
				else if (i == 0 || name[i - 1] == ' ') name[i] = (char)toupper(name[i]);
				else name[i] = (char)tolower(name[i]);
			}
			return name;
		}

		// Keycap width prefix for the instructional_buttons scaleform: everything after the
		// prefix is rendered as literal text inside keycap art of the matching width, so the
		// label must be a single token (no spaces) and the prefix must fit its length.
		std::string KeycapPrefix(size_t labelLength)
		{
			if (labelLength <= 2) return "t_";
			if (labelLength <= 4) return "T_";
			return "w_";
		}

		// Single-token uppercase keycap label; nullptr when the key has no sensible label.
		const char* KeycapLabel(UINT16 vk)
		{
			switch (vk)
			{
				case VirtualKey::Back: return "BACKSPACE";
				case VirtualKey::Tab: return "TAB";
				case VirtualKey::Return: return "ENTER";
				case VirtualKey::Shift: case VirtualKey::LeftShift: case VirtualKey::RightShift: return "SHIFT";
				case VirtualKey::Control: case VirtualKey::LeftControl: case VirtualKey::RightControl: return "CTRL";
				case VirtualKey::Menu: case VirtualKey::LeftMenu: case VirtualKey::RightMenu: return "ALT";
				case VirtualKey::Pause: return "PAUSE";
				case VirtualKey::CapsLock: return "CAPSLOCK";
				case VirtualKey::Escape: return "ESC";
				case VirtualKey::Space: return "SPACE";
				case VirtualKey::Prior: return "PGUP";
				case VirtualKey::Next: return "PGDN";
				case VirtualKey::End: return "END";
				case VirtualKey::Home: return "HOME";
				case VirtualKey::Left: return "LEFT";
				case VirtualKey::Up: return "UP";
				case VirtualKey::Right: return "RIGHT";
				case VirtualKey::Down: return "DOWN";
				case VirtualKey::Snapshot: return "PRTSCN";
				case VirtualKey::Insert: return "INS";
				case VirtualKey::Delete: return "DEL";
				case VirtualKey::Help: return "HELP";
				case VirtualKey::LeftWindows: case VirtualKey::RightWindows: return "WIN";
				case VirtualKey::Multiply: return "NUM*";
				case VirtualKey::Add: return "NUM+";
				case VirtualKey::Separator: return "NUM,";
				case VirtualKey::Subtract: return "NUM-";
				case VirtualKey::Decimal: return "NUM.";
				case VirtualKey::Divide: return "NUM/";
				case VirtualKey::NumLock: return "NUMLOCK";
				case VirtualKey::ScrollLock: return "SCROLL";
				case VirtualKey::OEM1: return ";";
				case VirtualKey::OEMPlus: return "=";
				case VirtualKey::OEMComma: return ",";
				case VirtualKey::OEMMinus: return "-";
				case VirtualKey::OEMPeriod: return ".";
				case VirtualKey::OEM2: return "/";
				case VirtualKey::OEM3: return "`";
				case VirtualKey::OEM4: return "[";
				case VirtualKey::OEM5: return "\\";
				case VirtualKey::OEM6: return "]";
				case VirtualKey::OEM7: return "'";
				case VirtualKey::OEM102: return "\\";
				default: return nullptr;
			}
		}

		// declared here, defined at the bottom of this file
		const std::map<UINT16, const char*>& GamepadButtonNames();
		const std::map<std::string, const char*>& PlayStationNames();
	}

	std::string KeyboardKeyName(UINT16 vk)
	{
		if (vk == NoBind) return UnboundGlyph;
		std::string name = VkCodeToStr((UINT8)vk);
		return name.empty() ? HexKey(vk) : name;
	}

	// Full instructional-buttons icon id for a keyboard key ("t_K", "T_CTRL", "w_BACKSPACE"),
	// or "" for NoBind. Consumed only by the instructional_buttons scaleform (Menu::get_key_IB);
	// never place these ids inside display text.
	std::string KeyboardKeyIbId(UINT16 vk)
	{
		if (vk == NoBind) return "";

		std::string label;
		const char* named = KeycapLabel(vk);
		if (named != nullptr)
		{
			label = named;
		}
		else if (vk >= VirtualKey::N0 && vk <= VirtualKey::N9)
		{
			label = std::to_string(vk - VirtualKey::N0);
		}
		else if (vk >= VirtualKey::A && vk <= VirtualKey::Z)
		{
			label = std::string(1, static_cast<char>('A' + (vk - VirtualKey::A)));
		}
		else if (vk >= VirtualKey::Numpad0 && vk <= VirtualKey::Numpad9)
		{
			label = "NUM" + std::to_string(vk - VirtualKey::Numpad0);
		}
		else if (vk >= VirtualKey::F1 && vk <= VirtualKey::F24)
		{
			label = "F" + std::to_string(vk - VirtualKey::F1 + 1);
		}
		else
		{
			label = HexKey(vk); // unknown key; "0xNN" is still a single safe token
		}
		return KeycapPrefix(label.length()) + label;
	}

	std::string GamepadButtonName(UINT16 control)
	{
		if (control == NoBind) return UnboundGlyph;
		std::string name;
		auto it = GamepadButtonNames().find(control);
		if (it != GamepadButtonNames().end())
			name = it->second;
		else
		{
			try
			{
				name = ControllerInputs::vNames.at(control);
				if (name.empty()) return HexKey(control);
				name = TitleCase(name); // contextual actions (e.g. INPUT_LOOK_BEHIND -> "Look Behind")
			}
			catch (...)
			{
				return HexKey(control);
			}
		}
		if (gamepadLabelStyle == GamepadLabelStyle::PlayStation)
		{
			auto ps = PlayStationNames().find(name);
			if (ps != PlayStationNames().end()) name = ps->second;
		}
		return name;
	}

	// ── Button-name lookup data ────────────────────────────────────────

	namespace
	{
		// ControllerInput id (current-game numbering, per the Cfx controls reference) -> physical
		// gamepad layout with the default Xbox button names. Ids without a pad binding (mouse/
		// keyboard-only actions, contextual actions) are omitted and fall through to the title-case
		// action name or hex. Regenerate with the Cfx "Controls" table parse (see KEYBINDS.md).
		const std::map<UINT16, const char*>& GamepadButtonNames()
		{
			static const std::map<UINT16, const char*> names =
			{
				{ 0, "Back" },  // INPUT_NEXT_CAMERA (0)
				{ 1, "Right Stick" },  // INPUT_LOOK_LR (1)
				{ 2, "Right Stick" },  // INPUT_LOOK_UD (2)
				{ 3, "Right Stick" },  // INPUT_LOOK_UP_ONLY (3)
				{ 4, "Right Stick" },  // INPUT_LOOK_DOWN_ONLY (4)
				{ 5, "Right Stick" },  // INPUT_LOOK_LEFT_ONLY (5)
				{ 6, "Right Stick" },  // INPUT_LOOK_RIGHT_ONLY (6)
				{ 7, "R3" },  // INPUT_CINEMATIC_SLOWMO (7)
				{ 8, "Left Stick" },  // INPUT_SCRIPTED_FLY_UD (8)
				{ 9, "Left Stick" },  // INPUT_SCRIPTED_FLY_LR (9)
				{ 10, "LT" },  // INPUT_SCRIPTED_FLY_ZUP (10)
				{ 11, "RT" },  // INPUT_SCRIPTED_FLY_ZDOWN (11)
				{ 12, "Right Stick" },  // INPUT_WEAPON_WHEEL_UD (12)
				{ 13, "Right Stick" },  // INPUT_WEAPON_WHEEL_LR (13)
				{ 14, "D-pad Right" },  // INPUT_WEAPON_WHEEL_NEXT (14)
				{ 15, "D-pad Left" },  // INPUT_WEAPON_WHEEL_PREV (15)
				{ 18, "A" },  // INPUT_SKIP_CUTSCENE (18)
				{ 19, "D-pad Down" },  // INPUT_CHARACTER_WHEEL (19)
				{ 20, "D-pad Down" },  // INPUT_MULTIPLAYER_INFO (20)
				{ 21, "A" },  // INPUT_SPRINT (21)
				{ 22, "X" },  // INPUT_JUMP (22)
				{ 23, "Y" },  // INPUT_ENTER (23)
				{ 24, "RT" },  // INPUT_ATTACK (24)
				{ 25, "LT" },  // INPUT_AIM (25)
				{ 26, "R3" },  // INPUT_LOOK_BEHIND (26)
				{ 27, "D-pad Up" },  // INPUT_PHONE (27)
				{ 28, "L3" },  // INPUT_SPECIAL_ABILITY (28)
				{ 29, "R3" },  // INPUT_SPECIAL_ABILITY_SECONDARY (29)
				{ 30, "Left Stick" },  // INPUT_MOVE_LR (30)
				{ 31, "Left Stick" },  // INPUT_MOVE_UD (31)
				{ 32, "Left Stick" },  // INPUT_MOVE_UP_ONLY (32)
				{ 33, "Left Stick" },  // INPUT_MOVE_DOWN_ONLY (33)
				{ 34, "Left Stick" },  // INPUT_MOVE_LEFT_ONLY (34)
				{ 35, "Left Stick" },  // INPUT_MOVE_RIGHT_ONLY (35)
				{ 36, "L3" },  // INPUT_DUCK (36)
				{ 37, "LB" },  // INPUT_SELECT_WEAPON (37)
				{ 38, "LB" },  // INPUT_PICKUP (38)
				{ 39, "Left Stick" },  // INPUT_SNIPER_ZOOM (39)
				{ 40, "Left Stick" },  // INPUT_SNIPER_ZOOM_IN_ONLY (40)
				{ 41, "Left Stick" },  // INPUT_SNIPER_ZOOM_OUT_ONLY (41)
				{ 42, "D-pad Up" },  // INPUT_SNIPER_ZOOM_IN_SECONDARY (42)
				{ 43, "D-pad Down" },  // INPUT_SNIPER_ZOOM_OUT_SECONDARY (43)
				{ 44, "RB" },  // INPUT_COVER (44)
				{ 45, "B" },  // INPUT_RELOAD (45)
				{ 46, "D-pad Right" },  // INPUT_TALK (46)
				{ 47, "D-pad Left" },  // INPUT_DETONATE (47)
				{ 48, "D-pad Down" },  // INPUT_HUD_SPECIAL (48)
				{ 49, "Y" },  // INPUT_ARREST (49)
				{ 50, "R3" },  // INPUT_ACCURATE_AIM (50)
				{ 51, "D-pad Right" },  // INPUT_CONTEXT (51)
				{ 52, "D-pad Left" },  // INPUT_CONTEXT_SECONDARY (52)
				{ 53, "Y" },  // INPUT_WEAPON_SPECIAL (53)
				{ 54, "D-pad Right" },  // INPUT_WEAPON_SPECIAL_TWO (54)
				{ 55, "RB" },  // INPUT_DIVE (55)
				{ 56, "Y" },  // INPUT_DROP_WEAPON (56)
				{ 57, "B" },  // INPUT_DROP_AMMO (57)
				{ 58, "D-pad Left" },  // INPUT_THROW_GRENADE (58)
				{ 59, "Left Stick" },  // INPUT_VEH_MOVE_LR (59)
				{ 60, "Left Stick" },  // INPUT_VEH_MOVE_UD (60)
				{ 61, "Left Stick" },  // INPUT_VEH_MOVE_UP_ONLY (61)
				{ 62, "Left Stick" },  // INPUT_VEH_MOVE_DOWN_ONLY (62)
				{ 63, "Left Stick" },  // INPUT_VEH_MOVE_LEFT_ONLY (63)
				{ 64, "Left Stick" },  // INPUT_VEH_MOVE_RIGHT_ONLY (64)
				{ 66, "Right Stick" },  // INPUT_VEH_GUN_LR (66)
				{ 67, "Right Stick" },  // INPUT_VEH_GUN_UD (67)
				{ 68, "LB" },  // INPUT_VEH_AIM (68)
				{ 69, "RB" },  // INPUT_VEH_ATTACK (69)
				{ 70, "A" },  // INPUT_VEH_ATTACK2 (70)
				{ 71, "RT" },  // INPUT_VEH_ACCELERATE (71)
				{ 72, "LT" },  // INPUT_VEH_BRAKE (72)
				{ 73, "A" },  // INPUT_VEH_DUCK (73)
				{ 74, "D-pad Right" },  // INPUT_VEH_HEADLIGHT (74)
				{ 75, "Y" },  // INPUT_VEH_EXIT (75)
				{ 76, "RB" },  // INPUT_VEH_HANDBRAKE (76)
				{ 77, "LT" },  // INPUT_VEH_HOTWIRE_LEFT (77)
				{ 78, "RT" },  // INPUT_VEH_HOTWIRE_RIGHT (78)
				{ 79, "R3" },  // INPUT_VEH_LOOK_BEHIND (79)
				{ 80, "B" },  // INPUT_VEH_CIN_CAM (80)
				{ 85, "D-pad Left" },  // INPUT_VEH_RADIO_WHEEL (85)
				{ 86, "L3" },  // INPUT_VEH_HORN (86)
				{ 87, "RT" },  // INPUT_VEH_FLY_THROTTLE_UP (87)
				{ 88, "LT" },  // INPUT_VEH_FLY_THROTTLE_DOWN (88)
				{ 89, "LB" },  // INPUT_VEH_FLY_YAW_LEFT (89)
				{ 90, "RB" },  // INPUT_VEH_FLY_YAW_RIGHT (90)
				{ 91, "LT" },  // INPUT_VEH_PASSENGER_AIM (91)
				{ 92, "RT" },  // INPUT_VEH_PASSENGER_ATTACK (92)
				{ 93, "R3" },  // INPUT_VEH_SPECIAL_ABILITY_FRANKLIN (93)
				{ 95, "Right Stick" },  // INPUT_VEH_CINEMATIC_UD (95)
				{ 98, "Right Stick" },  // INPUT_VEH_CINEMATIC_LR (98)
				{ 99, "X" },  // INPUT_VEH_SELECT_NEXT_WEAPON (99)
				{ 101, "D-pad Right" },  // INPUT_VEH_ROOF (101)
				{ 102, "RB" },  // INPUT_VEH_JUMP (102)
				{ 103, "D-pad Right" },  // INPUT_VEH_GRAPPLING_HOOK (103)
				{ 104, "D-pad Right" },  // INPUT_VEH_SHUFFLE (104)
				{ 105, "A" },  // INPUT_VEH_DROP_PROJECTILE (105)
				{ 107, "Left Stick" },  // INPUT_VEH_FLY_ROLL_LR (107)
				{ 108, "Left Stick" },  // INPUT_VEH_FLY_ROLL_LEFT_ONLY (108)
				{ 109, "Left Stick" },  // INPUT_VEH_FLY_ROLL_RIGHT_ONLY (109)
				{ 110, "Left Stick" },  // INPUT_VEH_FLY_PITCH_UD (110)
				{ 111, "Left Stick" },  // INPUT_VEH_FLY_PITCH_UP_ONLY (111)
				{ 112, "Left Stick" },  // INPUT_VEH_FLY_PITCH_DOWN_ONLY (112)
				{ 113, "L3" },  // INPUT_VEH_FLY_UNDERCARRIAGE (113)
				{ 114, "A" },  // INPUT_VEH_FLY_ATTACK (114)
				{ 115, "D-pad Left" },  // INPUT_VEH_FLY_SELECT_NEXT_WEAPON (115)
				{ 117, "LB" },  // INPUT_VEH_FLY_SELECT_TARGET_LEFT (117)
				{ 118, "RB" },  // INPUT_VEH_FLY_SELECT_TARGET_RIGHT (118)
				{ 119, "D-pad Right" },  // INPUT_VEH_FLY_VERTICAL_FLIGHT_MODE (119)
				{ 120, "A" },  // INPUT_VEH_FLY_DUCK (120)
				{ 121, "R3" },  // INPUT_VEH_FLY_ATTACK_CAMERA (121)
				{ 123, "Left Stick" },  // INPUT_VEH_SUB_TURN_LR (123)
				{ 124, "Left Stick" },  // INPUT_VEH_SUB_TURN_LEFT_ONLY (124)
				{ 125, "Left Stick" },  // INPUT_VEH_SUB_TURN_RIGHT_ONLY (125)
				{ 126, "Left Stick" },  // INPUT_VEH_SUB_PITCH_UD (126)
				{ 127, "Left Stick" },  // INPUT_VEH_SUB_PITCH_UP_ONLY (127)
				{ 128, "Left Stick" },  // INPUT_VEH_SUB_PITCH_DOWN_ONLY (128)
				{ 129, "RT" },  // INPUT_VEH_SUB_THROTTLE_UP (129)
				{ 130, "LT" },  // INPUT_VEH_SUB_THROTTLE_DOWN (130)
				{ 131, "X" },  // INPUT_VEH_SUB_ASCEND (131)
				{ 132, "A" },  // INPUT_VEH_SUB_DESCEND (132)
				{ 133, "LB" },  // INPUT_VEH_SUB_TURN_HARD_LEFT (133)
				{ 134, "RB" },  // INPUT_VEH_SUB_TURN_HARD_RIGHT (134)
				{ 136, "A" },  // INPUT_VEH_PUSHBIKE_PEDAL (136)
				{ 137, "A" },  // INPUT_VEH_PUSHBIKE_SPRINT (137)
				{ 138, "LT" },  // INPUT_VEH_PUSHBIKE_FRONT_BRAKE (138)
				{ 139, "RT" },  // INPUT_VEH_PUSHBIKE_REAR_BRAKE (139)
				{ 140, "B" },  // INPUT_MELEE_ATTACK_LIGHT (140)
				{ 141, "A" },  // INPUT_MELEE_ATTACK_HEAVY (141)
				{ 142, "RT" },  // INPUT_MELEE_ATTACK_ALTERNATE (142)
				{ 143, "X" },  // INPUT_MELEE_BLOCK (143)
				{ 144, "Y" },  // INPUT_PARACHUTE_DEPLOY (144)
				{ 145, "Y" },  // INPUT_PARACHUTE_DETACH (145)
				{ 146, "Left Stick" },  // INPUT_PARACHUTE_TURN_LR (146)
				{ 147, "Left Stick" },  // INPUT_PARACHUTE_TURN_LEFT_ONLY (147)
				{ 148, "Left Stick" },  // INPUT_PARACHUTE_TURN_RIGHT_ONLY (148)
				{ 149, "Left Stick" },  // INPUT_PARACHUTE_PITCH_UD (149)
				{ 150, "Left Stick" },  // INPUT_PARACHUTE_PITCH_UP_ONLY (150)
				{ 151, "Left Stick" },  // INPUT_PARACHUTE_PITCH_DOWN_ONLY (151)
				{ 152, "LB" },  // INPUT_PARACHUTE_BRAKE_LEFT (152)
				{ 153, "RB" },  // INPUT_PARACHUTE_BRAKE_RIGHT (153)
				{ 154, "A" },  // INPUT_PARACHUTE_SMOKE (154)
				{ 170, "B" },  // INPUT_SAVE_REPLAY_CLIP (170)
				{ 172, "D-pad Up" },  // INPUT_CELLPHONE_UP (172)
				{ 173, "D-pad Down" },  // INPUT_CELLPHONE_DOWN (173)
				{ 174, "D-pad Left" },  // INPUT_CELLPHONE_LEFT (174)
				{ 175, "D-pad Right" },  // INPUT_CELLPHONE_RIGHT (175)
				{ 176, "A" },  // INPUT_CELLPHONE_SELECT (176)
				{ 177, "B" },  // INPUT_CELLPHONE_CANCEL (177)
				{ 178, "Y" },  // INPUT_CELLPHONE_OPTION (178)
				{ 179, "X" },  // INPUT_CELLPHONE_EXTRA_OPTION (179)
				{ 182, "RT" },  // INPUT_CELLPHONE_CAMERA_FOCUS_LOCK (182)
				{ 183, "RB" },  // INPUT_CELLPHONE_CAMERA_GRID (183)
				{ 184, "R3" },  // INPUT_CELLPHONE_CAMERA_SELFIE (184)
				{ 185, "LB" },  // INPUT_CELLPHONE_CAMERA_DOF (185)
				{ 186, "L3" },  // INPUT_CELLPHONE_CAMERA_EXPRESSION (186)
				{ 187, "D-pad Down" },  // INPUT_FRONTEND_DOWN (187)
				{ 188, "D-pad Up" },  // INPUT_FRONTEND_UP (188)
				{ 189, "D-pad Left" },  // INPUT_FRONTEND_LEFT (189)
				{ 190, "D-pad Right" },  // INPUT_FRONTEND_RIGHT (190)
				{ 191, "A" },  // INPUT_FRONTEND_RDOWN (191)
				{ 192, "Y" },  // INPUT_FRONTEND_RUP (192)
				{ 193, "X" },  // INPUT_FRONTEND_RLEFT (193)
				{ 194, "B" },  // INPUT_FRONTEND_RRIGHT (194)
				{ 195, "Left Stick" },  // INPUT_FRONTEND_AXIS_X (195)
				{ 196, "Left Stick" },  // INPUT_FRONTEND_AXIS_Y (196)
				{ 197, "Right Stick" },  // INPUT_FRONTEND_RIGHT_AXIS_X (197)
				{ 198, "Right Stick" },  // INPUT_FRONTEND_RIGHT_AXIS_Y (198)
				{ 199, "Start" },  // INPUT_FRONTEND_PAUSE (199)
				{ 201, "A" },  // INPUT_FRONTEND_ACCEPT (201)
				{ 202, "B" },  // INPUT_FRONTEND_CANCEL (202)
				{ 203, "X" },  // INPUT_FRONTEND_X (203)
				{ 204, "Y" },  // INPUT_FRONTEND_Y (204)
				{ 205, "LB" },  // INPUT_FRONTEND_LB (205)
				{ 206, "RB" },  // INPUT_FRONTEND_RB (206)
				{ 207, "LT" },  // INPUT_FRONTEND_LT (207)
				{ 208, "RT" },  // INPUT_FRONTEND_RT (208)
				{ 209, "L3" },  // INPUT_FRONTEND_LS (209)
				{ 210, "R3" },  // INPUT_FRONTEND_RS (210)
				{ 211, "RB" },  // INPUT_FRONTEND_LEADERBOARD (211)
				{ 212, "Back" },  // INPUT_FRONTEND_SOCIAL_CLUB (212)
				{ 213, "RB" },  // INPUT_FRONTEND_SOCIAL_CLUB_SECONDARY (213)
				{ 214, "X" },  // INPUT_FRONTEND_DELETE (214)
				{ 215, "A" },  // INPUT_FRONTEND_ENDSCREEN_ACCEPT (215)
				{ 216, "X" },  // INPUT_FRONTEND_ENDSCREEN_EXPAND (216)
				{ 217, "Back" },  // INPUT_FRONTEND_SELECT (217)
				{ 218, "Left Stick" },  // INPUT_SCRIPT_LEFT_AXIS_X (218)
				{ 219, "Left Stick" },  // INPUT_SCRIPT_LEFT_AXIS_Y (219)
				{ 220, "Right Stick" },  // INPUT_SCRIPT_RIGHT_AXIS_X (220)
				{ 221, "Right Stick" },  // INPUT_SCRIPT_RIGHT_AXIS_Y (221)
				{ 222, "Y" },  // INPUT_SCRIPT_RUP (222)
				{ 223, "A" },  // INPUT_SCRIPT_RDOWN (223)
				{ 224, "X" },  // INPUT_SCRIPT_RLEFT (224)
				{ 225, "B" },  // INPUT_SCRIPT_RRIGHT (225)
				{ 226, "LB" },  // INPUT_SCRIPT_LB (226)
				{ 227, "RB" },  // INPUT_SCRIPT_RB (227)
				{ 228, "LT" },  // INPUT_SCRIPT_LT (228)
				{ 229, "RT" },  // INPUT_SCRIPT_RT (229)
				{ 230, "L3" },  // INPUT_SCRIPT_LS (230)
				{ 231, "R3" },  // INPUT_SCRIPT_RS (231)
				{ 232, "D-pad Up" },  // INPUT_SCRIPT_PAD_UP (232)
				{ 233, "D-pad Down" },  // INPUT_SCRIPT_PAD_DOWN (233)
				{ 234, "D-pad Left" },  // INPUT_SCRIPT_PAD_LEFT (234)
				{ 235, "D-pad Right" },  // INPUT_SCRIPT_PAD_RIGHT (235)
				{ 236, "Back" },  // INPUT_SCRIPT_SELECT (236)
				{ 244, "Back" },  // INPUT_INTERACTION_MENU (244)
				{ 250, "L3" },  // INPUT_CREATOR_LS (250)
				{ 251, "R3" },  // INPUT_CREATOR_RS (251)
				{ 252, "LT" },  // INPUT_CREATOR_LT (252)
				{ 253, "RT" },  // INPUT_CREATOR_RT (253)
				{ 255, "A" },  // INPUT_CREATOR_ACCEPT (255)
				{ 256, "X" },  // INPUT_CREATOR_DELETE (256)
				{ 257, "RT" },  // INPUT_ATTACK2 (257)
				{ 258, "A" },  // INPUT_RAPPEL_JUMP (258)
				{ 259, "X" },  // INPUT_RAPPEL_LONG_JUMP (259)
				{ 260, "RT" },  // INPUT_RAPPEL_SMASH_WINDOW (260)
				{ 261, "D-pad Left" },  // INPUT_PREV_WEAPON (261)
				{ 262, "D-pad Right" },  // INPUT_NEXT_WEAPON (262)
				{ 263, "B" },  // INPUT_MELEE_ATTACK1 (263)
				{ 264, "A" },  // INPUT_MELEE_ATTACK2 (264)
				{ 266, "Left Stick" },  // INPUT_MOVE_LEFT (266)
				{ 267, "Left Stick" },  // INPUT_MOVE_RIGHT (267)
				{ 268, "Left Stick" },  // INPUT_MOVE_UP (268)
				{ 269, "Left Stick" },  // INPUT_MOVE_DOWN (269)
				{ 270, "Right Stick" },  // INPUT_LOOK_LEFT (270)
				{ 271, "Right Stick" },  // INPUT_LOOK_RIGHT (271)
				{ 272, "Right Stick" },  // INPUT_LOOK_UP (272)
				{ 273, "Right Stick" },  // INPUT_LOOK_DOWN (273)
				{ 274, "Right Stick" },  // INPUT_SNIPER_ZOOM_IN (274)
				{ 275, "Right Stick" },  // INPUT_SNIPER_ZOOM_OUT (275)
				{ 276, "Left Stick" },  // INPUT_SNIPER_ZOOM_IN_ALTERNATE (276)
				{ 277, "Left Stick" },  // INPUT_SNIPER_ZOOM_OUT_ALTERNATE (277)
				{ 278, "Left Stick" },  // INPUT_VEH_MOVE_LEFT (278)
				{ 279, "Left Stick" },  // INPUT_VEH_MOVE_RIGHT (279)
				{ 280, "Left Stick" },  // INPUT_VEH_MOVE_UP (280)
				{ 281, "Left Stick" },  // INPUT_VEH_MOVE_DOWN (281)
				{ 282, "Right Stick" },  // INPUT_VEH_GUN_LEFT (282)
				{ 283, "Right Stick" },  // INPUT_VEH_GUN_RIGHT (283)
				{ 284, "Right Stick" },  // INPUT_VEH_GUN_UP (284)
				{ 285, "Right Stick" },  // INPUT_VEH_GUN_DOWN (285)
				{ 286, "Right Stick" },  // INPUT_VEH_LOOK_LEFT (286)
				{ 287, "Right Stick" },  // INPUT_VEH_LOOK_RIGHT (287)
				{ 288, "A" },  // INPUT_REPLAY_START_STOP_RECORDING (288)
				{ 289, "X" },  // INPUT_REPLAY_START_STOP_RECORDING_SECONDARY (289)
				{ 290, "Right Stick" },  // INPUT_SCALED_LOOK_LR (290)
				{ 291, "Right Stick" },  // INPUT_SCALED_LOOK_UD (291)
				{ 292, "Right Stick" },  // INPUT_SCALED_LOOK_UP_ONLY (292)
				{ 293, "Right Stick" },  // INPUT_SCALED_LOOK_DOWN_ONLY (293)
				{ 294, "Right Stick" },  // INPUT_SCALED_LOOK_LEFT_ONLY (294)
				{ 295, "Right Stick" },  // INPUT_SCALED_LOOK_RIGHT_ONLY (295)
				{ 296, "X" },  // INPUT_REPLAY_MARKER_DELETE (296)
				{ 297, "Y" },  // INPUT_REPLAY_CLIP_DELETE (297)
				{ 298, "A" },  // INPUT_REPLAY_PAUSE (298)
				{ 299, "LB" },  // INPUT_REPLAY_REWIND (299)
				{ 300, "RB" },  // INPUT_REPLAY_FFWD (300)
				{ 301, "A" },  // INPUT_REPLAY_NEWMARKER (301)
				{ 303, "D-pad Up" },  // INPUT_REPLAY_SCREENSHOT (303)
				{ 304, "R3" },  // INPUT_REPLAY_HIDEHUD (304)
				{ 307, "D-pad Right" },  // INPUT_REPLAY_ADVANCE (307)
				{ 308, "D-pad Left" },  // INPUT_REPLAY_BACK (308)
				{ 309, "D-pad Down" },  // INPUT_REPLAY_TOOLS (309)
				{ 310, "Back" },  // INPUT_REPLAY_RESTART (310)
				{ 311, "D-pad Down" },  // INPUT_REPLAY_SHOWHOTKEY (311)
				{ 312, "D-pad Left" },  // INPUT_REPLAY_CYCLEMARKERLEFT (312)
				{ 313, "D-pad Right" },  // INPUT_REPLAY_CYCLEMARKERRIGHT (313)
				{ 314, "RB" },  // INPUT_REPLAY_FOVINCREASE (314)
				{ 315, "LB" },  // INPUT_REPLAY_FOVDECREASE (315)
				{ 318, "Start" },  // INPUT_REPLAY_SAVE (318)
				{ 328, "RT" },  // INPUT_REPLAY_PREVIEW_AUDIO (328)
				{ 332, "Right Stick" },  // INPUT_RADIO_WHEEL_UD (332)
				{ 333, "Right Stick" },  // INPUT_RADIO_WHEEL_LR (333)
				{ 334, "Left Stick" },  // INPUT_VEH_SLOWMO_UD (334)
				{ 335, "Left Stick" },  // INPUT_VEH_SLOWMO_UP_ONLY (335)
				{ 336, "Left Stick" },  // INPUT_VEH_SLOWMO_DOWN_ONLY (336)
				{ 337, "A" },  // INPUT_VEH_HYDRAULICS_CONTROL_TOGGLE (337)
				{ 338, "Left Stick" },  // INPUT_VEH_HYDRAULICS_CONTROL_LEFT (338)
				{ 339, "Left Stick" },  // INPUT_VEH_HYDRAULICS_CONTROL_RIGHT (339)
				{ 340, "Left Stick" },  // INPUT_VEH_HYDRAULICS_CONTROL_UP (340)
				{ 341, "Left Stick" },  // INPUT_VEH_HYDRAULICS_CONTROL_DOWN (341)
				{ 342, "Left Stick" },  // INPUT_VEH_HYDRAULICS_CONTROL_UD (342)
				{ 343, "Left Stick" },  // INPUT_VEH_HYDRAULICS_CONTROL_LR (343)
				{ 344, "D-pad Right" },  // INPUT_SWITCH_VISOR (344)
				{ 345, "A" },  // INPUT_VEH_MELEE_HOLD (345)
				{ 346, "LB" },  // INPUT_VEH_MELEE_LEFT (346)
				{ 347, "RB" },  // INPUT_VEH_MELEE_RIGHT (347)
				{ 348, "Y" },  // INPUT_MAP_POI (348)
				{ 349, "X" },  // INPUT_REPLAY_SNAPMATIC_PHOTO (349)
				{ 350, "L3" },  // INPUT_VEH_CAR_JUMP (350)
				{ 351, "L3" },  // INPUT_VEH_ROCKET_BOOST (351)
				{ 352, "L3" },  // INPUT_VEH_FLY_BOOST (352)
				{ 353, "A" },  // INPUT_VEH_PARACHUTE (353)
				{ 354, "A" },  // INPUT_VEH_BIKE_WINGS (354)
				{ 355, "D-pad Right" },  // INPUT_VEH_FLY_BOMB_BAY (355)
				{ 356, "D-pad Right" },  // INPUT_VEH_FLY_COUNTER (356)
				{ 357, "A" },  // INPUT_VEH_TRANSFORM (357)
				{ 358, "RB" },  // INPUT_QUAD_LOCO_REVERSE (358)
			};
			return names;
		}

		// PlayStation physical button names for the labels that differ from the Xbox layout.
		const std::map<std::string, const char*>& PlayStationNames()
		{
			static const std::map<std::string, const char*> names =
			{
				{ "A", "Cross" },
				{ "B", "Circle" },
				{ "X", "Square" },
				{ "Y", "Triangle" },
				{ "LB", "L1" },
				{ "RB", "R1" },
				{ "LT", "L2" },
				{ "RT", "R2" },
				{ "Back", "Share" },
				{ "Start", "Options" },
			};
			return names;
		}

		}

	const std::vector<UINT16>& PhysicalAControls()
	{
		static const std::vector<UINT16> ids = [] {
			std::vector<UINT16> result;
			for (const auto& [id, name] : GamepadButtonNames())
				if (std::string(name) == "A")
					result.push_back(id);
			return result;
		}();
		return ids;
	}

	bool IsPhysicalA(UINT16 control)
	{
		for (UINT16 id : PhysicalAControls())
			if (id == control) return true;
		return false;
	}
}