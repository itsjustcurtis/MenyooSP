#include "Keybinds.h"

#include "MenuConfig.h"

#include "..\Util\ExePath.h"
#include "..\Util\FileLogger.h"

#include <pugixml/src/pugixml.hpp>

#include <string>

namespace Keybinds
{
	// ── Persistence ────────────────────────────────────────────────────

	namespace
	{
		std::string XmlPath()
		{
			return GetPathffA(Pathff::Main, true) + "Keybinds.xml";
		}

		// if missing = uses default, if out of range = uses default and logs warning
		UINT16 ReadSlotValue(const pugi::xml_node& node, const char* attributeName,
			const KeybindEntry* entry, UINT16 fallback)
		{
			const pugi::xml_attribute attribute = node.attribute(attributeName);
			if (!attribute) return fallback;
			const int value = attribute.as_int(fallback);
			if (value < -1 || value > (int)NoBind)
			{
				addlog(ige::LogType::LOG_WARNING, std::string("Keybinds.xml: ") + attributeName
					+ " value " + std::to_string(value) + " out of range for bind \"" + entry->Id()
					+ "\" - attribute ignored");
				return fallback;
			}
			return (UINT16)value;
		}

		bool ReadXml()
		{
			pugi::xml_document doc;
			pugi::xml_parse_result result = doc.load_file(XmlPath().c_str());
			if (result.status != pugi::status_ok)
			{
				if (result.status == pugi::status_file_not_found)
					addlog(ige::LogType::LOG_INFO, "Keybinds.xml not found - importing legacy binds from menyooConfig.ini");
				else
					addlog(ige::LogType::LOG_ERROR, std::string("Keybinds.xml: failed to load (") + result.description()
						+ ") - binds fell back to defaults/legacy ini import");
				return false;
			}

			pugi::xml_node root = doc.child("Keybinds");
			if (root.empty())
				addlog(ige::LogType::LOG_WARNING, "Keybinds.xml: no <Keybinds> root node - default binds in use");
			int style = root.attribute("gamepadLabels").as_int(0);
			if (style < 0 || style > 1) style = 0;
			gamepadLabelStyle = (GamepadLabelStyle)style;
			for (pugi::xml_node node : root.children("Bind"))
			{
				const char* id = node.attribute("id").value();
				const KeybindEntry* entry = FindEntry(id);
				if (entry == nullptr)
				{
					addlog(ige::LogType::LOG_WARNING, std::string("Keybinds.xml: unknown bind id \"")
						+ (id[0] != '\0' ? id : "<missing>") + "\" - entry skipped");
					continue;
				}
				entry->SetKeyboardKey1(ReadSlotValue(node, "keyboard", entry, entry->KeyboardKey1()));
				entry->SetKeyboardKey2(ReadSlotValue(node, "keyboard2", entry, entry->KeyboardKey2()));
				entry->SetGamepadButton1(ReadSlotValue(node, "pad1", entry, entry->GamepadButton1()));
				entry->SetGamepadButton2(ReadSlotValue(node, "pad2", entry, entry->GamepadButton2()));
			}
			return true;
		}

		void WriteXml()
		{
			pugi::xml_document doc;
			auto decl = doc.append_child(pugi::node_declaration);
			decl.append_attribute("version") = "1.0";
			decl.append_attribute("encoding") = "UTF-8";
			auto root = doc.append_child("Keybinds");
			root.append_attribute("gamepadLabels") = (int)gamepadLabelStyle;
			for (const auto& entry : Registry())
			{
				auto node = root.append_child("Bind");
				node.append_attribute("id") = entry.Id().c_str();
				node.append_attribute("keyboard") = (int)entry.KeyboardKey1();
				node.append_attribute("keyboard2") = (int)entry.KeyboardKey2();
				node.append_attribute("pad1") = (int)entry.GamepadButton1();
				node.append_attribute("pad2") = (int)entry.GamepadButton2();
			}
			if (!doc.save_file(XmlPath().c_str()))
				addlog(ige::LogType::LOG_ERROR, "Keybinds.xml: failed to save - keybind changes will NOT persist (file locked or read-only?)");
		}

		// convert old menyooConfig.ini keybinds to Keybinds.xml format, if present
		void ImportFromIni()
		{
			auto& ini = MenuConfig::iniFile;
			struct LegacyKey { const char* section; const char* key; const char* id; UINT16 slot; };
			const LegacyKey legacy[] =
			{
				{ "settings", "open_key", "menu_open", 0 },
				{ "settings", "open_button_for_gamepad_1", "menu_open", 1 },
				{ "settings", "open_button_for_gamepad_2", "menu_open", 2 },
				{ "settings", "manual_respawn_button", "respawn", 1 },
				{ "settings", "stop_animation_key", "stop_animation", 0 },
				{ "general", "FreeCamButton", "no_clip", 0 },
				{ "object-spooner", "SpoonerModeHotkey", "spooner_mode", 0 },
				{ "object-spooner", "SpoonerModeGamepadBind_1", "spooner_mode", 1 },
				{ "object-spooner", "SpoonerModeGamepadBind_2", "spooner_mode", 2 },
				{ "settings", "tp_to_wp_key", "teleport_waypoint", 0 },
				{ "settings", "tp_to_wp_pad", "teleport_waypoint", 1 },
				{ "general", "tow_key", "tow_rope", 0 },
				{ "general", "tow_pad", "tow_rope", 1 },
				{ "settings", "category_nav_key", "category_jump", 0 },
				{ "settings", "category_nav_pad", "category_jump", 1 },
				{ "object-spooner", "SpoonerMarkerHotkey", "spooner_marker", 0 },
				{ "object-spooner", "SpoonerMarkerPad", "spooner_marker", 1 },
				{ "general", "EntityDeleteHotkey", "menu_action", 0 },
				{ "object-spooner", "SpoonerEditModeHotkey", "spooner_edit_mode", 0 },
				{ "object-spooner", "SpoonerEditCopyHotkey", "spooner_edit_copy", 0 },
				{ "object-spooner", "SpoonerEditTransformHotkey", "spooner_edit_transform", 0 },
				{ "object-spooner", "SpoonerEditCamLockHotkey", "spooner_camera_lock", 0 },
				{ "object-spooner", "SpoonerEditLocalSpaceHotkey", "spooner_local_space", 0 },
				{ "general", "FreeCamPad_1", "no_clip", 1 },
				{ "general", "FreeCamPad_2", "no_clip", 2 },
			};
			for (const auto& item : legacy)
			{
				const KeybindEntry* entry = FindEntry(item.id);
				if (entry == nullptr)
				{
					addlog(ige::LogType::LOG_WARNING, std::string("Legacy ini import: [") + item.section + "] "
						+ item.key + " references unknown bind id \"" + item.id + "\" - skipped");
					continue;
				}
				LONG value = ini.GetLongValue(item.section, item.key, -1);
				if (value < 0) continue; // unset key: keep the registry default
				if (value > (LONG)NoBind)
				{
					addlog(ige::LogType::LOG_WARNING, std::string("Legacy ini import: [") + item.section + "] "
						+ item.key + " value " + std::to_string(value) + " out of range for bind \""
						+ item.id + "\" - kept default");
					continue;
				}
				// skips keyboard2 slot because legacy ini only had one keyboard key per bind
				switch (item.slot)
				{
					case 0: entry->SetKeyboardKey1((UINT16)value); break;
					case 1: entry->SetGamepadButton1((UINT16)value); break;
					case 2: entry->SetGamepadButton2((UINT16)value); break;
				}
			}
			WriteXml();
		}
	}

	void Load()
	{
		if (ReadXml())
			addlog(ige::LogType::LOG_INFO, "Keybinds loaded from Keybinds.xml");
		else
		{
			ImportFromIni();
			addlog(ige::LogType::LOG_INFO, "Keybinds imported from menyooConfig.ini. Keybinds.xml created.");
		}
	}

	void Save()
	{
		WriteXml();
	}
}