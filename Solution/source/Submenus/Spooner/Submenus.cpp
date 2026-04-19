/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "Submenus.h"

#include "..\..\macros.h"

#include "..\..\Menu\Menu.h"
#include "..\..\Menu\Routine.h"

#include "..\..\Natives\natives2.h"
#include "..\..\Scripting\GTAentity.h"
#include "..\..\Scripting\GTAprop.h"
#include "..\..\Scripting\GTAvehicle.h"
#include "..\..\Scripting\GTAped.h"
#include "..\..\Util\GTAmath.h"
#include "..\..\Scripting\ModelNames.h"
#include "..\..\Scripting\Game.h"
#include "..\..\Scripting\World.h"
#include "..\..\Memory\GTAmemory.h"
#include "..\..\Util\ExePath.h"
#include "..\..\Util\StringManip.h"
#include "..\..\Menu\FolderPreviewBmps.h"
#include "..\..\Scripting\GTAblip.h"
#include "..\..\Scripting\TimecycleModification.h"
#include "..\..\Scripting\Camera.h"
#include "..\..\Util\keyboard.h"
#include "..\..\Scripting\WeaponIndivs.h"

#include "STSTask.h"
#include "STSTasks.h"
#include "SpoonerEntity.h"
#include "SpoonerMode.h"
#include "SpoonerSettings.h"
#include "Databases.h"
#include "FileManagement.h"
#include "EntityManagement.h"
#include "MarkerManagement.h"
#include "FavouritesManagement.h"
#include "MenuOptions.h"
#include "SpoonerMarker.h"
#include "..\..\Submenus\PedComponentChanger.h"
#include "..\..\Submenus\Settings.h"
#include "..\..\Submenus\PedModelChanger.h"
#include "..\..\Submenus\VehicleSpawner.h"
#include "..\..\Submenus\PedAnimation.h"
#include "..\..\Submenus\PedSpeech.h"
#include "..\..\Submenus\PtfxSubs.h"
#include "..\..\Submenus\FunnyVehicles.h"
#include "..\..\Util\FileLogger.h"

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <string>
#include <tuple>
#include <vector>
#include <array>
#include <pugixml/src/pugixml.hpp>
#include <dirent\include\dirent.h>

namespace sub
{
	namespace Spooner::Submenus
	{
		std::string& _searchStr = dict2;
		std::tuple<GTAentity, Vector3*, Vector3*> SpoonerVector3ManualPlacementPtrs = { 0, nullptr, nullptr };
		float _manualPlacementPrecision = 0.01f;
		float _fSaveRangeRadius = 5.0f;
		UINT8 _copyEntTexterValue = 0;
		UINT8 _entTypeToShowTexterValue = 0;
		void SetEnt241() { g_Ped1 = selectedEntity.handle.Handle(); }
		void SetEnt12() { g_Ped4 = selectedEntity.handle.Handle(); }

		void Sub_SpoonerMain()
		{
			selectedEntity.handle = 0;
			_searchStr.clear(); // Sub_SaveFiles _searchStr
			dict3.clear(); // Sub_SaveFiles _dir

			AddTitle("Object Spooner");

			AddLocal("Spooner Mode", SpoonerMode::bEnabled, SpoonerMode::Toggle, SpoonerMode::Toggle);
			AddOption("Spawn Entity Into World", null, nullFunc, SUB::SPOONER_SPAWN_CATEGORIES);
			AddOption("Manage Entity Database", null, nullFunc, SUB::SPOONER_MANAGEDB);
			AddOption("Manage Markers", null, nullFunc, SUB::SPOONER_MANAGEMARKERS);
			AddOption("Manage Saved Files", null, nullFunc, SUB::SPOONER_SAVEFILES);
			AddOption("Quick Manual Placement (Legacy)", null, nullFunc, SUB::SPOONER_QUICKMANUALPLACEMENT);
			AddOption("Edit Multiple Entities Simultaneously", null, nullFunc, SUB::SPOONER_GROUPSPOON);
			AddOption("Settings", null, nullFunc, SUB::SPOONER_SETTINGS);
		}
		void Sub_Settings()
		{
			bool bSmm_plus = false, bSmm_minus = false;
			bool movsensK_input = 0, movsensK_plus = 0, movsensK_minus = 0;
			bool movsensG_input = 0, movsensG_plus = 0, movsensG_minus = 0;
			bool rotsensK_input = 0, rotsensK_plus = 0, rotsensK_minus = 0;
			bool rotsensG_input = 0, rotsensG_plus = 0, rotsensG_minus = 0;

			AddTitle("Settings");
			AddToggle("Display Model Previews (Spooner Mode)", Settings::bShowModelPreviews);
			AddToggle("Display Entity Surrounding Box", Settings::bShowBoxAroundSelectedEntity);
			AddToggle("Spawn Dynamic Objects", Settings::bSpawnDynamicProps);
			AddToggle("Spawn Dynamic Peds", Settings::bSpawnDynamicPeds);
			AddToggle("Spawn Dynamic Vehicles", Settings::bSpawnDynamicVehicles);
			AddToggle("Freeze Entity When Moving It (Spooner Mode)", Settings::bFreezeEntityWhenMovingIt);
			AddToggle("Spawn Invincible Entities", Settings::bSpawnInvincibleEntities);
			AddToggle("Spawn Still Peds (Block Fleeing)", Settings::bSpawnStillPeds);
			AddToggle("Make Added (To DB) Entities Persistent", Settings::bAddToDbAsMissionEntities);
			AddToggle("Teleport To Reference When Loading File", Settings::bTeleportToReferenceWhenLoadingFile);
			AddTexter("Spooner Mode Method", static_cast<UINT8>(Settings::spoonerModeMode), spoonerModeModeNames, null, bSmm_plus, bSmm_minus);

			if (Menu::bitController)
			{
				AddNumber("Movement Sensitivity (Gamepad)", Settings::cameraMovementSensitivityGamepad, 4, movsensG_input, movsensG_plus, movsensG_minus);
				AddNumber("Rotation Sensitivity (Gamepad)", Settings::cameraRotationSensitivityGamepad, 4, rotsensG_input, rotsensG_plus, rotsensG_minus);
			}
			else
			{
				AddNumber("Movement Sensitivity (Keyboard)", Settings::cameraMovementSensitivityKeyboard, 4, movsensK_input, movsensK_plus, movsensK_minus);
				AddNumber("Rotation Sensitivity (Mouse)", Settings::cameraRotationSensitivityMouse, 4, rotsensK_input, rotsensK_plus, rotsensK_minus);
			}

			AddOption("Reload Model List Files", null, PopulateGlobalEntityModelsArrays);

			if (bSmm_plus) { if ((UINT8)Settings::spoonerModeMode < spoonerModeModeNames.size() - 1) Settings::spoonerModeMode = eSpoonerModeMode((UINT8)Settings::spoonerModeMode + 1); }
			if (bSmm_minus) { if ((UINT8)Settings::spoonerModeMode > 0) Settings::spoonerModeMode = eSpoonerModeMode((UINT8)Settings::spoonerModeMode - 1); }

			if (movsensK_input || movsensG_input)
			{
				float& val = (movsensK_input ? Settings::cameraMovementSensitivityKeyboard : Settings::cameraMovementSensitivityGamepad);
				std::string inputStr = Game::InputBox("", 11U, "", std::to_string(val).substr(0, 10));
				if (inputStr.length() > 0)
				{
					try { val = stof(inputStr); }
					catch (...) { Game::Print::PrintErrorInvalidInput(inputStr); }
				}
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SetArg1Float, std::string(), 10U, std::to_string(val).substr(0, 10));
				//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&val);
			}
			if (movsensK_plus || movsensG_plus)
			{
				float& val = (movsensK_plus ? Settings::cameraMovementSensitivityKeyboard : Settings::cameraMovementSensitivityGamepad);
				if (val < FLT_MAX)
					val += (movsensK_minus ? 0.0005f : 0.0005f);
			}
			if (movsensK_minus || movsensG_minus)
			{
				float& val = (movsensK_minus ? Settings::cameraMovementSensitivityKeyboard : Settings::cameraMovementSensitivityGamepad);
				if (val > 0.0f)
					val -= (movsensK_minus ? 0.0005f : 0.0005f);
			}

			if (rotsensK_input || rotsensG_input)
			{
				float& val = (rotsensK_input ? Settings::cameraRotationSensitivityMouse : Settings::cameraRotationSensitivityGamepad);
				std::string inputStr = Game::InputBox("", 11U, "", std::to_string(val).substr(0, 10));
				if (inputStr.length() > 0)
				{
					try { val = stof(inputStr); }
					catch (...) { Game::Print::PrintErrorInvalidInput(inputStr); }
				}
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SetArg1Float, std::string(), 10U, std::to_string(val).substr(0, 10));
				//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&val);
			}
			if (rotsensK_plus || rotsensG_plus)
			{
				bool isK = rotsensK_plus;
				float& val = (rotsensK_plus ? Settings::cameraRotationSensitivityMouse : Settings::cameraRotationSensitivityGamepad);
				if (val < FLT_MAX)
					val += (rotsensK_plus ? 0.0005f : 0.0005f);
			}
			if (rotsensK_minus || rotsensG_minus)
			{
				float& val = (rotsensK_minus ? Settings::cameraRotationSensitivityMouse : Settings::cameraRotationSensitivityGamepad);
				if (val > 0.0f)
					val -= (rotsensK_minus ? 0.0005f : 0.0005f);
			}

		}
		void Sub_SaveFiles()
		{
			std::string& _name = dict;
			std::string& _dir = dict3;

			GTAentity myPed = PLAYER_PED_ID();
			const Vector3& myPos = myPed.GetPosition();
			auto& fSaveRangeRadius = _fSaveRangeRadius;

			AddTitle("Manage Saved Files");

			bool bSaveDb = false;
			AddOption("Save Database To File (" + std::to_string(Databases::EntityDb.size()) + ")", bSaveDb); if (bSaveDb)
			{
				std::string inputStr = Game::InputBox("", 28U, "Enter file name:");
				if (inputStr.length() > 0)
				{
					if (FileManagement::SaveDbToFile(_dir + "\\" + inputStr + ".xml", true))
					{
						Game::Print::PrintBottomLeft("File ~b~saved~s~.");
					}
					else
					{
						Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to save file.");
						addlog(ige::LogType::LOG_ERROR, "Attempt to save Database file " + inputStr + ".xml failed");
					}
				}
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SpoonerSaveDbToFile, std::string(), 28U, "Enter file name:");
				//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&_dir);
			}

			bool bSaveWorld = false;
			AddOption("Save World To File (" + std::to_string(worldEntities.size()) + ")", bSaveWorld); if (bSaveWorld)
			{
				std::string inputStr = Game::InputBox("", 28U, "Enter file name:");
				if (inputStr.length() > 0)
				{
					if (FileManagement::SaveWorldToFile(_dir + "\\" + inputStr + ".xml", worldEntities, Databases::MarkerDb))
					{
						Game::Print::PrintBottomLeft("File ~b~saved~s~.");
					}
					else
					{
						Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to save file.");
						addlog(ige::LogType::LOG_ERROR, "Attempt to save World file " + inputStr + ".xml failed");
					}
				}
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SpoonerSaveWorldToFile, std::string(), 28U, "Enter file name:");
				//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&_dir);
			}

			std::vector<Entity> vSaveRangeEntities;
			GTAmemory::GetEntityHandles(vSaveRangeEntities, myPos, fSaveRangeRadius);
			bool bSaveRange_plus = false, bSaveRange_minus = false, bSaveRange_save = false;
			AddNumber("Save Range To File (" + std::to_string(vSaveRangeEntities.size()) + ")", fSaveRangeRadius, 0, bSaveRange_save, bSaveRange_plus, bSaveRange_minus);
			if (*Menu::currentopATM == Menu::printingop)
				EntityManagement::DrawRadiusDisplayingMarker(myPos, fSaveRangeRadius);
			if (bSaveRange_plus) { if (fSaveRangeRadius < FLT_MAX) fSaveRangeRadius += 1.0f; }
			if (bSaveRange_minus) { if (fSaveRangeRadius > 0.0f) fSaveRangeRadius -= 1.0f; }
			if (bSaveRange_save)
			{
				std::string inputStr = Game::InputBox("", 28U, "Enter file name:");
				if (inputStr.length() > 0)
				{
					std::vector<SpoonerMarker> vSaveRangeMarkers;
					MarkerManagement::GetAllMarkersInRange(vSaveRangeMarkers, myPos, fSaveRangeRadius);

					if (FileManagement::SaveWorldToFile(_dir + "\\" + inputStr + ".xml", vSaveRangeEntities, vSaveRangeMarkers))
					{
						Game::Print::PrintBottomLeft("File ~b~saved~s~.");
					}
					else
					{
						Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to save file.");
						addlog(ige::LogType::LOG_ERROR, "Attempt to save Range Markers file " + inputStr + ".xml failed");
					}
				}
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SpoonerSaveRangeToFile, std::string(), 28U, "Enter file name:");
				//OnscreenKeyboard::State::arg1._float = fSaveRangeRadius;
				//OnscreenKeyboard::State::arg2._vec3 = new Vector3(myPos);
				//OnscreenKeyboard::State::arg3._ptr = reinterpret_cast<void*>(&_dir);
			}

			/*bool bLoadFromFile = false;
			AddOption("Load From File", bLoadFromFile); if (bLoadFromFile)
			{
			std::string inputStr = Game::InputBox("", 28U, "Enter file name:");
			if (inputStr.length() > 0)
			{
			if (FileManagement::Exists(_dir + "\\" + inputStr + ".xml"))
			{
			_name = inputStr;
			Menu::SetSub_delayed = SUB::SPOONER_SAVEFILES_LOAD;
			}
			else
			{
			Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to load file.");
			}
			}
			//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SpoonerLoadFromFile, std::string(), 28U, "Enter file name:");
			//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&_name);
			//OnscreenKeyboard::State::arg2._ptr = reinterpret_cast<void*>(&_dir);
			}*/

			std::vector<std::string> vfilnames;
			//get_all_filenames_with_extension(GetPathffA(Pathff::Spooner, false), ".xml", vfilnames, false);
			if (_dir.empty())
				_dir = GetPathffA(Pathff::Spooner, false);
			DIR* dir_point = opendir(_dir.c_str());
			dirent* entry = readdir(dir_point);
			while (entry)
			{
				vfilnames.push_back(entry->d_name);
				entry = readdir(dir_point);
			}
			closedir(dir_point);

			bool bCreateFolderPressed = false;
			AddOption("Create New Folder", bCreateFolderPressed); if (bCreateFolderPressed)
			{
				std::string inputStr = Game::InputBox("", 28U, "Enter folder name:");
				if (inputStr.length() > 0)
				{
					if (CreateDirectoryA((_dir + "\\" + inputStr).c_str(), NULL) ||
						GetLastError() == ERROR_ALREADY_EXISTS)
					{
						_dir = _dir + "\\" + inputStr;
						Menu::currentop = 6;
						Game::Print::PrintBottomLeft("Folder ~b~created~s~.");
					}
					else
					{
						Game::Print::PrintBottomCentre("~r~Failed~s~ to create folder.");
						addlog(ige::LogType::LOG_ERROR, "Attempt to create new folder " + inputStr + " failed");
					}
				}
				else
					Game::Print::PrintErrorInvalidInput(inputStr);
				return;
				// No OnscreenKeyboard!
			}

			AddBreak("---Found Files---");

			bool bFolderBackPressed = false;
			AddOption("..", bFolderBackPressed); if (bFolderBackPressed)
			{
				_dir = _dir.substr(0, _dir.rfind("\\"));
				Menu::currentop = 6;
			}

			if (!vfilnames.empty())
			{
				bool bSearchPressed = false;
				AddOption(_searchStr.empty() ? "SEARCH" : _searchStr, bSearchPressed, nullFunc, -1, true); if (bSearchPressed)
				{
					_searchStr = Game::InputBox(_searchStr, 126U, "SEARCH", boost::to_lower_copy(_searchStr));
					boost::to_upper(_searchStr);
					//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SearchToUpper, _searchStr, 126U, std::string(), _searchStr);
					//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&_searchStr);
				}

				for (auto& filname : vfilnames)
				{
					if (filname.front() == '.' || filname.front() == ',')
						continue;
					if (!_searchStr.empty()) { if (boost::to_upper_copy(filname).find(_searchStr) == std::string::npos) continue; }

					bool isFolder = PathIsDirectoryA((_dir + "\\" + filname).c_str()) != 0;
					bool isXml = filname.length() > 4 && filname.rfind(".xml") == filname.length() - 4;
					bool isSp00n = filname.length() > 6 && filname.rfind(".SP00N") == filname.length() - 6;
					TICKOL icon = TICKOL::NONE;
					if (isFolder)
						icon = TICKOL::ARROWRIGHT;
					else if (isXml || isSp00n)
						icon = TICKOL::TICK2;
					bool bFilePressed = false;

					if (isFolder)
					{
						AddTickol(filname + " >>>", true, bFilePressed, bFilePressed, icon, TICKOL::NONE); if (bFilePressed)
						{
							_dir = _dir + "\\" + filname;
							Menu::currentop = 6;
						}

						if (Menu::printingop == *Menu::currentopATM && !bFilePressed)
						{
							if (FolderPreviewBmps_catind::bFolderBmpsEnabled)
								FolderPreviewBmps_catind::DrawBmp(_dir + "\\" + filname);
						}
					}
					else if (isXml || isSp00n)
					{
						AddTickol(filname, true, bFilePressed, bFilePressed, icon, TICKOL::NONE); if (bFilePressed)
						{
							if (isXml)
							{
								_name = filname.substr(0, filname.rfind('.'));
								Menu::SetSub_delayed = SUB::SPOONER_SAVEFILES_LOAD;
							}
							else if (isSp00n)
							{
								_name = filname.substr(0, filname.rfind('.'));
								Menu::SetSub_delayed = SUB::SPOONER_SAVEFILES_LOAD_LEGACYSP00N;
							}
						}
					}
				}
			}

			/*vfilnames.clear();
			get_all_filenames_with_extension(GetPathffA(Pathff::Spooner, false), ".SP00N", vfilnames, false);
			if (!vfilnames.empty())
			{
			AddBreak("---Legacy SP00N Files---");

			for (auto& filname : vfilnames)
			{
			bool bSearchPressed = false;
			AddOption(_searchStr.empty() ? "SEARCH" : _searchStr, bSearchPressed, nullFunc, -1, true); if (bSearchPressed)
			{
			_searchStr = Game::InputBox(_searchStr, 126U, "SEARCH", boost::to_lower_copy(_searchStr));
			boost::to_upper(_searchStr);
			//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SearchToUpper, _searchStr, 126U, std::string(), _searchStr);
			//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&_searchStr);
			}

			if (!_searchStr.empty()) { if (boost::to_upper_copy(filname).find(_searchStr) == std::string::npos) continue; }

			bool bFilePressed = false;
			AddOption(filname, bFilePressed); if (bFilePressed)
			{
			if (FileManagement::Exists(filname, ".SP00N"))
			{
			_name = filname;
			Menu::SetSub_delayed = SUB::SPOONER_SAVEFILES_LOAD_LEGACYSP00N;
			}
			}
			}
			}*/

		}
		void Sub_SaveFiles_Load()
		{
			std::string& _name = dict;
			std::string& _dir = dict3;
			std::string filePath = _dir + "\\" + _name + ".xml";

			AddTitle(_name);

			bool bTeleToRef = false;
			AddOption("Teleport To Reference", bTeleToRef); if (bTeleToRef)
			{
				FileManagement::TeleportToReference(filePath);
			}

			bool bLoadPlacements = false;
			AddOption("Load Placements", bLoadPlacements); if (bLoadPlacements)
			{
				if (FileManagement::LoadPlacementsFromFile(filePath))
				{
					Game::Print::PrintBottomLeft("File ~b~loaded~s~.");
				}
				else
				{
					Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to load file.");
					addlog(ige::LogType::LOG_ERROR, "Attempt to load placements from " + filePath + ".xml failed");
				}
			}

			bool bRenameFile = false;
			AddOption("Rename File", bRenameFile); if (bRenameFile)
			{
				std::string inputStr = Game::InputBox("", 28U, "Enter new name:", _name);
				if (inputStr.length() > 0)
				{
					if (rename(filePath.c_str(), (_dir + "\\" + inputStr + ".xml").c_str()) == 0)
					{
						_name = inputStr;
						Game::Print::PrintBottomLeft("File ~b~renamed~s~.");
					}
					else
					{
						Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to rename file.");
						addlog(ige::LogType::LOG_ERROR, "Attempt to rename file " + _name + ".xml to " + inputStr + " failed");
					}
				}
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::RenameSpoonerFile, std::string(), 28U, "Enter new name:", fileName);
				//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&_name);
				//OnscreenKeyboard::State::arg2._ptr = reinterpret_cast<void*>(&_dir);
			}

			bool vOverwriteFile = false;
			AddOption("Overwrite Placements In File (With DB)", vOverwriteFile); if (vOverwriteFile)
			{
				if (FileManagement::SaveDbToFile(filePath, false))
				{
					Game::Print::PrintBottomLeft("File ~b~overwritten~s~. Extra settings kept.");
				}
				else
				{
					Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to overwrite file.");
					addlog(ige::LogType::LOG_ERROR, "Attempt to overwrite " + filePath + " failed");
				}
			}

			bool bDeleteFile = false;
			AddOption("Delete File", bDeleteFile); if (bDeleteFile)
			{
				if (remove(filePath.c_str()) == 0)
				{
					Game::Print::PrintBottomLeft("File ~b~deleted~s~.");
				}
				else
				{
					Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to delete file.");
					addlog(ige::LogType::LOG_ERROR, "Attempt to delete file " + filePath + " failed");
				}
				Menu::SetPreviousMenu();
				Menu::Up();
				return;
			}

			//=============================================================================
			pugi::xml_document doc;
			if (doc.load_file((const char*)filePath.c_str()).status == pugi::status_ok)
			{
				AddBreak("---Attributes---");
				auto nodeRoot = doc.child("SpoonerPlacements"); // Root

				auto nodeNote = nodeRoot.child("Note");
				if (nodeNote)
				{
					std::string noteStr = nodeNote.text().as_string();
					bool bEditNotePressed = false;
					AddTexter("Note", 0, std::vector<std::string>{noteStr.length() > 0 ? (noteStr.length() < 10 ? noteStr : noteStr.substr(0, 10) + "...") : "~italic~None"}, bEditNotePressed); if (bEditNotePressed)
					{
						std::string inputStr = Game::InputBox("~`", 300U, "Enter note:", noteStr);
						if (inputStr.compare("~`") != 0)
						{
							noteStr = inputStr;
							nodeNote.text() = noteStr.c_str();
							doc.save_file((const char*)filePath.c_str());
						}
						//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SpoonerFileNote, "~`", 300U, "Enter note:", noteStr);
						//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&fileName);
					}
				}

				auto nodeAudioFile = nodeRoot.child("AudioFile");
				if (nodeAudioFile)
				{
					std::string audioFileName = nodeAudioFile.text().as_string();
					bool bEditAudioFileNamePressed = false;
					AddTexter("Audio File To Play", 0, std::vector<std::string>{audioFileName.length() > 0 ? audioFileName : "~italic~None"}, bEditAudioFileNamePressed); if (bEditAudioFileNamePressed)
					{
						std::string inputStr = Game::InputBox("~`", 64U, "Enter filename with extension (file should be in menyooStuff\\Audio):", audioFileName);
						if (inputStr.compare("~`") != 0)
						{
							audioFileName = inputStr;
							nodeAudioFile.text() = audioFileName.c_str();
							doc.save_file((const char*)filePath.c_str());
						}
						//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SpoonerFileAudioFile, "~`", 64U, "Enter filename with extension (file should be in menyooStuff\\Audio):", audioFileName);
						//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&fileName);
					}
				}

				auto nodeStartTaskSeqOnLoad = nodeRoot.child("StartTaskSequencesOnLoad");
				if (nodeStartTaskSeqOnLoad)
				{
					bool bToggleStartTaskSeqOnLoadPressed = false;
					AddTickol("Start Task Sequences Immediately", nodeStartTaskSeqOnLoad.text().as_bool(), bToggleStartTaskSeqOnLoadPressed, bToggleStartTaskSeqOnLoadPressed, TICKOL::BOXTICK, TICKOL::BOXBLANK); if (bToggleStartTaskSeqOnLoadPressed)
					{
						nodeStartTaskSeqOnLoad.text() = !nodeStartTaskSeqOnLoad.text().as_bool();
						doc.save_file((const char*)filePath.c_str());
					}
				}

				auto nodeClearDatabase = nodeRoot.child("ClearDatabase");
				if (nodeClearDatabase)
				{
					bool bToggleClearDatabasePressed = false;
					AddTickol("Delete Database Entities", nodeClearDatabase.text().as_bool(), bToggleClearDatabasePressed, bToggleClearDatabasePressed, TICKOL::BOXTICK, TICKOL::BOXBLANK); if (bToggleClearDatabasePressed)
					{
						nodeClearDatabase.text() = !nodeClearDatabase.text().as_bool();
						doc.save_file((const char*)filePath.c_str());
					}
				}

				auto nodeClearMarkers = nodeRoot.child("ClearMarkers");
				if (nodeClearMarkers)
				{
					bool bToggleClearMarkersPressed = false;
					AddTickol("Delete Database Markers", nodeClearMarkers.text().as_bool(), bToggleClearMarkersPressed, bToggleClearMarkersPressed, TICKOL::BOXTICK, TICKOL::BOXBLANK); if (bToggleClearMarkersPressed)
					{
						nodeClearMarkers.text() = !nodeClearMarkers.text().as_bool();
						doc.save_file((const char*)filePath.c_str());
					}
				}

				Vector3 refCoords, imgLoadingCoords;
				auto nodeReferenceCoords = nodeRoot.child("ReferenceCoords");
				auto nodeImgLoadingCoords = nodeRoot.child("ImgLoadingCoords");
				for (auto& nas : std::vector<std::pair<Vector3*, pugi::xml_node*>>
					{
						{ &refCoords, &nodeReferenceCoords },
						{ &imgLoadingCoords, &nodeImgLoadingCoords }
					})
				{
					if (*nas.second)
					{
						nas.first->x = nas.second->child("X").text().as_float();
						nas.first->y = nas.second->child("Y").text().as_float();
						nas.first->z = nas.second->child("Z").text().as_float();

						EntityManagement::DrawRadiusDisplayingMarker(*nas.first, 0.5f, RGBA(0, 102, 204, 130));
						Vector2 scrnPos;
						if (World::WorldToScreen(*nas.first, scrnPos))
						{
							Game::Print::SetupDraw(GTAfont::Impact, Vector2(0.3f, 0.3f), true, false, true);
							Game::Print::drawstring(nas.second->name(), scrnPos.x, scrnPos.y);
						}
					}
				}

				auto nodeClearWorld = nodeRoot.child("ClearWorld");
				if (nodeClearDatabase && nodeReferenceCoords)
				{
					float clearWorldRadius = nodeClearWorld.text().as_float();
					bool clearWorld_plus = false, clearWorld_minus = false;
					AddNumber("Delete World Entities (Within Radius)", clearWorldRadius, 0, null, clearWorld_plus, clearWorld_minus);
					if (*Menu::currentopATM == Menu::printingop)
						EntityManagement::DrawRadiusDisplayingMarker(refCoords, clearWorldRadius, RGBA(255, 0, 0, 130));
					if (clearWorld_plus) { if (clearWorldRadius < FLT_MAX) { clearWorldRadius += 1.0f; nodeClearWorld.text() = abs(clearWorldRadius); doc.save_file((const char*)filePath.c_str()); } }
					if (clearWorld_minus) { if (clearWorldRadius > 0.0f) { clearWorldRadius -= 1.0f; nodeClearWorld.text() = abs(clearWorldRadius); doc.save_file((const char*)filePath.c_str()); } }
				}

				for (auto& nas : std::vector<std::tuple<std::string, Vector3*, pugi::xml_node*, std::string>>
					{
						std::make_tuple("Reference Coordinates", &refCoords, &nodeReferenceCoords, "ReferenceCoords"),
						std::make_tuple("CD-Image Loading Coordinates", &imgLoadingCoords, &nodeImgLoadingCoords, "ImgLoadingCoords")
					})
				{
					auto& xNode = *std::get<2>(nas);
					AddBreak(std::get<0>(nas));
					AddOption("~italic~" + (xNode ? std::get<1>(nas)->ToString() : "Not Set"), null);

					auto& spoocam = SpoonerMode::spoonerModeCamera;
					if (!spoocam.IsActive())
					{
						bool bSetPosToMe = false;
						AddOption("Set To Player Position", bSetPosToMe); if (bSetPosToMe)
						{
							Vector3 myPos = GTAentity(PLAYER_PED_ID()).GetPosition();
							*std::get<1>(nas) = myPos;
							if (!xNode)
							{
								xNode = nodeRoot.append_child(std::get<3>(nas).c_str());
								xNode.append_child("X");
								xNode.append_child("Y");
								xNode.append_child("Z");
							}
							xNode.child("X").text() = std::get<1>(nas)->x;
							xNode.child("Y").text() = std::get<1>(nas)->y;
							xNode.child("Z").text() = std::get<1>(nas)->z;
							doc.save_file((const char*)filePath.c_str());
						}
					}
					else
					{
						bool bSetPosToHitCoords = false;
						AddOption("Set To Camera Target", bSetPosToHitCoords); if (bSetPosToHitCoords)
						{
							Vector3 hitCoords = spoocam.RaycastForCoord(Vector2(0.0f, 0.0f), 0, 160.0f, 3.0f);
							*std::get<1>(nas) = hitCoords;
							if (!xNode)
							{
								xNode = nodeRoot.append_child(std::get<3>(nas).c_str());
								xNode.append_child("X");
								xNode.append_child("Y");
								xNode.append_child("Z");
							}
							xNode.child("X").text() = std::get<1>(nas)->x;
							xNode.child("Y").text() = std::get<1>(nas)->y;
							xNode.child("Z").text() = std::get<1>(nas)->z;
							doc.save_file((const char*)filePath.c_str());
						}
					}
					if (IS_WAYPOINT_ACTIVE())
					{
						bool bSetPosToWp = false;
						AddOption("Set To Waypoint", bSetPosToWp); if (bSetPosToWp)
						{
							GTAblip wpBlip = GET_FIRST_BLIP_INFO_ID(BlipIcon::Waypoint);
							Vector3 wpCoords = wpBlip.GetPosition();
							wpCoords.z = World::GetGroundHeight(wpCoords);
							*std::get<1>(nas) = wpCoords;
							if (!xNode)
							{
								xNode = nodeRoot.append_child(std::get<3>(nas).c_str());
								xNode.append_child("X");
								xNode.append_child("Y");
								xNode.append_child("Z");
							}
							xNode.child("X").text() = std::get<1>(nas)->x;
							xNode.child("Y").text() = std::get<1>(nas)->y;
							xNode.child("Z").text() = std::get<1>(nas)->z;
							doc.save_file((const char*)filePath.c_str());
						}
					}
				}

				auto nodeTimecycMod = nodeRoot.child("TimecycleModifier");
				if (nodeTimecycMod)
				{
					AddBreak("Vision Hax");

					std::string timecycModStr = nodeTimecycMod.text().as_string();
					bool bTimecycMod_pressed = false, bTimecycMod_plus = false, bTimecycMod_minus = false;
					AddTexter("Timecycle Mod", 0, std::vector<std::string>{timecycModStr.length() ? timecycModStr : "None"}, bTimecycMod_pressed, bTimecycMod_plus, bTimecycMod_minus);
					if (bTimecycMod_plus)
					{
						auto tit = std::find_if(TimecycleModification::vTimecycles.begin(), TimecycleModification::vTimecycles.end(),
							[timecycModStr](const std::pair<std::string, std::string>& item) {
							return item.first.compare(timecycModStr) == 0;
						});
						if (tit == TimecycleModification::vTimecycles.end())
						{
							timecycModStr = TimecycleModification::vTimecycles.front().first;
							nodeTimecycMod.text() = timecycModStr.c_str();
							doc.save_file((const char*)filePath.c_str());

						}
						else
						{
							++tit;
							if (tit != TimecycleModification::vTimecycles.end())
							{
								timecycModStr = tit->first;
								nodeTimecycMod.text() = timecycModStr.c_str();
								doc.save_file((const char*)filePath.c_str());
							}
						}

					}
					else if (bTimecycMod_minus)
					{
						auto tit = std::find_if(TimecycleModification::vTimecycles.begin(), TimecycleModification::vTimecycles.end(),
							[timecycModStr](const std::pair<std::string, std::string>& item) {
							return item.first.compare(timecycModStr) == 0;
						});
						if (tit == TimecycleModification::vTimecycles.begin())
						{
							timecycModStr.clear();
							nodeTimecycMod.text() = timecycModStr.c_str();
							doc.save_file((const char*)filePath.c_str());
						}
						else if (tit != TimecycleModification::vTimecycles.end())
						{
							--tit;
							timecycModStr = tit->first;
							nodeTimecycMod.text() = timecycModStr.c_str();
							doc.save_file((const char*)filePath.c_str());
						}
					}
					else if (bTimecycMod_pressed)
					{
						std::string inputStr = Game::InputBox(timecycModStr, 28U, "Enter timecycle mod name:", timecycModStr);
						if (inputStr.compare(timecycModStr) != 0)
						{
							timecycModStr = inputStr;
							nodeTimecycMod.text() = timecycModStr.c_str();
							doc.save_file((const char*)filePath.c_str());
						}
						// No OnScreenKeyboard!
					}

					float timecycModStrength = nodeTimecycMod.attribute("strength").as_float(1.0f);
					bool bTimecycModStrength_plus = false, bTimecycModStrength_minus = false;
					AddNumber("Strength", timecycModStrength, 2, null, bTimecycModStrength_plus, bTimecycModStrength_minus);
					if (bTimecycModStrength_plus)
					{
						if (timecycModStrength < 3.0f)
						{
							timecycModStrength += 0.02f;
							nodeTimecycMod.attribute("strength") = timecycModStrength;
							doc.save_file((const char*)filePath.c_str());
						}
					}
					else if (bTimecycModStrength_minus)
					{
						if (timecycModStrength > 0.0f)
						{
							currentTimecycleStrength -= 0.02f;
							nodeTimecycMod.attribute("strength") = timecycModStrength;
							doc.save_file((const char*)filePath.c_str());
						}
					}
				}

				auto nodeWeatherToSet = nodeRoot.child("WeatherToSet");
				if (nodeWeatherToSet)
				{
					AddBreak("Weather To Set");
					std::string weatherToSetStr = nodeWeatherToSet.text().as_string();
					int weatherToSetInt = static_cast<int>(World::GetWeather(weatherToSetStr));
					//bool weatherToSet_plus = false, weatherToSet_minus = false;
					//AddTexter("Weather To Set", (weatherToSetInt == -1 ? 0 : weatherToSetInt), (weatherToSetInt == -1 ? std::vector<std::string>{"None"} : World::sWeatherNames), null, weatherToSet_plus, weatherToSet_minus);
					//if (weatherToSet_plus){ if (weatherToSetInt < World::sWeatherNames.size() - 1){ weatherToSetInt++; nodeWeatherToSet.text() = World::WeatherName_get(static_cast<WeatherType>(weatherToSetInt)).c_str(); doc.save_file((const char*)filePath.c_str()); } }
					//if (weatherToSet_minus){ if (weatherToSetInt > -1){ weatherToSetInt--; nodeWeatherToSet.text() = World::WeatherName_get(static_cast<WeatherType>(weatherToSetInt)).c_str(); doc.save_file((const char*)filePath.c_str()); } }

					bool bWeatherNonePressed = false;
					AddTickol("None", weatherToSetInt == -1, bWeatherNonePressed, bWeatherNonePressed); if (bWeatherNonePressed)
					{
						nodeWeatherToSet.text() = "";
						doc.save_file((const char*)filePath.c_str());
					}
					for (auto& ws : World::sWeatherNames)
					{
						bool bWeatherNamePressed = false;
						AddTickol(ws.first, weatherToSetStr == ws.second, bWeatherNamePressed, bWeatherNamePressed); if (bWeatherNamePressed)
						{
							nodeWeatherToSet.text() = ws.second.c_str();
							doc.save_file((const char*)filePath.c_str());
						}
					}
				}

			}

		}
		void Sub_SaveFiles_Load_LegacySP00N()
		{
			std::string& _name = dict;
			std::string& _dir = dict3;
			std::string filePath = _dir + "\\" + _name + ".SP00N";

			AddTitle(_name);

			bool bLoadPlacements = false;
			AddOption("Load Placements", bLoadPlacements); if (bLoadPlacements)
			{
				if (FileManagement::LoadPlacementsFromSP00NFile(filePath))
				{
					Game::Print::PrintBottomLeft("File ~b~loaded~s~.");
				}
				else
				{
					Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to load file.");
					addlog(ige::LogType::LOG_ERROR, "Attempt to load Placements file from" + filePath + " failed");
				}
			}

			bool bRenameFile = false;
			AddOption("Rename File", bRenameFile); if (bRenameFile)
			{
				std::string inputStr = Game::InputBox("", 28U, "Enter new name:", _name);
				if (inputStr.length() > 0)
				{
					if (rename(filePath.c_str(), (_dir + "\\" + inputStr + ".SP00N").c_str()) == 0)
					{
						_name = inputStr;
						Game::Print::PrintBottomLeft("File ~b~renamed~s~.");
					}
					else
					{
						addlog(ige::LogType::LOG_ERROR, "Attempt to rename file from" + _name + " to " + inputStr +".SP00N failed");
					}
				}
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::RenameSp00nFile, std::string(), 28U, "Enter new name:", fileName);
				//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&_name);
				//OnscreenKeyboard::State::arg2._ptr = reinterpret_cast<void*>(&_dir);
			}

			bool bDeleteFile = false;
			AddOption("Delete File", bDeleteFile); if (bDeleteFile)
			{
				if (remove(filePath.c_str()) == 0)
				{
					Game::Print::PrintBottomLeft("File ~b~deleted~s~.");
				}
				else
				{
					Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to delete file.");
					addlog(ige::LogType::LOG_ERROR, "Attempt to delete file" + filePath + " failed");
				}
				Menu::SetPreviousMenu();
				Menu::Up();
				return;
			}
		}
		void Sub_ManageEntities()
		{
			selectedEntity.handle = 0;

			SpoonerEntity* p_entityToDelete = nullptr;
			GTAentity myPed = PLAYER_PED_ID();
			auto myIndexInDb = EntityManagement::GetEntityIndexInDb(myPed);
			bool bIsSelfInDb = myIndexInDb >= 0;

			AddTitle("Manage Entities");

			AddOption("Removal", null, nullFunc, SUB::SPOONER_MANAGEDB_REMOVAL);

			bool bPressedSelf = false;
			AddOption((std::string)"Self (" + (bIsSelfInDb ? "Is In Database)" : "Is Not In Database)"), bPressedSelf); if (bPressedSelf)
			{
				if (bIsSelfInDb) selectedEntity = Databases::EntityDb[myIndexInDb];
				else
				{
					selectedEntity = SpoonerEntity();
					selectedEntity.dynamic = true;
					selectedEntity.handle = myPed;
					selectedEntity.hashName = GetPedModelLabel(myPed.Model(), true);
					if (selectedEntity.hashName.length() == 0)
						selectedEntity.hashName = "Player (Unk Model)";
					selectedEntity.isStill = true;
					selectedEntity.type = EntityType::PED;
				}
				Menu::SetSub_delayed = SUB::SPOONER_SELECTEDENTITYOPS;
				return;
			}

			if (!Databases::EntityDb.empty())
			{
				AddBreak("---Database---");

				bool bEntTypeToShow_plus = false, bEntTypeToShow_minus = false;
				AddTexter("Show", _entTypeToShowTexterValue, std::vector<std::string>{"All", "Peds", "Vehicles", "Objects"}, null, bEntTypeToShow_plus, bEntTypeToShow_minus);
				if (bEntTypeToShow_plus) { if (_entTypeToShowTexterValue < 3) _entTypeToShowTexterValue++; }
				if (bEntTypeToShow_minus) { if (_entTypeToShowTexterValue > 0) _entTypeToShowTexterValue--; }

				for (UINT i = 0; i < Databases::EntityDb.size(); i++)
				{
					auto& e = Databases::EntityDb[i];
					if (_entTypeToShowTexterValue != 0 && static_cast<UINT8>(e.type) != _entTypeToShowTexterValue)
						continue;
					bool bEntityExists = e.handle.Exists();
					bool bEntityPressed = false;

					const std::string& strEntTypeConcat = (*Menu::currentopATM == Menu::printingop + 1 ? "  ~bold~[" + e.TypeName() + "]~bold~" : std::string());
					AddOption(e.hashName + (bEntityExists ? "" : " (Invalid)") + strEntTypeConcat, bEntityPressed); if (bEntityPressed)
					{
						if (bEntityExists)
						{
							selectedEntity = e;
							Menu::SetSub_delayed = SUB::SPOONER_SELECTEDENTITYOPS;
						}
					}


					if (*Menu::currentopATM == Menu::printingop)
					{
						EntityManagement::ShowArrowAboveEntity(e.handle);

						bool bShortcutDeletePressed;
						if (Menu::bitController)
						{
							Menu::add_IB(INPUT_SCRIPT_RLEFT, bEntityExists ? "Delete Entity" : "Remove Invalid Entity From DB");
							bShortcutDeletePressed = IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_SCRIPT_RLEFT) != 0;
						}
						else
						{
							Menu::add_IB(VirtualKey::B, bEntityExists ? "Delete Entity" : "Remove Invalid Entity From DB");
							bShortcutDeletePressed = IsKeyJustUp(VirtualKey::B);
						}

						if (bShortcutDeletePressed)
						{
							p_entityToDelete = &e;
							//i--;
							//if (*Menu::currentopATM >= Menu::totalop) Menu::Up();
						}
					}

				}
			}

			if (p_entityToDelete != nullptr)
			{
				auto e = *p_entityToDelete;
				if (e.handle.Exists())
				{
					e.handle.RequestControl(600);
					EntityManagement::DeleteEntity(e);
				}
				else
				{
					EntityManagement::RemoveEntityFromDb(e);
				}
			}

			if (*Menu::currentopATM > Menu::printingop)
				Menu::Up(); // Go up if you're too far down due to removing an entity from DB from the properties menu and coming back here again.

		}
		void Sub_ManageEntities_Removal()
		{
			AddTitle("Removal");

			//AddOption("Select DB Entities To Delete", null, nullFunc, SUB::SPOONER_MANAGEDB_REMOVAL_FROMDB);

			AddBreak("---Database---");
			AddOption("Delete All Markers (" + std::to_string(Databases::MarkerDb.size()) + ")", null, MarkerManagement::RemoveAllMarkers);
			AddOption("Delete All Entities In Database (" + std::to_string(Databases::EntityDb.size()) + ")", null, EntityManagement::DeleteAllEntitiesInDb);
			AddOption("Delete All Objects In Database", null, EntityManagement::DeleteAllPropsInDb);
			AddOption("Delete All Peds In Database", null, EntityManagement::DeleteAllPedsInDb);
			AddOption("Delete All Vehicles In Database", null, EntityManagement::DeleteAllVehiclesInDb);
			AddOption("Delete All Invalid Entities In Database", null, EntityManagement::DeleteInvalidEntitiesInDb);

			AddBreak("---World---");
			AddOption("Delete All Entities In World (" + std::to_string(worldEntities.size()) + ")", null, EntityManagement::DeleteAllEntitiesInWorld);
			AddOption("Delete All Objects In World (" + std::to_string(worldObjects.size()) + ")", null, EntityManagement::DeleteAllPropsInWorld);
			AddOption("Delete All Peds In World (" + std::to_string(worldPeds.size()) + ")", null, EntityManagement::DeleteAllPedsInWorld);
			AddOption("Delete All Vehicles In World (" + std::to_string(worldVehicles.size()) + ")", null, EntityManagement::DeleteAllVehiclesInWorld);

			AddOption("Clear Entity Database (And Keep Entities)", null, EntityManagement::ClearDb);
		}
		/*void Sub_ManageEntities_Removal_FromDb()
		{
			if (Databases::EntityDb.empty())
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ The Spooner entity database is empty.");
				Menu::SetSub_previous();
				return;
			}

			AddTitle("Delete Entities");

			for (auto& e : Databases::EntityDb)
			{
				bool bEntityExists = e.handle.Exists();
				bool bEntityPressed = false;
				AddOption(e.hashName + (bEntityExists ? "" : " (Invalid)"), bEntityPressed);
				if (*Menu::currentopATM == Menu::printingop) EntityManagement::ShowArrowAboveEntity(e.handle, RGBA(255, 0, 0, 200));
				if (bEntityPressed)
				{
					if (bEntityExists)
					{
						e.handle.RequestControl(600);
						EntityManagement::DeleteEntity(e);
					}
					else
					{
						EntityManagement::RemoveEntityFromDb(e);
					}
					break;
				}
			}

		}*/
		void Sub_SelectedEntityOps()
		{
			if (!selectedEntity.handle.Exists())
			{
				Menu::SetPreviousMenu();
				return;
			}
			selectedEntity.handle.RequestControlOnce();
			auto thisEntityIndexInDb = EntityManagement::GetEntityIndexInDb(selectedEntity);
			bool isThisEntityInDb = thisEntityIndexInDb >= 0;
			if (isThisEntityInDb)
			{
				selectedEntity.taskSequence = Databases::EntityDb[thisEntityIndexInDb].taskSequence;
				Databases::EntityDb[thisEntityIndexInDb] = selectedEntity;
			}
			Model selectedEntityModel = selectedEntity.handle.Model();

			AddTitle(selectedEntity.hashName);

			switch (selectedEntity.type)
			{
			case EntityType::PROP:
			{
				bool bIsAFav = FavouritesManagement::IsPropAFavourite(selectedEntity.hashName, selectedEntityModel.hash);
				bool bAddToFav = false, bRemoveFromFav = false;
				AddTickol("Model Is A Favourite", bIsAFav, bAddToFav, bRemoveFromFav, TICKOL::BOXTICK, TICKOL::BOXBLANK);
				if (bAddToFav)
				{
					FavouritesManagement::AddPropToFavourites(selectedEntity.hashName, selectedEntityModel.hash);
				}
				else if (bRemoveFromFav)
				{
					FavouritesManagement::RemovePropFromFavourites(selectedEntity.hashName, selectedEntityModel.hash);
				}
			}
			break;
			case EntityType::VEHICLE:
			{
				bool bIsAFav = FavouritesManagement::IsVehicleAFavourite(selectedEntityModel);
				bool bAddToFav = false, bRemoveFromFav = false;
				AddTickol("Model Is A Favourite", bIsAFav, bAddToFav, bRemoveFromFav, TICKOL::BOXTICK, TICKOL::BOXBLANK);
				if (bAddToFav)
				{
					std::string inputStr = Game::InputBox("", 28U, "Enter custom name:", selectedEntityModel.VehicleDisplayName(true));
					if (inputStr.length() > 0)
					{
						FavouritesManagement::AddVehicleToFavourites(selectedEntityModel, inputStr);
					}
					//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::FavouriteVehicleModelCurrent, std::string(), 28U, "Enter custom name:", selectedEntityModel.VehicleDisplayName(true));
					//OnscreenKeyboard::State::arg1._uint = selectedEntityModel.hash;
				}
				else if (bRemoveFromFav)
				{
					FavouritesManagement::RemoveVehicleFromFavourites(selectedEntityModel);
				}
			}
			break;
			}

			bool bAddToOrDb = false, bRemoveFromDb = false;
			AddTickol("Entity Is In Database", isThisEntityInDb, bAddToOrDb, bRemoveFromDb, TICKOL::BOXTICK, TICKOL::BOXBLANK);
			if (bAddToOrDb)
			{
				EntityManagement::AddEntityToDb(selectedEntity);
				Game::Print::PrintBottomLeft(selectedEntity.hashName + " added to database.");
			}
			else if (bRemoveFromDb)
			{
				EntityManagement::RemoveEntityFromDb(selectedEntity);
				Game::Print::PrintBottomLeft(selectedEntity.hashName + " removed from database. Properties will no longer be stored in Spooner's memory.");
				if (Menu::currentArray[Menu::currentArrayIndex] == SUB::SPOONER_MANAGEDB)
				{
					Menu::currentop_ar[Menu::currentArrayIndex] -= 1;
				}
			}

			bool bCopyPressed = false, bCopy_plus = false, bCopy_minus = false;
			AddTexter("Copy", _copyEntTexterValue, std::vector<std::string>{ "This Entity Only", "With Attachments" }, bCopyPressed, bCopy_plus, bCopy_minus);
			if (bCopy_plus) { if (_copyEntTexterValue < 1U) _copyEntTexterValue++; }
			if (bCopy_minus) { if (_copyEntTexterValue > 0) _copyEntTexterValue--; }
			if (bCopyPressed)
			{
				const SpoonerEntity& copiedEntity = EntityManagement::CopyEntity(selectedEntity, isThisEntityInDb, true, _copyEntTexterValue);
				//EntityManagement::AddEntityToDb(copiedEntity);
				selectedEntity = copiedEntity;
			}

			bool bDeletePressed = false;
			AddOption("Delete", bDeletePressed); if (bDeletePressed)
			{
				selectedEntity.handle.RequestControl(600);
				EntityManagement::DeleteEntity(selectedEntity);
				if (Menu::currentArray[Menu::currentArrayIndex] == SUB::SPOONER_MANAGEDB)
				{
					Menu::currentop_ar[Menu::currentArrayIndex] -= 1;
				}
				Menu::SetPreviousMenu();
				return;
			}

			bool bDynamicPressed = false;
			AddLocal("Dynamic", selectedEntity.dynamic, bDynamicPressed, bDynamicPressed); if (bDynamicPressed)
			{
				selectedEntity.dynamic = !selectedEntity.dynamic;
				selectedEntity.handle.SetDynamic(selectedEntity.dynamic);
				selectedEntity.handle.FreezePosition(!selectedEntity.dynamic);
			}

			bool bFreezePosPressed = false;
			AddLocal("Frozen In Place", selectedEntity.handle.IsPositionFrozen(), bFreezePosPressed, bFreezePosPressed); if (bFreezePosPressed)
			{
				selectedEntity.handle.FreezePosition(!selectedEntity.handle.IsPositionFrozen());
			}
			if (selectedEntity.type == EntityType::VEHICLE && selectedEntity.handle.GetLandingGearState() != -1)
			{
				bool bLandingGearDown = (selectedEntity.handle.GetLandingGearState() == 0); // 0 = Deployed
				bool oldValue = bLandingGearDown;

				AddToggle("Landing Gear", bLandingGearDown);

				if (bLandingGearDown != oldValue)
				{
					selectedEntity.handle.SetLandingGear(bLandingGearDown);
				}
			}

			if (selectedEntity.type == EntityType::PROP)
			{
				auto& thisTextureVariation = selectedEntity.textureVariation;
				bool bTextureVariation_plus = false, bTextureVariation_minus = false;
				AddNumber("Texture Variation", thisTextureVariation, 0, null, bTextureVariation_plus, bTextureVariation_minus);
				if (bTextureVariation_plus) { if (thisTextureVariation < UINT8_MAX) { thisTextureVariation++; SET_OBJECT_TINT_INDEX(selectedEntity.handle.Handle(), thisTextureVariation); } }
				if (bTextureVariation_minus) { if (thisTextureVariation > 0) { thisTextureVariation--; SET_OBJECT_TINT_INDEX(selectedEntity.handle.Handle(), thisTextureVariation); } }
			}

			int thisHealth = selectedEntity.handle.GetHealth();
			bool bHealth_plus = false, bHealth_minus = false, bHealth_input = false;
			AddNumber("Health", thisHealth, 0, bHealth_input, bHealth_plus, bHealth_minus);
			if (bHealth_plus) { if (thisHealth < INT_MAX) { thisHealth++; if (selectedEntity.handle.GetMaxHealth() < thisHealth) { selectedEntity.handle.SetMaxHealth(thisHealth); } selectedEntity.handle.SetHealth(thisHealth); } }
			if (bHealth_minus) { if (thisHealth > 0) { thisHealth--; if (selectedEntity.handle.GetMaxHealth() > thisHealth + 30) { selectedEntity.handle.SetMaxHealth(thisHealth + 30); } selectedEntity.handle.SetHealth(thisHealth); } }
			if (bHealth_input)
			{
				GTAentity& ent = selectedEntity.handle;
				std::string inputStr = Game::InputBox("", 5U, "", std::to_string(thisHealth));
				if (inputStr.length() > 0)
				{
					try
					{
						thisHealth = stoi(inputStr);
						if (ent.GetMaxHealth() < thisHealth)
						{
							ent.SetMaxHealth(thisHealth);
						}
						ent.SetHealth(thisHealth);
					}
					catch (...) { Game::Print::PrintErrorInvalidInput(inputStr); }
				}
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SpoonerEoHealth, std::string(), 4U, std::string(), std::to_string(thisHealth));
				//OnscreenKeyboard::State::arg1._int = ent.GetHandle();
			}

			bool bInvinciblePressed = false;
			AddLocal("Invincible", selectedEntity.handle.IsInvincible(), bInvinciblePressed, bInvinciblePressed); if (bInvinciblePressed)
			{
				bool bEnable = !selectedEntity.handle.IsInvincible();
				selectedEntity.handle.SetInvincible(bEnable);
				selectedEntity.handle.SetExplosionProof(bEnable);
				selectedEntity.handle.SetMeleeProof(bEnable);
			}

			bool bFireProofPressed = false;
			AddLocal("Fireproof", selectedEntity.handle.IsFireProof(), bFireProofPressed, bFireProofPressed); if (bFireProofPressed)
			{
				bool bEnable = !selectedEntity.handle.IsFireProof();
				selectedEntity.handle.SetFireProof(bEnable);
			}
			bool bOnFirePressed = false;
			AddLocal("Is On Fire (For Peds)", selectedEntity.handle.IsOnFire(), bOnFirePressed, bOnFirePressed); if (bOnFirePressed)
			{
				bool bEnable = !selectedEntity.handle.IsOnFire();
				selectedEntity.handle.SetOnFire(bEnable);
			}

			bool bVisiblePressed = false;
			AddLocal("Visible", selectedEntity.handle.IsVisible(), bVisiblePressed, bVisiblePressed); if (bVisiblePressed)
			{
				selectedEntity.handle.SetVisible(!selectedEntity.handle.IsVisible());
			}

			AddOption("Opacity (Local)", null, SetEnt12, SUB::ENTITYALPHALEVEL);

			bool bCollisionPressed = false;
			AddLocal("Collision", selectedEntity.handle.GetIsCollisionEnabled(), bCollisionPressed, bCollisionPressed); if (bCollisionPressed)
			{
				GTAentity attBaseEnt;
				EntityManagement::GetEntityThisEntityIsAttachedTo(selectedEntity.handle, attBaseEnt);
				selectedEntity.handle.SetIsCollisionEnabled(!selectedEntity.handle.GetIsCollisionEnabled());
				if (selectedEntity.attachmentArgs.isAttached)
				{
					EntityManagement::AttachEntity(selectedEntity, attBaseEnt, selectedEntity.attachmentArgs.boneIndex, selectedEntity.attachmentArgs.offset, selectedEntity.attachmentArgs.rotation);
				}
			}

			bool bHasGravity = selectedEntity.handle.GetHasGravity();
			bool bGravityTogglePressed = false;
			AddLocal("Gravity", bHasGravity, bGravityTogglePressed, bGravityTogglePressed); if (bGravityTogglePressed)
			{
				switch (selectedEntity.type)
				{
				case EntityType::PED: GTAped(selectedEntity.handle).SetHasGravity(!bHasGravity); break;
				case EntityType::VEHICLE: GTAvehicle(selectedEntity.handle).SetHasGravity(!bHasGravity); break;
				case EntityType::PROP: GTAprop(selectedEntity.handle).SetHasGravity(!bHasGravity); break;
				default: selectedEntity.handle.SetHasGravity(!bHasGravity); break;
				}
				bHasGravity = !bHasGravity;
			}

			bool bGoToEntityPressed = false;
			AddOption("Go To Entity", bGoToEntityPressed); if (bGoToEntityPressed)
			{
				if (SpoonerMode::spoonerModeCamera.IsActive())
				{
					auto& cam = SpoonerMode::spoonerModeCamera;
					cam.SetPosition(selectedEntity.handle.GetOffsetInWorldCoords(0, -5.0f - selectedEntity.handle.Dim2().y, 0));
				}
				else
				{
					GTAped myPed = PLAYER_PED_ID();
					myPed.SetPosition(selectedEntity.handle.GetPosition());
				}
			}

			bool bBringEntityToSelfPressed = false;
			AddOption((std::string)"Bring Entity To Self" + (selectedEntity.handle.IsAttached() ? " (And Detach)" : ""), bBringEntityToSelfPressed); if (bBringEntityToSelfPressed)
			{
				//GTAentity entityToTele = selectedEntity.handle;
				//GTAentity entityToTeleMaybe;
				//while (EntityManagement::GetEntityThisEntityIsAttachedTo(entityToTele, entityToTeleMaybe))
					//entityToTele = entityToTeleMaybe;
				if (selectedEntity.handle.IsAttached()) EntityManagement::DetachEntity(selectedEntity); // Detach if attached :(

				if (SpoonerMode::spoonerModeCamera.IsActive())
				{
					auto& cam = SpoonerMode::spoonerModeCamera;
					selectedEntity.handle.SetPosition(cam.GetOffsetInWorldCoords(0, 5.0f + selectedEntity.handle.Dim2().y, 0));
				}
				else
				{
					GTAped myPed = PLAYER_PED_ID();
					selectedEntity.handle.SetPosition(myPed.GetPosition() + (myPed.ForwardVector() * (selectedEntity.handle.Dim1().y + 4.0f)));
				}
			}

			bool bPlaceOnGround = false;
			AddOption("Place On Ground", bPlaceOnGround); if (bPlaceOnGround)
			{
				selectedEntity.handle.PlaceOnGround();
			}

			bool bResetRotation = false;
			AddOption("Reset rotation", bResetRotation); if (bResetRotation)
			{
				selectedEntity.handle.SetRotation(Vector3::Zero());
			}

			AddOption("TriggerFX", null, SetEnt241, SUB::PTFXSUB);

			bool bGoToTaskSeqMenu = false;
			AddTexter("Task Sequence", selectedEntity.taskSequence.IsActive() ? 1 : 0, std::vector<std::string>{"Inactive", "Active"}, bGoToTaskSeqMenu); if (bGoToTaskSeqMenu)
			{
				Menu::SetSub_delayed = SUB::SPOONER_TASKSEQUENCE_TASKLIST;
			}

			if (selectedEntity.type == EntityType::PED)
			{
				AddOption("Ped Options", null, nullFunc, SUB::SPOONER_PEDOPS);
			}
			else if (selectedEntity.type == EntityType::VEHICLE)
			{
				AddOption("Menyoo Customs", null, SetEnt12, SUB::MODSHOP);
			}

			AddOption("Attachment Options", null, nullFunc, SUB::SPOONER_ATTACHMENTOPS);
			AddOption("Manual Placement", null, nullFunc, SUB::SPOONER_MANUALPLACEMENT);
			AddOption("Manual Resize (beta)", null, nullFunc, SUB::SPOONER_SIZEMANIPULATION);

		}
		void Sub_AttachmentOps()
		{
			if (!selectedEntity.handle.Exists())
			{
				Menu::SetPreviousMenu();
				return;
			}
			selectedEntity.handle.RequestControlOnce();
			auto thisEntityIndexInDb = EntityManagement::GetEntityIndexInDb(selectedEntity);
			bool isThisEntityInDb = thisEntityIndexInDb >= 0;
			if (isThisEntityInDb)
			{
				selectedEntity.taskSequence = Databases::EntityDb[thisEntityIndexInDb].taskSequence;
				Databases::EntityDb[thisEntityIndexInDb] = selectedEntity;
			}

			GTAentity baseEntityIfExists;
			bool seIsAttached = EntityManagement::GetEntityThisEntityIsAttachedTo(selectedEntity.handle, baseEntityIfExists);
			EntityType baseEntityType = (EntityType)baseEntityIfExists.Type();

			bool prec_plus = 0, prec_minus = 0;

			AddTitle("Attachment");

			if (!seIsAttached)
			{
				AddOption("Attach To Something", null, nullFunc, SUB::SPOONER_ATTACHMENTOPS_ATTACHTO);
			}
			else
			{
				bool bDetachPressed = false;
				AddOption("Detach", bDetachPressed); if (bDetachPressed)
				{
					EntityManagement::DetachEntity(selectedEntity);
				}
			}

			AddNumber("Scroll Sensitivity", _manualPlacementPrecision, 4, null, prec_minus, prec_plus);
			if (prec_plus) { if (_manualPlacementPrecision < 10.0f) _manualPlacementPrecision *= 10; }
			if (prec_minus) { if (_manualPlacementPrecision > 0.0001f) _manualPlacementPrecision /= 10; }

			if (seIsAttached)
			{
				bool x_plus = 0, x_minus = 0;
				bool y_plus = 0, y_minus = 0;
				bool z_plus = 0, z_minus = 0;
				bool pitch_plus = 0, pitch_minus = 0;
				bool roll_plus = 0, roll_minus = 0;
				bool yaw_plus = 0, yaw_minus = 0;

				int nextBoneIndex = selectedEntity.attachmentArgs.boneIndex;
				Vector3 nextOffset = selectedEntity.attachmentArgs.offset;
				Vector3 nextRot = selectedEntity.attachmentArgs.rotation;

				// Bone text scroller if type is PED or VEHICLE. Reattach and reset args on bone change.
				if (baseEntityType == EntityType::PED)
				{
					int obj_currentPedBoneArrayIndex = 17; // SKEL_ROOT is at index 17 idk
					for (int i = 0; i < Bone::vBoneNames.size(); i++)
					{
						if (nextBoneIndex == GTAped(baseEntityIfExists).GetBoneIndex(Bone::vBoneNames[i].boneid))
						{
							obj_currentPedBoneArrayIndex = i;
							break;
						}
					}
					std::vector<std::string> vCurrentPedBoneNameStr{ Bone::vBoneNames[obj_currentPedBoneArrayIndex].name };

					bool pbone_plus = 0, pbone_minus = 0, pbone_input = 0;
					AddTexter("Bone", 0, vCurrentPedBoneNameStr, pbone_input, pbone_plus, pbone_minus);
					if (pbone_plus)
					{
						if (obj_currentPedBoneArrayIndex < Bone::vBoneNames.size() - 1)
						{
							obj_currentPedBoneArrayIndex++;
							nextBoneIndex = GTAped(baseEntityIfExists).GetBoneIndex(Bone::vBoneNames[obj_currentPedBoneArrayIndex].boneid);
							//nextOffset = Vector3::Zero();
							//nextRot = Vector3::Zero();
						}
					}
					if (pbone_minus)
					{
						if (obj_currentPedBoneArrayIndex > 0)
						{
							obj_currentPedBoneArrayIndex--;
							nextBoneIndex = GTAped(baseEntityIfExists).GetBoneIndex(Bone::vBoneNames[obj_currentPedBoneArrayIndex].boneid);
							//nextOffset = Vector3::Zero();
							//nextRot = Vector3::Zero();
						}
					}
					if (pbone_input)
					{
						//std::string srch = Game::InputBox("", 28U, "Enter bone name:");
						//bool found = false;
						//for (auto& pb : Bone::vBoneNames)
						//{
						//	if (pb.name.find(srch) != std::string::npos)
						//	{
						//		bool found = true;
						//		nextBoneIndex = GTAped(baseEntityIfExists).GetBoneIndex(pb.boneid);
						//		//obj_currentPedBoneArrayIndex = index; // Not needed
						//		nextOffset = Vector3::Zero();
						//		nextRot = Vector3::Zero();
						//		break;
						//	}
						//}
						//if (!found) Game::Print::PrintError_InvalidInput(inputStr);
						Menu::SetSub_delayed = SUB::SPOONER_ATTACHMENTOPS_SELECTBONE;
					}
				}
				else if (baseEntityType == EntityType::VEHICLE)
				{
					int obj_currentVehBoneArrayIndex = 10; // 10 is bodyshell idk
					for (int i = 0; i < VBone::vNames.size(); i++)
					{
						if (nextBoneIndex == GTAvehicle(baseEntityIfExists).GetBoneIndex(VBone::vNames[i]))
						{
							obj_currentVehBoneArrayIndex = i;
							break;
						}
					}
					std::vector<std::string> vCurrentVehBoneNameStr{ VBone::vNames[obj_currentVehBoneArrayIndex] };

					bool vbone_plus = 0, vbone_minus = 0, vbone_input = 0;
					AddTexter("Bone", 0, vCurrentVehBoneNameStr, vbone_input, vbone_plus, vbone_minus);
					if (vbone_plus)
					{
						if (obj_currentVehBoneArrayIndex < VBone::vNames.size() - 1)
						{
							obj_currentVehBoneArrayIndex++;
							nextBoneIndex = GTAvehicle(baseEntityIfExists).GetBoneIndex(VBone::vNames[obj_currentVehBoneArrayIndex]);
							//nextOffset = Vector3::Zero();
							//nextRot = Vector3::Zero();
						}
					}
					if (vbone_minus)
					{
						if (obj_currentVehBoneArrayIndex > 0)
						{
							obj_currentVehBoneArrayIndex--;
							nextBoneIndex = GTAvehicle(baseEntityIfExists).GetBoneIndex(VBone::vNames[obj_currentVehBoneArrayIndex]);
							//nextOffset = Vector3::Zero();
							//nextRot = Vector3::Zero();
						}
					}
					if (vbone_input)
					{
						//std::string srch = Game::InputBox("", 28U, "Enter bone name:");
						//bool found = false;
						//for (auto& vbn : VBone::vNames)
						//{
						//	if (vbn.find(srch) != std::string::npos)
						//	{
						//		bool found = true;
						//		nextBoneIndex = GTAvehicle(baseEntityIfExists).GetBoneIndex(vbn);
						//		//obj_currentVehBoneArrayIndex = index; // Not needed
						//		nextOffset = Vector3::Zero();
						//		nextRot = Vector3::Zero();
						//		break;
						//	}
						//}
						//if (!found) Game::Print::PrintError_InvalidInput(inputStr);
						Menu::SetSub_delayed = SUB::SPOONER_ATTACHMENTOPS_SELECTBONE;
					}
				}

				AddNumber("X", nextOffset.x, 4, null, x_plus, x_minus);
				AddNumber("Y", nextOffset.y, 4, null, y_plus, y_minus);
				AddNumber("Z", nextOffset.z, 4, null, z_plus, z_minus);
				AddNumber("Pitch", nextRot.x, 4, null, pitch_plus, pitch_minus);
				AddNumber("Roll", nextRot.y, 4, null, roll_plus, roll_minus);
				AddNumber("Yaw", nextRot.z, 4, null, yaw_plus, yaw_minus);

				if (x_plus) nextOffset.x += _manualPlacementPrecision;
				if (x_minus) nextOffset.x -= _manualPlacementPrecision;
				if (y_plus) nextOffset.y += _manualPlacementPrecision;
				if (y_minus) nextOffset.y -= _manualPlacementPrecision;
				if (z_plus) nextOffset.z += _manualPlacementPrecision;
				if (z_minus) nextOffset.z -= _manualPlacementPrecision;

				if (pitch_plus) { nextRot.x += _manualPlacementPrecision; if (nextRot.x > 180.0f) nextRot.x -= 360.0f; }
				if (pitch_minus) { nextRot.x -= _manualPlacementPrecision; if (nextRot.x < -180.0f) nextRot.x += 360.0f; }
				if (roll_plus) { nextRot.y += _manualPlacementPrecision; if (nextRot.y > 180.0f) nextRot.y -= 360.0f; }
				if (roll_minus) { nextRot.y -= _manualPlacementPrecision; if (nextRot.y < -180.0f) nextRot.y += 360.0f; }
				if (yaw_plus) { nextRot.z += _manualPlacementPrecision; if (nextRot.z > 180.0f) nextRot.z -= 360.0f; }
				if (yaw_minus) { nextRot.z -= _manualPlacementPrecision; if (nextRot.z < -180.0f) nextRot.z += 360.0f; }

				if (nextOffset != selectedEntity.attachmentArgs.offset || nextRot != selectedEntity.attachmentArgs.rotation || nextBoneIndex != selectedEntity.attachmentArgs.boneIndex)
				{
					EntityManagement::AttachEntity(selectedEntity, baseEntityIfExists, nextBoneIndex, nextOffset, nextRot);
				}
			}

		}
		void Sub_AttachmentOps_AttachTo()
		{
			if (!selectedEntity.handle.Exists())
			{
				Menu::SetPreviousMenu();
				return;
			}
			GTAped myPed = PLAYER_PED_ID();
			GTAvehicle myVehicle = g_myVeh;

			AddTitle("Attach To Something");

			bool bToggleKeepPosWhenAttaching = false;
			AddTickol("Keep World Position When Attaching", Settings::bKeepPositionWhenAttaching, bToggleKeepPosWhenAttaching, bToggleKeepPosWhenAttaching, TICKOL::BOXTICK, TICKOL::BOXBLANK); if (bToggleKeepPosWhenAttaching)
				Settings::bKeepPositionWhenAttaching = !Settings::bKeepPositionWhenAttaching;

			AddBreak("---Available Entities---");

			bool bSelf = false;
			AddOption("Self", bSelf); if (bSelf)
			{
				EntityManagement::AttachEntityInit(selectedEntity, myPed, Settings::bKeepPositionWhenAttaching);
				Menu::SetPreviousMenu();
				return;
			}

			if (myVehicle.Exists())
			{
				bool bSelfVeh = false;
				AddOption(std::string(myPed.IsInVehicle() ? "Current" : "Last Seated") + " Vehicle", bSelfVeh); if (bSelfVeh)
				{
					EntityManagement::AttachEntityInit(selectedEntity, myVehicle, Settings::bKeepPositionWhenAttaching);
					Menu::SetPreviousMenu();
					return;
				}
			}

			if (!Databases::EntityDb.empty())
			{
				if (Databases::EntityDb.size() > 1 || Databases::EntityDb.front().handle != selectedEntity.handle)
				{
					AddBreak("---Database---");
					for (auto& e : Databases::EntityDb)
					{
						if (e.handle != selectedEntity.handle)
						{
							if (e.handle.Exists())
							{
								if (!selectedEntity.handle.IsAttachedTo(e.handle))
								{
									bool bEntityPressed = false;
									AddOption(e.hashName, bEntityPressed); if (bEntityPressed)
									{
										EntityManagement::AttachEntityInit(selectedEntity, e.handle, Settings::bKeepPositionWhenAttaching);
										Menu::SetPreviousMenu();
										return;
									}
									if (*Menu::currentopATM == Menu::printingop)
										EntityManagement::ShowArrowAboveEntity(e.handle, RGBA(0, 255, 0, 200));
								}
								else
								{
									AddOption(e.hashName + " (already attached)", null);
									if (*Menu::currentopATM == Menu::printingop)
										EntityManagement::ShowArrowAboveEntity(e.handle, RGBA(255, 0, 0, 200));
								}
								
							}
							else
							{
								AddOption(e.hashName + " (Invalid)", null);
							}
						}
					}
				}
			}
		}
		void Sub_AttachmentOps_SelectBone()
		{
			if (!selectedEntity.handle.Exists())
			{
				Menu::SetPreviousMenu();
				Menu::currentop = 1;
				return;
			}
			selectedEntity.handle.RequestControlOnce();
			auto thisEntityIndexInDb = EntityManagement::GetEntityIndexInDb(selectedEntity);
			bool isThisEntityInDb = thisEntityIndexInDb >= 0;
			if (isThisEntityInDb)
			{
				selectedEntity.taskSequence = Databases::EntityDb[thisEntityIndexInDb].taskSequence;
				Databases::EntityDb[thisEntityIndexInDb] = selectedEntity;
			}

			GTAentity baseEntity;
			bool seIsAttached = EntityManagement::GetEntityThisEntityIsAttachedTo(selectedEntity.handle, baseEntity);
			EntityType baseEntityType = (EntityType)baseEntity.Type();
			if (!baseEntity.Exists())
			{
				Menu::SetPreviousMenu();
				Menu::currentop = 1;
				return;
			}

			int nextBoneIndex = selectedEntity.attachmentArgs.boneIndex;
			Vector3 nextOffset = selectedEntity.attachmentArgs.offset;
			Vector3 nextRot = selectedEntity.attachmentArgs.rotation;

			bool bSelectedBoneIndexExists = false; // Idek what this is doing here
			int defaultBone;

			AddTitle("Bone");

			// Bone text scroller if type is PED or VEHICLE. Reattach and reset args on bone change.
			if (baseEntityType == EntityType::PED)
			{
				GTAped baseEntityPed = baseEntity;
				defaultBone = baseEntityPed.GetBoneIndex(Bone::SKEL_ROOT);

				bSelectedBoneIndexExists = nextBoneIndex == baseEntityPed.GetBoneIndex(Bone::vBoneNames.front().boneid);
				for (auto& b : Bone::vBoneNames)
				{
					bool bBoneNamePressed = false;
					if (bSelectedBoneIndexExists && nextBoneIndex == baseEntityPed.GetBoneIndex(b.boneid))
					{
						continue;
					}
					bSelectedBoneIndexExists = (nextBoneIndex == baseEntityPed.GetBoneIndex(b.boneid)); // And is selected (in this case)
					AddTickol(b.name, bSelectedBoneIndexExists, bBoneNamePressed, bBoneNamePressed); if (bBoneNamePressed)
					{
						nextBoneIndex = baseEntityPed.GetBoneIndex(b.boneid);
						//nextOffset = Vector3::Zero();
						//nextRot = Vector3::Zero();
						break;
					}
				}
			}
			else if (baseEntityType == EntityType::VEHICLE)
			{
				GTAvehicle baseEntityVeh = baseEntity;
				defaultBone = baseEntityVeh.GetBoneIndex(VBone::bodyshell);

				bSelectedBoneIndexExists = nextBoneIndex == baseEntityVeh.GetBoneIndex(VBone::vNames.front());
				for (auto& b : VBone::vNames)
				{
					bool bBoneNamePressed = false;
					if (!baseEntityVeh.HasBone(b) || (bSelectedBoneIndexExists && nextBoneIndex == baseEntityVeh.GetBoneIndex(b)))
					{
						continue;
					}
					bSelectedBoneIndexExists = (nextBoneIndex == baseEntityVeh.GetBoneIndex(b)); // And is selected (in this case)
					AddTickol(b, bSelectedBoneIndexExists, bBoneNamePressed, bBoneNamePressed); if (bBoneNamePressed)
					{
						nextBoneIndex = baseEntityVeh.GetBoneIndex(b);
						//nextOffset = Vector3::Zero();
						//nextRot = Vector3::Zero();
						break;
					}
				}
			}

			if (nextBoneIndex != selectedEntity.attachmentArgs.boneIndex)
			{
				EntityManagement::AttachEntity(selectedEntity, baseEntity, nextBoneIndex, nextOffset, nextRot);
			}

		}
		void Sub_ManualPlacement()
		{
			if (!selectedEntity.handle.Exists())
			{
				Menu::SetPreviousMenu();
				return;
			}
			selectedEntity.handle.RequestControlOnce();

			GTAped thisPed = selectedEntity.handle;
			Vector3 currPos = selectedEntity.handle.GetPosition();
			Vector3 currRot = selectedEntity.handle.Rotation_get();
			Vector3 nextPos = currPos;
			Vector3 nextRot = currRot;

			bool prec_plus = 0, prec_minus = 0,
				x_plus = 0, x_minus = 0,
				y_plus = 0, y_minus = 0,
				z_plus = 0, z_minus = 0,
				pitch_plus = 0, pitch_minus = 0,
				roll_plus = 0, roll_minus = 0,
				yaw_plus = 0, yaw_minus = 0;

			AddTitle("Manual Placement");
			AddNumber("Scroll Sensitivity", _manualPlacementPrecision, 4, null, prec_minus, prec_plus);
			AddNumber("X", currPos.x, 4, null, x_plus, x_minus);
			AddNumber("Y", currPos.y, 4, null, y_plus, y_minus);
			AddNumber("Z", currPos.z, 4, null, z_plus, z_minus);
			AddNumber("Pitch", currRot.x, 4, null, pitch_plus, pitch_minus);
			AddNumber("Roll", currRot.y, 4, null, roll_plus, roll_minus);
			AddNumber("Yaw", currRot.z, 4, null, yaw_plus, yaw_minus);

			if (prec_plus) { if (_manualPlacementPrecision < 10.0f) _manualPlacementPrecision *= 10; }
			if (prec_minus) { if (_manualPlacementPrecision > 0.0001f) _manualPlacementPrecision /= 10; }

			if (x_plus) nextPos.x += _manualPlacementPrecision;
			if (x_minus) nextPos.x -= _manualPlacementPrecision;
			if (y_plus) nextPos.y += _manualPlacementPrecision;
			if (y_minus) nextPos.y -= _manualPlacementPrecision;
			if (z_plus) nextPos.z += _manualPlacementPrecision;
			if (z_minus) nextPos.z -= _manualPlacementPrecision;

			if (pitch_plus) { nextRot.x += _manualPlacementPrecision; if (nextRot.x > 180.0f) nextRot.x -= 360.0f; }
			if (pitch_minus) { nextRot.x -= _manualPlacementPrecision; if (nextRot.x < -180.0f) nextRot.x += 360.0f; }
			if (roll_plus) { nextRot.y += _manualPlacementPrecision; if (nextRot.y > 180.0f) nextRot.y -= 360.0f; }
			if (roll_minus) { nextRot.y -= _manualPlacementPrecision; if (nextRot.y < -180.0f) nextRot.y += 360.0f; }
			if (yaw_plus) { nextRot.z += _manualPlacementPrecision; if (nextRot.z > 180.0f) nextRot.z -= 360.0f; }
			if (yaw_minus) { nextRot.z -= _manualPlacementPrecision; if (nextRot.z < -180.0f) nextRot.z += 360.0f; }

			if (nextPos != currPos)
			{
				selectedEntity.handle.SetPosition(nextPos);
				currPos = selectedEntity.handle.GetPosition();
				GTAentity attBase;
				if (EntityManagement::GetEntityThisEntityIsAttachedTo(selectedEntity.handle, attBase))
					World::DrawLine(attBase.GetPosition(), currPos, RGBA::AllWhite()); // Just pointing out that it's attached
			}
			if (nextRot != currRot)
			{
				selectedEntity.handle.SetRotation(nextRot);
				currRot = selectedEntity.handle.Rotation_get();
				GTAentity attBase;
				if (EntityManagement::GetEntityThisEntityIsAttachedTo(selectedEntity.handle, attBase))
					World::DrawLine(attBase.GetPosition(), currPos, RGBA::AllWhite()); // Just pointing out that it's attached
			}
		}

		void Sub_SizeManipulation()
		{
			if (!selectedEntity.handle.Exists())
			{
				Menu::SetPreviousMenu();
				return;
			}
			selectedEntity.handle.RequestControlOnce();

			AddTitle("Size Manipulation");

			UINT64 ptr = GTAmemory::_entityAddressFunc(selectedEntity.handle.Handle());
			if (!ptr) return;

			float length = GTAmemory::ReadFloat(ptr + 0x60);
			float width = GTAmemory::ReadFloat(ptr + 0x74);
			float height = GTAmemory::ReadFloat(ptr + 0x88);

			bool prec_plus = false, prec_minus = false;
			bool x_plus = false, x_minus = false;
			bool y_plus = false, y_minus = false;
			bool z_plus = false, z_minus = false;

			AddNumber("Scroll Sensitivity", _manualPlacementPrecision, 4, null, prec_minus, prec_plus);
			AddNumber("Length (Y)", length, 4, null, y_plus, y_minus);
			AddNumber("Width (X)", width, 4, null, x_plus, x_minus);
			AddNumber("Height (Z)", height, 4, null, z_plus, z_minus);

			if (prec_plus) { if (_manualPlacementPrecision < 10.0f) _manualPlacementPrecision *= 10; }
			if (prec_minus) { if (_manualPlacementPrecision > 0.0001f) _manualPlacementPrecision /= 10; }

			if (y_plus) length += (_manualPlacementPrecision * 25.0f);
			if (y_minus) length -= (_manualPlacementPrecision * 25.0f);
			if (x_plus) width += (_manualPlacementPrecision * 25.0f);
			if (x_minus) width -= (_manualPlacementPrecision * 25.0f);
			if (z_plus) height += (_manualPlacementPrecision * 25.0f);
			if (z_minus) height -= (_manualPlacementPrecision * 25.0f);

			if (length < 0.01f) length = 0.01f;
			if (width < 0.01f) width = 0.01f;
			if (height < 0.01f) height = 0.01f;

			GTAmemory::WriteFloat(ptr + 0x60, length);
			GTAmemory::WriteFloat(ptr + 0x74, width);
			GTAmemory::WriteFloat(ptr + 0x88, height);
		}

		void Sub_QuickManualPlacement()
		{
			if (SpoonerMode::bIsSomethingHeld)
			{
				Menu::SetPreviousMenu();
				Game::Print::PrintBottomCentre("~r~Error:~s~ There is an entity held in Spooner Mode.");
				return;
			}

			auto currIndexInDb = EntityManagement::GetEntityIndexInDb(selectedEntity);
			if (currIndexInDb < 0 || !selectedEntity.handle.Exists())
			{
				bool bFoundExistingEntity = false;
				for (UINT i = 0; i < Databases::EntityDb.size(); i++)
				{
					if (Databases::EntityDb[i].handle.Exists())
					{
						currIndexInDb = i;
						selectedEntity = Databases::EntityDb[currIndexInDb];
						bFoundExistingEntity = true;
						break;
					}
				}
				if (!bFoundExistingEntity)
				{
					Menu::SetPreviousMenu();
					Game::Print::PrintBottomCentre("~r~Error:~s~ No valid entities found in the database.");
					return;
				}
			}


			AddTitle("Manual Placement");

			bool bPropertiesSubPressed = false, bEnt_plus = false, bEnt_minus = false;
			AddTexter(selectedEntity.hashName, currIndexInDb, std::vector<std::string>{}, bPropertiesSubPressed, bEnt_plus, bEnt_minus);
			if (*Menu::currentopATM == Menu::printingop)
				EntityManagement::ShowArrowAboveEntity(selectedEntity.handle, RGBA(51, 153, 255, 200));
			if (bPropertiesSubPressed)
			{
				Menu::SetSub_delayed = SUB::SPOONER_SELECTEDENTITYOPS;
			}
			else if (bEnt_plus)
			{
				auto newIndexInDb = currIndexInDb + 1;
				while (newIndexInDb < Databases::EntityDb.size())
				{
					if (Databases::EntityDb[newIndexInDb].handle.Exists())
					{
						currIndexInDb = newIndexInDb;
						selectedEntity = Databases::EntityDb[currIndexInDb];
						break;
					}
					newIndexInDb++;
				}
			}
			else if (bEnt_minus)
			{
				auto newIndexInDb = currIndexInDb - 1;
				while (newIndexInDb >= 0)
				{
					if (Databases::EntityDb[newIndexInDb].handle.Exists())
					{
						currIndexInDb = newIndexInDb;
						selectedEntity = Databases::EntityDb[currIndexInDb];
						break;
					}
					newIndexInDb--;
				}
			}

			// It's the normal ManualPlacement stuff from here on

			selectedEntity.handle.RequestControlOnce();

			Vector3 currPos = selectedEntity.handle.GetPosition();
			Vector3 currRot = selectedEntity.handle.Rotation_get();
			Vector3 nextPos = currPos;
			Vector3 nextRot = currRot;

			bool prec_plus = 0, prec_minus = 0,
				x_plus = 0, x_minus = 0,
				y_plus = 0, y_minus = 0,
				z_plus = 0, z_minus = 0,
				pitch_plus = 0, pitch_minus = 0,
				roll_plus = 0, roll_minus = 0,
				yaw_plus = 0, yaw_minus = 0;

			AddNumber("Scroll Sensitivity", _manualPlacementPrecision, 4, null, prec_minus, prec_plus);
			AddNumber("X", currPos.x, 4, null, x_plus, x_minus);
			AddNumber("Y", currPos.y, 4, null, y_plus, y_minus);
			AddNumber("Z", currPos.z, 4, null, z_plus, z_minus);
			AddNumber("Pitch", currRot.x, 4, null, pitch_plus, pitch_minus);
			AddNumber("Roll", currRot.y, 4, null, roll_plus, roll_minus);
			AddNumber("Yaw", currRot.z, 4, null, yaw_plus, yaw_minus);
			AddOption("Other Properites", null, nullFunc, SUB::SPOONER_SELECTEDENTITYOPS);

			if (prec_plus) { if (_manualPlacementPrecision < 10.0f) _manualPlacementPrecision *= 10; }
			if (prec_minus) { if (_manualPlacementPrecision > 0.0001f) _manualPlacementPrecision /= 10; }

			if (x_plus) nextPos.x += _manualPlacementPrecision;
			if (x_minus) nextPos.x -= _manualPlacementPrecision;
			if (y_plus) nextPos.y += _manualPlacementPrecision;
			if (y_minus) nextPos.y -= _manualPlacementPrecision;
			if (z_plus) nextPos.z += _manualPlacementPrecision;
			if (z_minus) nextPos.z -= _manualPlacementPrecision;

			if (pitch_plus) { nextRot.x += _manualPlacementPrecision; if (nextRot.x > 180.0f) nextRot.x -= 360.0f; }
			if (pitch_minus) { nextRot.x -= _manualPlacementPrecision; if (nextRot.x < -180.0f) nextRot.x += 360.0f; }
			if (roll_plus) { nextRot.y += _manualPlacementPrecision; if (nextRot.y > 180.0f) nextRot.y -= 360.0f; }
			if (roll_minus) { nextRot.y -= _manualPlacementPrecision; if (nextRot.y < -180.0f) nextRot.y += 360.0f; }
			if (yaw_plus) { nextRot.z += _manualPlacementPrecision; if (nextRot.z > 180.0f) nextRot.z -= 360.0f; }
			if (yaw_minus) { nextRot.z -= _manualPlacementPrecision; if (nextRot.z < -180.0f) nextRot.z += 360.0f; }

			if (nextPos != currPos) selectedEntity.handle.SetPosition(nextPos);
			if (nextRot != currRot) selectedEntity.handle.SetRotation(nextRot);
		}
		void Sub_Vector3_ManualPlacement()
		{
			if (std::get<1>(SpoonerVector3ManualPlacementPtrs) == nullptr && std::get<2>(SpoonerVector3ManualPlacementPtrs) == nullptr)
			{
				Menu::SetPreviousMenu();
				return;
			}

			Vector3 markerPos = *std::get<1>(SpoonerVector3ManualPlacementPtrs);
			if (std::get<0>(SpoonerVector3ManualPlacementPtrs).Exists())
			{
				markerPos = std::get<0>(SpoonerVector3ManualPlacementPtrs).GetOffsetInWorldCoords(markerPos);
			}
			World::DrawLightWithRange(markerPos, g_fadedRGB, 2.3f, 1.5f);
			World::DrawMarker(MarkerType::DebugSphere, markerPos, Vector3(), Vector3(), Vector3(0.1f, 0.1f, 0.1f), g_fadedRGB.ToRGBA(190));

			AddTitle("Manual Placement");

			bool prec_plus = 0, prec_minus = 0,
				x_plus = 0, x_minus = 0,
				y_plus = 0, y_minus = 0,
				z_plus = 0, z_minus = 0,
				pitch_plus = 0, pitch_minus = 0,
				roll_plus = 0, roll_minus = 0,
				yaw_plus = 0, yaw_minus = 0;

			AddNumber("Scroll Sensitivity", _manualPlacementPrecision, 4, null, prec_minus, prec_plus);
			if (prec_plus) { if (_manualPlacementPrecision < 10.0f) _manualPlacementPrecision *= 10; }
			if (prec_minus) { if (_manualPlacementPrecision > 0.0001f) _manualPlacementPrecision /= 10; }

			if (std::get<1>(SpoonerVector3ManualPlacementPtrs) != nullptr)
			{
				Vector3& nextPos = *std::get<1>(SpoonerVector3ManualPlacementPtrs);
				const Vector3& currPos = nextPos;

				AddNumber("X", currPos.x, 4, null, x_plus, x_minus);
				AddNumber("Y", currPos.y, 4, null, y_plus, y_minus);
				AddNumber("Z", currPos.z, 4, null, z_plus, z_minus);
				if (x_plus) nextPos.x += _manualPlacementPrecision;
				if (x_minus) nextPos.x -= _manualPlacementPrecision;
				if (y_plus) nextPos.y += _manualPlacementPrecision;
				if (y_minus) nextPos.y -= _manualPlacementPrecision;
				if (z_plus) nextPos.z += _manualPlacementPrecision;
				if (z_minus) nextPos.z -= _manualPlacementPrecision;
			}
			if (std::get<2>(SpoonerVector3ManualPlacementPtrs) != nullptr)
			{
				Vector3& nextRot = *std::get<2>(SpoonerVector3ManualPlacementPtrs);
				const Vector3& currRot = nextRot;

				AddNumber("Pitch", currRot.x, 4, null, pitch_plus, pitch_minus);
				AddNumber("Roll", currRot.y, 4, null, roll_plus, roll_minus);
				AddNumber("Yaw", currRot.z, 4, null, yaw_plus, yaw_minus);
				if (pitch_plus) { nextRot.x += _manualPlacementPrecision; if (nextRot.x > 180.0f) nextRot.x -= 360.0f; }
				if (pitch_minus) { nextRot.x -= _manualPlacementPrecision; if (nextRot.x < -180.0f) nextRot.x += 360.0f; }
				if (roll_plus) { nextRot.y += _manualPlacementPrecision; if (nextRot.y > 180.0f) nextRot.y -= 360.0f; }
				if (roll_minus) { nextRot.y -= _manualPlacementPrecision; if (nextRot.y < -180.0f) nextRot.y += 360.0f; }
				if (yaw_plus) { nextRot.z += _manualPlacementPrecision; if (nextRot.z > 180.0f) nextRot.z -= 360.0f; }
				if (yaw_minus) { nextRot.z -= _manualPlacementPrecision; if (nextRot.z < -180.0f) nextRot.z += 360.0f; }
			}

		}
		void Sub_GroupSpoon()
		{
			auto& vGroup = selectedSpoonGroup;
			SpoonerEntity refEnt;
			bool bEntitiesExist = false;
			for (auto it = vGroup.begin(); it != vGroup.end();)
			{
				if (!it->handle.Exists())
				{
					it = vGroup.erase(it);
				}
				else
				{
					if (!bEntitiesExist)
					{
						bEntitiesExist = true;
						refEnt = *it;
					}
					++it;
				}
			}

			AddTitle("Multiple Entities");

			AddOption("Select Entities", null, nullFunc, SUB::SPOONER_GROUPSPOON_SELECTENTITIES);

			if (bEntitiesExist)
			{
				//=================================================================================

				AddBreak("---Place---");

				bool isOnTheLine = NETWORK_IS_IN_SESSION() != 0;
				Vector3 refPos = refEnt.handle.GetPosition();
				Vector3 refRot = refEnt.handle.Rotation_get();
				Vector3 nextPosOffset;
				Vector3 nextRotOffset;

				bool prec_plus = 0, prec_minus = 0,
					x_plus = 0, x_minus = 0,
					y_plus = 0, y_minus = 0,
					z_plus = 0, z_minus = 0,
					pitch_plus = 0, pitch_minus = 0,
					roll_plus = 0, roll_minus = 0,
					yaw_plus = 0, yaw_minus = 0;

				AddNumber("Scroll Sensitivity", _manualPlacementPrecision, 4, null, prec_minus, prec_plus);
				AddNumber("X", refPos.x, 4, null, x_plus, x_minus);
				AddNumber("Y", refPos.y, 4, null, y_plus, y_minus);
				AddNumber("Z", refPos.z, 4, null, z_plus, z_minus);
				AddNumber("Pitch", refRot.x, 4, null, pitch_plus, pitch_minus);
				AddNumber("Roll", refRot.y, 4, null, roll_plus, roll_minus);
				AddNumber("Yaw", refRot.z, 4, null, yaw_plus, yaw_minus);

				if (prec_plus) { if (_manualPlacementPrecision < 10.0f) _manualPlacementPrecision *= 10; }
				if (prec_minus) { if (_manualPlacementPrecision > 0.0001f) _manualPlacementPrecision /= 10; }

				if (x_plus) nextPosOffset.x += _manualPlacementPrecision;
				if (x_minus) nextPosOffset.x -= _manualPlacementPrecision;
				if (y_plus) nextPosOffset.y += _manualPlacementPrecision;
				if (y_minus) nextPosOffset.y -= _manualPlacementPrecision;
				if (z_plus) nextPosOffset.z += _manualPlacementPrecision;
				if (z_minus) nextPosOffset.z -= _manualPlacementPrecision;

				if (pitch_plus) { nextRotOffset.x += _manualPlacementPrecision; if (nextRotOffset.x > 180.0f) nextRotOffset.x -= 360.0f; }
				if (pitch_minus) { nextRotOffset.x -= _manualPlacementPrecision; if (nextRotOffset.x < -180.0f) nextRotOffset.x += 360.0f; }
				if (roll_plus) { nextRotOffset.y += _manualPlacementPrecision; if (nextRotOffset.y > 180.0f) nextRotOffset.y -= 360.0f; }
				if (roll_minus) { nextRotOffset.y -= _manualPlacementPrecision; if (nextRotOffset.y < -180.0f) nextRotOffset.y += 360.0f; }
				if (yaw_plus) { nextRotOffset.z += _manualPlacementPrecision; if (nextRotOffset.z > 180.0f) nextRotOffset.z -= 360.0f; }
				if (yaw_minus) { nextRotOffset.z -= _manualPlacementPrecision; if (nextRotOffset.z < -180.0f) nextRotOffset.z += 360.0f; }

				if (!nextPosOffset.IsZero())
				{
					for (auto& e : vGroup)
					{
						if (isOnTheLine)
							e.handle.RequestControl(100);
						e.handle.SetPosition(e.handle.GetPosition() + nextPosOffset);
					}
				}
				if (!nextRotOffset.IsZero())
				{
					for (auto& e : vGroup)
					{
						if (isOnTheLine)
							e.handle.RequestControl(100);
						e.handle.SetRotation(e.handle.Rotation_get() + nextRotOffset);
					}
				}

				//=================================================================================

				AddBreak("---Task Sequences---");

				bool bStartTaskSequences = false;
				AddOption("Start Task Sequences", bStartTaskSequences); if (bStartTaskSequences)
				{
					for (auto& e : vGroup)
					{
						auto eiidb = EntityManagement::GetEntityIndexInDb(e);
						if (eiidb >= 0)
						{
							if (isOnTheLine)
								e.handle.RequestControl();
							Databases::EntityDb[eiidb].taskSequence.Start();
						}
					}
				}

				bool bStopTaskSequences = false;
				AddOption("Stop Task Sequences", bStopTaskSequences); if (bStopTaskSequences)
				{
					for (auto& e : vGroup)
					{
						auto eiidb = EntityManagement::GetEntityIndexInDb(e);
						if (eiidb >= 0)
						{
							if (e.handle.IsPed())
							{
								if (isOnTheLine)
									e.handle.RequestControl();
								TASK_CLEAR_LOOK_AT(e.handle.Handle());
								CLEAR_PED_TASKS_IMMEDIATELY(e.handle.Handle());
							}
							Databases::EntityDb[eiidb].taskSequence.Reset();
						}
					}
				}

				AddBreak("---Edit---");

				int opacityLevel = refEnt.handle.GetAlpha();
				bool bOpacity_plus = false, bOpacity_minus = false;
				AddNumber("Opacity (Local)", opacityLevel, 0, null, bOpacity_plus, bOpacity_minus);
				if (bOpacity_plus) { if (opacityLevel < 255) opacityLevel++; else opacityLevel = 0; for (auto& e : vGroup) { if (isOnTheLine) e.handle.RequestControl(); e.handle.SetAlpha(opacityLevel); } }
				if (bOpacity_minus) { if (opacityLevel > 0) opacityLevel--; else opacityLevel = 255; for (auto& e : vGroup) { if (isOnTheLine) e.handle.RequestControl(); e.handle.SetAlpha(opacityLevel); } }

				AddOption("Attach To Something", null, nullFunc, SUB::SPOONER_GROUPSPOON_ATTACHTO);
				bool bDetachPressed = false;
				AddOption("Detach", bDetachPressed); if (bDetachPressed)
				{
					for (auto& e : vGroup)
					{
						if (isOnTheLine)
							e.handle.RequestControl(400);
						EntityManagement::DetachEntity(e);
					}
				}

				bool bCopyPressed = false, bCopy_plus = false, bCopy_minus = false;
				AddTexter("Copy", _copyEntTexterValue, std::vector<std::string>{ "Selected Only", "Copy With Attachments" }, bCopyPressed, bCopy_plus, bCopy_minus);
				if (bCopy_plus) { if (_copyEntTexterValue < 1U) _copyEntTexterValue++; }
				if (bCopy_minus) { if (_copyEntTexterValue > 0) _copyEntTexterValue--; }
				if (bCopyPressed)
				{
					for (auto& e : vGroup)
					{
						const SpoonerEntity& copiedEntity = EntityManagement::CopyEntity(e, true, true, _copyEntTexterValue);
						//EntityManagement::AddEntityToDb(copiedEntity);
						//e = copiedEntity;
					}
				}
				bool bDeletePressed = false;
				AddOption("Delete", bDeletePressed); if (bDeletePressed)
				{
					for (auto& e : vGroup)
					{
						if (isOnTheLine)
							e.handle.RequestControl(400);
						EntityManagement::DeleteEntity(e);
					}
					vGroup.clear();
					*Menu::currentopATM = 1;
				}

			}
		}
		void Sub_GroupSpoon_SelectEntities()
		{
			auto& vGroup = selectedSpoonGroup;

			AddTitle("Select Entities");

			bool bClearGroupPressed = false;
			AddTickol("CLEAR SELECTION", true, bClearGroupPressed, bClearGroupPressed, TICKOL::CROSS, TICKOL::CROSS); if (bClearGroupPressed)
			{
				vGroup.clear();
			}
			bool bSelectAllPressed = false;
			AddTickol("SELECT ALL", vGroup == Databases::EntityDb, bSelectAllPressed, bSelectAllPressed, TICKOL::TICK2, TICKOL::NONE); if (bSelectAllPressed)
			{
				if (vGroup != Databases::EntityDb)
					vGroup = Databases::EntityDb;
				else
					vGroup.clear();
			}

			for (auto& e : Databases::EntityDb)
			{
				auto grpIt = std::find(vGroup.begin(), vGroup.end(), e);
				bool bEntityIsInGroup = grpIt != vGroup.end();
				bool bEntityPressed = false;
				AddTickol(e.hashName + (e.handle.Exists() ? "" : " (Invalid)"), bEntityIsInGroup, bEntityPressed, bEntityPressed, TICKOL::BOXTICK, TICKOL::BOXBLANK);
				if (*Menu::currentopATM == Menu::printingop) EntityManagement::ShowArrowAboveEntity(e.handle, RGBA(127, 0, 255, 200));
				if (bEntityPressed)
				{
					if (bEntityIsInGroup)
					{
						vGroup.erase(grpIt);
					}
					else
					{
						vGroup.push_back(e);
					}
				}
			}
		}
		void Sub_GroupSpoon_AttachTo()
		{
			auto& vGroup = selectedSpoonGroup;
			GTAped myPed = PLAYER_PED_ID();
			GTAvehicle myVehicle = g_myVeh;
			bool isOnTheLine = NETWORK_IS_IN_SESSION() != 0;

			AddTitle("Attach To Something");

			bool bToggleKeepPosWhenAttaching = false;
			AddTickol("Keep World Position When Attaching", Settings::bKeepPositionWhenAttaching, bToggleKeepPosWhenAttaching, bToggleKeepPosWhenAttaching, TICKOL::BOXTICK, TICKOL::BOXBLANK); if (bToggleKeepPosWhenAttaching)
				Settings::bKeepPositionWhenAttaching = !Settings::bKeepPositionWhenAttaching;

			AddBreak("---Available Entities---");

			bool bSelf = false;
			AddOption("Self", bSelf); if (bSelf)
			{
				for (auto& e : vGroup)
				{
					if (e.handle.Exists())
					{
						if (isOnTheLine)
							e.handle.RequestControl(400);
						EntityManagement::AttachEntityInit(e, myPed, Settings::bKeepPositionWhenAttaching);
					}
				}
				Menu::SetPreviousMenu();
				return;
			}

			if (myVehicle.Exists())
			{
				bool bSelfVeh = false;
				AddOption(std::string(myPed.IsInVehicle() ? "Current" : "Last Seated") + " Vehicle", bSelfVeh); if (bSelfVeh)
				{
					for (auto& e : vGroup)
					{
						if (e.handle.Exists())
						{
							if (isOnTheLine)
								e.handle.RequestControl(400);
							EntityManagement::AttachEntityInit(e, myVehicle, Settings::bKeepPositionWhenAttaching);
						}
					}
					Menu::SetPreviousMenu();
					return;
				}
			}

			if (!Databases::EntityDb.empty())
			{
				if (Databases::EntityDb.size() > 1 || std::find(vGroup.begin(), vGroup.end(), Databases::EntityDb.front()) == vGroup.end())
				{
					AddBreak("---Database---");
					for (auto& e : Databases::EntityDb)
					{
						if (std::find(vGroup.begin(), vGroup.end(), e) == vGroup.end())
						{
							if (e.handle.Exists())
							{
								bool bEntityPressed = false;
								AddOption(e.hashName, bEntityPressed); if (bEntityPressed)
								{
									for (auto& eig : vGroup)
									{
										if (eig.handle.Exists())
										{
											if (isOnTheLine)
												eig.handle.RequestControl(400);
											EntityManagement::AttachEntityInit(eig, e.handle, Settings::bKeepPositionWhenAttaching);
										}
									}
									Menu::SetPreviousMenu();
									return;
								}

								if (*Menu::currentopATM == Menu::printingop)
									EntityManagement::ShowArrowAboveEntity(e.handle, RGBA(0, 255, 0, 200));
							}
							else
							{
								AddOption(e.hashName + " (Invalid)", null);
							}
						}
					}
				}
			}
		}

		void Sub_PedOps()
		{
			if (!selectedEntity.handle.Exists())
			{
				Menu::SetPreviousMenu();
				return;
			}
			selectedEntity.handle.RequestControlOnce();
			auto thisEntityIndexInDb = EntityManagement::GetEntityIndexInDb(selectedEntity);
			bool isThisEntityInDb = thisEntityIndexInDb >= 0;
			if (isThisEntityInDb)
			{
				selectedEntity.taskSequence = Databases::EntityDb[thisEntityIndexInDb].taskSequence;
				Databases::EntityDb[thisEntityIndexInDb] = selectedEntity;
			}

			GTAped myPed = PLAYER_PED_ID();
			GTAped thisPed = selectedEntity.handle;
			bool butAmIOnline = NETWORK_IS_IN_SESSION() != 0;
			bool isPedMyPed = thisPed.Handle() == myPed.Handle();
			bool bIsPedShortHeighted = GET_PED_CONFIG_FLAG(thisPed.Handle(), ePedConfigFlags::_Shrink, false) != 0;
			PedGroup myPedGroup = myPed.GetCurrentPedGroup();

			bool pedops_isStill_toggle = false,
				pedops_canRagdoll_toggle = false,
				pedops_shortHeighted_toggle = false,
				pedops_friend = false,
				pedops_burn = false,
				pedops_piggyback = false,
				pedops_shoulderRide = false,
				pedops_warp_nearest = false,
				pedops_wp_walk = false,
				pedops_become_ped = false;

			AddTitle("Ped Options");

			if (!isPedMyPed)
				MenuOptions::AddOption_RelationshipTextScroller();
			AddToggle("Is Still (Block Fleeing)", selectedEntity.isStill, pedops_isStill_toggle, pedops_isStill_toggle); if (pedops_isStill_toggle) { thisPed.SetBlockPermanentEvent(selectedEntity.isStill); }
			AddLocal("Can Ragdoll", thisPed.GetCanRagdoll(), pedops_canRagdoll_toggle, pedops_canRagdoll_toggle); if (pedops_canRagdoll_toggle) { bool ns = !thisPed.GetCanRagdoll(); thisPed.SetCanRagdoll(ns); SET_PED_RAGDOLL_ON_COLLISION(thisPed.Handle(), ns); }
			AddLocal("Is Short Heighted (Small)", bIsPedShortHeighted, pedops_shortHeighted_toggle, pedops_shortHeighted_toggle); if (pedops_shortHeighted_toggle) { SET_PED_CONFIG_FLAG(selectedEntity.handle.Handle(), ePedConfigFlags::_Shrink, bIsPedShortHeighted ? 0 : 1); }

			int thisArmour = thisPed.GetArmour();
			bool bArmour_plus = false, bArmour_minus = false, bArmour_input = false;
			AddNumber("Armour", thisArmour, 0, bArmour_input, bArmour_plus, bArmour_minus);
			if (bArmour_plus) { if (thisArmour < INT_MAX) { thisArmour++; thisPed.SetArmour(thisArmour); } }
			if (bArmour_minus) { if (thisArmour > 0) { thisArmour--;  thisPed.SetArmour(thisArmour); } }
			if (bArmour_input)
			{
				std::string inputStr = Game::InputBox("", 5U, "", std::to_string(thisArmour));
				if (inputStr.length() > 0)
				{
					try
					{
						thisArmour = stoi(inputStr);
						thisPed.SetArmour(thisArmour);
					}
					catch (...) { Game::Print::PrintErrorInvalidInput(inputStr); }
				}
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SpoonerEoArmour, std::string(), 4U, std::string(), std::to_string(thisHealth));
				//OnscreenKeyboard::State::arg1._int = ent.GetHandle();
			}

			AddOption("Wardrobe", null, SetEnt241, SUB::COMPONENTS);
			if (g_cam_componentChanger.Exists())
			{
				g_cam_componentChanger.SetActive(false);
				g_cam_componentChanger.Destroy();
				World::SetRenderingCamera(0);
			}

			AddOption("Animations", null, SetEnt241, SUB::ANIMATIONSUB);
			AddOption("Scenario Actions", null, SetEnt241, SUB::AnimationTaskScenarios);
			AddOption("Moods", null, SetEnt241, SUB::FACIALMOOD);
			AddOption("Movement Styles", null, SetEnt241, SUB::MOVEMENTGROUP);
			AddOption("Weapon", null, nullFunc, SUB::SPOONER_PEDOPS_WEAPON);
			AddOption("Speech Player  (Doesn't Save)", null, SetEnt241, SUB::SPEECHPLAYER);
			AddOption("Voice Changer  (Doesn't Save)", null, SetEnt241, SUB::VOICECHANGER);
			AddOption("Explosions  (Doesn't Save)", null, SetEnt241, SUB::PEDEXPLOSIONSUB);
			//AddOption("Ped Flags (Doesn't Save)", null, SetEnt241, SUB::PEDFLAGMANAGER_NAMEDLIST);
			//AddOption("Give Vehicle", null, SetEnt241, SUB::SPAWNVEHICLE);
			AddOption("Attach Objects (Doesn't Save)", null, SetEnt241, SUB::ATTACHFUNNYOBJECTSUB);
			AddLocal("Companion (7 Max) (Doesn't Save) (Obsolete)", myPedGroup.Contains(thisPed), pedops_friend, pedops_friend);
			AddLocal("Burn Ped", thisPed.IsOnFire(), pedops_burn, pedops_burn);
			if (!isPedMyPed)
				AddLocal("Piggyback Ride  (Doesn't Save)", (GET_ENTITY_ATTACHED_TO(myPed.Handle() == thisPed.Handle()) && IS_ENTITY_PLAYING_ANIM(myPed.Handle(), "mini@prostitutes@sexnorm_veh", "bj_loop_male", 3)), pedops_piggyback, pedops_piggyback);
			if (!isPedMyPed)
				AddLocal("Shoulder Ride  (Doesn't Save)", (GET_ENTITY_ATTACHED_TO(myPed.Handle() == thisPed.Handle()) && IS_ENTITY_PLAYING_ANIM(myPed.Handle(), "amb@prop_human_seat_chair@male@elbows_on_knees@idle_a", "idle_a", 3)), pedops_shoulderRide, pedops_shoulderRide);
			//AddOption("Warp Into Nearest Car", pedops_warp_nearest);
			AddOption("Travel To Waypoint", pedops_wp_walk);
			if (!isPedMyPed) AddOption("Become This Ped (Soul-Steal) (SP)", butAmIOnline ? null : pedops_become_ped);


			if (pedops_friend)
			{
				NETWORK_REQUEST_CONTROL_OF_NETWORK_ID(thisPed.NetID());
				if (myPedGroup.Exists())
				{
					if (!myPedGroup.Contains(thisPed))
					{
						myPedGroup.Add(thisPed, false);
						myPedGroup.SetSeparationRange(100.0f);
						myPedGroup.SetFormationSpacing(1.5f);
						thisPed.Task().FightAgainstHatedTargets(400.0f);
					}
					else
					{
						myPedGroup.Remove(thisPed);
					}
				}
				else
				{
					myPedGroup = PedGroup::CreateNewGroup();
					myPedGroup.Add(myPed, true);
					myPedGroup.SetSeparationRange(100.0f);
					myPedGroup.SetFormationSpacing(1.5f);
				}
			}

			if (pedops_burn) { thisPed.SetOnFire(!thisPed.IsOnFire()); }

			if (pedops_piggyback)
			{
				if (thisPed == myPed)
					Game::Print::PrintBottomCentre("~r~Error:~s~ Can't do that to yourself.");
				else
				{
					if (GET_ENTITY_ATTACHED_TO(myPed.Handle()) != thisPed.Handle())
					{
						thisPed.RequestControl();
						myPed.AttachTo(thisPed, -1, false, Vector3(0.0f, -0.3f, 0.0f), Vector3(0, 0, 0));
						myPed.Task().PlayAnimation("mini@prostitutes@sexnorm_veh", "bj_loop_male", 8.0f, 0.0f, -1, 9, 0, false);
					}
					else
					{
						myPed.Detach();
						myPed.Task().ClearAllImmediately();
					}
				}
			}

			if (pedops_shoulderRide)
			{
				if (thisPed == myPed)
					Game::Print::PrintBottomCentre("~r~Error:~s~ Can't do that to yourself.");
				else
				{
					if (GET_ENTITY_ATTACHED_TO(myPed.Handle()) != thisPed.Handle())
					{
						//set_ped_shoulder_ride_ped(myPed, thisPed, true);
						myPed.AttachTo(thisPed, Bone::SKEL_ROOT, Vector3(0, -0.25, 0.35f), Vector3(45.0f, 0, 0), true, true, false, true, 1, true);
						myPed.Task().PlayAnimation("amb@prop_human_seat_chair@male@elbows_on_knees@idle_a", "idle_a", 1000.0f, -1.5f, -1, AnimFlag::Loop, 0.445f, false);
					}
					else
					{
						//set_ped_shoulder_ride_ped(myPed, thisPed, false);
						myPed.Detach();
						myPed.Task().ClearAllImmediately();
					}
				}
			}

			if (pedops_warp_nearest)
			{
				const auto& closestVeh = World::GetClosestVehicle(thisPed.GetPosition(), 100.0f);
				if (closestVeh.Exists())
					thisPed.SetIntoVehicle(closestVeh, closestVeh.FirstFreeSeat(SEAT_DRIVER));
				else
					Game::Print::PrintBottomCentre("~r~Error:~s~ Unable to find a vehicle.");
			}

			if (pedops_wp_walk)
			{
				if (!IS_WAYPOINT_ACTIVE()) Game::Print::PrintBottomCentre("~r~Error:~s~ No Waypoint Set.");
				else
				{
					Vector3 coord = GET_BLIP_INFO_ID_COORD(GET_FIRST_BLIP_INFO_ID(BlipIcon::Waypoint));
					coord.z = World::GetGroundHeight(coord);
					//thisPed.Task().GoTo(coord);
					TASK_GO_TO_COORD_ANY_MEANS(thisPed.Handle(), coord.x, coord.y, coord.z, 3.0f, 0, 0, 786603, -1082130432);
				}
			}

			if (pedops_become_ped) { SetBecomePed(thisPed); }

		}
		void Sub_PedOps_Weapon()
		{
			auto& selectedCategoryIndex = msCurrentPaintIndex;
			GTAped myPed = PLAYER_PED_ID();
			GTAped thisPed = selectedEntity.handle;
			Hash pedCurrWeapon = thisPed.GetWeapon();
			bool isPedMyPed = thisPed.Handle() == myPed.Handle();

			AddTitle("Weapon");

			bool bSetToNonePressed = false;
			AddTickol("None", pedCurrWeapon == WEAPON_UNARMED, bSetToNonePressed, bSetToNonePressed); if (bSetToNonePressed)
			{
				pedCurrWeapon = WEAPON_UNARMED;
				SET_CURRENT_PED_WEAPON(thisPed.Handle(), pedCurrWeapon, true);
			}

			bool bMyCurrWeapon = false;
			if (!isPedMyPed) AddOption("Your Current Weapon", bMyCurrWeapon); if (bMyCurrWeapon)
			{
				//std::vector<s_Weapon_Components_Tint> weaponsBackup;
				//myPed.StoreWeaponsInArray(weaponsBackup);
				//GTAped(thisPed).GiveWeaponsFromArray(weaponsBackup);
				Hash weaponHash = myPed.GetWeapon();
				GIVE_DELAYED_WEAPON_TO_PED(thisPed.Handle(), weaponHash, 9999, true);
				GIVE_WEAPON_TO_PED(thisPed.Handle(), weaponHash, 1, true, false);
				int ammo;
				GET_MAX_AMMO(thisPed.Handle(), weaponHash, &ammo);
				SET_PED_AMMO(thisPed.Handle(), weaponHash, ammo, 0);
				SET_AMMO_IN_CLIP(thisPed.Handle(), weaponHash, GET_MAX_AMMO_IN_CLIP(thisPed.Handle(), weaponHash, true));
				SET_CURRENT_PED_WEAPON(thisPed.Handle(), weaponHash, true);
			}

			for (UINT i = 0; i < WeaponIndivs::vCategoryNames.size(); i++)
			{
				bool bCategoryPressed = false;
				AddOption(WeaponIndivs::vCategoryNames[i], bCategoryPressed, nullFunc, SUB::SPOONER_PEDOPS_WEAPON_INCATEGORY); if (bCategoryPressed)
				{
					selectedCategoryIndex = i;
				}
			}
		}
		void Sub_PedOps_Weapon_InCategory()
		{
			auto& selectedCategoryIndex = msCurrentPaintIndex;
			AddTitle(WeaponIndivs::vCategoryNames[selectedCategoryIndex]);

			GTAped thisPed = selectedEntity.handle;
			Hash currentWeapon = thisPed.GetWeapon();

			for (auto& wc : *WeaponIndivs::vAllWeapons[selectedCategoryIndex])
			{
				bool bWeapPressed = false;
				AddTickol(get_weapon_label(wc.weaponHash, true), currentWeapon == wc.weaponHash, bWeapPressed, bWeapPressed); if (bWeapPressed)
				{
					GIVE_DELAYED_WEAPON_TO_PED(thisPed.Handle(), wc.weaponHash, 9999, true);
					GIVE_WEAPON_TO_PED(thisPed.Handle(), wc.weaponHash, 1, true, false);
					int ammo;
					GET_MAX_AMMO(thisPed.Handle(), wc.weaponHash, &ammo);
					SET_PED_AMMO(thisPed.Handle(), wc.weaponHash, ammo, 0);
					SET_AMMO_IN_CLIP(thisPed.Handle(), wc.weaponHash, GET_MAX_AMMO_IN_CLIP(thisPed.Handle(), wc.weaponHash, true));
					SET_CURRENT_PED_WEAPON(thisPed.Handle(), wc.weaponHash, true);
				}
			}
		}

		void Sub_ManageMarkers()
		{
			int markerIndexInDbToDelete = -1;

			AddTitle("Markers");

			AddOption("Removal", null, nullFunc, SUB::SPOONER_MANAGEMARKERS_REMOVAL);

			AddBreak("---Database---");
			for (UINT i = 0; i < Databases::MarkerDb.size(); i++)
			{
				auto& m = Databases::MarkerDb[i];
				bool bMarkerPressed = false;

				AddOption(m.m_name, bMarkerPressed); if (bMarkerPressed)
				{
					SelectedMarker = &m;
					Menu::SetSub_delayed = SUB::SPOONER_MANAGEMARKERS_INMARKER;
				}


				if (*Menu::currentopATM == Menu::printingop)
				{
					m.m_selectedInSub = true;

					bool bShortcutDeletePressed;
					if (Menu::bitController)
					{
						Menu::add_IB(INPUT_SCRIPT_RLEFT, "Delete Marker");
						bShortcutDeletePressed = IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_SCRIPT_RLEFT) != 0;
					}
					else
					{
						Menu::add_IB(VirtualKey::B, "Delete Marker");
						bShortcutDeletePressed = IsKeyJustUp(VirtualKey::B);
					}

					if (bShortcutDeletePressed)
					{
						markerIndexInDbToDelete = i;
					}
				}
			}

			bool bAddNewMarkerPressed = false;
			AddTickol("ADD NEW MARKER", true, bAddNewMarkerPressed, bAddNewMarkerPressed, TICKOL::SMALLNEWSTAR); if (bAddNewMarkerPressed)
			{
				auto& spoocam = SpoonerMode::spoonerModeCamera;
				if (!spoocam.IsActive())
				{
					GTAentity myPed = PLAYER_PED_ID();
					Vector3 myPos = myPed.GetPosition();
					SelectedMarker = MarkerManagement::AddMarker(myPos, Vector3(0, 0, myPed.GetHeading()));
				}
				else
				{
					Vector3 spawnPos = spoocam.RaycastForCoord(Vector2(0.0f, 0.0f), 0, 120.0f, 30.0f + SpoonerMarker().m_scale / 2);
					spawnPos.z += SpoonerMarker().m_scale / 2;
					SelectedMarker = MarkerManagement::AddMarker(spawnPos, Vector3(0, 0, spoocam.GetRotation().z));
				}
				Menu::SetSub_delayed = SUB::SPOONER_MANAGEMARKERS_INMARKER;
			}


			if (markerIndexInDbToDelete != -1)
			{
				MarkerManagement::RemoveMarker(markerIndexInDbToDelete);
			}

			if (*Menu::currentopATM > Menu::printingop)
				Menu::Up(); // Go up if you're too far down due to removing a marker from DB from the properties menu and coming back here again.

		}
		void Sub_ManageMarkers_Removal()
		{
			auto& fMarkerRemovalRadius = _fSaveRangeRadius;
			GTAentity myPed = PLAYER_PED_ID();
			const Vector3& myPos = myPed.GetPosition();

			AddTitle("Removal");

			bool bInRange_plus = false, bInRange_minus = false, bInRange_execute = false;
			AddNumber("Delete Markers In Range", fMarkerRemovalRadius, 0, bInRange_execute, bInRange_plus, bInRange_minus);
			if (*Menu::currentopATM == Menu::printingop)
				EntityManagement::DrawRadiusDisplayingMarker(myPos, fMarkerRemovalRadius);
			if (bInRange_plus) { if (fMarkerRemovalRadius < FLT_MAX) fMarkerRemovalRadius += 1.0f; }
			if (bInRange_minus) { if (fMarkerRemovalRadius > 0.0f) fMarkerRemovalRadius -= 1.0f; }
			if (bInRange_execute)
			{
				MarkerManagement::RemoveAllMarkersInRange(myPos, fMarkerRemovalRadius);
			}

			AddOption("Delete All Markers (" + std::to_string(Databases::MarkerDb.size()) + ")", null, MarkerManagement::RemoveAllMarkers);
		}
		void Sub_ManageMarkers_InMarker()
		{
			if (SelectedMarker == nullptr)
			{
				Menu::SetPreviousMenu();
				return;
			}

			AddTitle(SelectedMarker->m_name);

			Vector3 finalPosition, finalRotation;
			if (SelectedMarker->m_attachmentArgs.attachedTo.Exists())
			{
				finalPosition = SelectedMarker->m_attachmentArgs.attachedTo.GetOffsetInWorldCoords(SelectedMarker->m_attachmentArgs.offset);
				finalRotation = SelectedMarker->m_attachmentArgs.attachedTo.Rotation_get() + SelectedMarker->m_attachmentArgs.rotation;
			}
			else
			{
				finalPosition = SelectedMarker->m_position;
				finalRotation = SelectedMarker->m_rotation;
			}

			SpoonerMarkerPosition* dest = SelectedMarker->m_destinationPtr == nullptr ? &SelectedMarker->m_destinationVal : SelectedMarker->m_destinationPtr;
			Vector3 finalDest;
			float finalDestHeading;
			if (dest->m_attachmentArgs.attachedTo.Exists())
			{
				finalDest = dest->m_attachmentArgs.attachedTo.GetOffsetInWorldCoords(dest->m_attachmentArgs.offset);
				finalDestHeading = dest->m_attachmentArgs.attachedTo.Rotation_get().z + SelectedMarker->m_destinationHeading;
			}
			else
			{
				finalDest = dest->m_position;
				finalDestHeading = SelectedMarker->m_destinationHeading;
			}

			if (!dest->m_position.IsZero())
			{
				World::DrawLine(finalPosition, finalDest, RGBA(SelectedMarker->m_colour, 200));
				World::DrawLightWithRange(finalDest, RgbS(SelectedMarker->m_colour), 2.3f, 1.5f);

				const Vector3& helpingSpherePos = finalDest.PointOnCircle(1.16f, finalDestHeading + 90.0f);
				World::DrawLine(finalDest, helpingSpherePos, RGBA(SelectedMarker->m_colour, 200));
				World::DrawMarker(MarkerType::DebugSphere, helpingSpherePos, Vector3(), Vector3(), Vector3(0.1f, 0.1f, 0.1f), RGBA(SelectedMarker->m_colour, 200));
			}

			auto& spoocam = SpoonerMode::spoonerModeCamera;

			bool bEditNamePressed = false;
			AddTexter("Name", 0, std::vector<std::string>{SelectedMarker->m_name}, bEditNamePressed); if (bEditNamePressed)
			{
				SelectedMarker->m_name = Game::InputBox(SelectedMarker->m_name, 26U, "Enter custom marker name:", SelectedMarker->m_name);
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SetArg1String, SelectedMarker->m_name, 26U, "Enter custom marker name:", SelectedMarker->m_name);
				//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&SelectedMarker->m_name);
			}

			bool bType_plus = false, bType_minus = false;
			AddTexter("Type", SelectedMarker->m_type, MarkerType::vNames, null, bType_plus, bType_minus);
			if (bType_plus) { if (SelectedMarker->m_type < MarkerType::vNames.size() - 1) SelectedMarker->m_type++; }
			if (bType_minus) { if (SelectedMarker->m_type > 0) SelectedMarker->m_type--; }

			bool bScale_plus = false, bScale_minus = false;
			AddNumber("Size", SelectedMarker->m_scale, 2, null, bScale_plus, bScale_minus);
			if (bScale_plus) { if (SelectedMarker->m_scale < 10.0f) SelectedMarker->m_scale += 0.05f; }
			if (bScale_minus) { if (SelectedMarker->m_scale > 0.0f) SelectedMarker->m_scale -= 0.05f; }

			bool bShowNameToggle = false;
			AddTickol("Show Name", SelectedMarker->m_showName, bShowNameToggle, bShowNameToggle, TICKOL::BOXTICK, TICKOL::BOXBLANK); if (bShowNameToggle) SelectedMarker->m_showName = !SelectedMarker->m_showName;

			bool bRotateContinuouslyToggle = false;
			AddTickol("Rotate Continuously", SelectedMarker->m_rotateContinuously, bRotateContinuouslyToggle, bRotateContinuouslyToggle, TICKOL::BOXTICK, TICKOL::BOXBLANK); if (bRotateContinuouslyToggle) SelectedMarker->m_rotateContinuously = !SelectedMarker->m_rotateContinuously;

			bool bAllowVehiclesToggle = false;
			AddTickol("Allow Vehicle Teleportation", SelectedMarker->m_allowVehicles, bAllowVehiclesToggle, bAllowVehiclesToggle, TICKOL::BOXTICK, TICKOL::BOXBLANK); if (bAllowVehiclesToggle) SelectedMarker->m_allowVehicles = !SelectedMarker->m_allowVehicles;

			AddsettingscolOption("Colour", SelectedMarker->m_colour);

			AddBreak("---Position---");
			{
				AddOption("~italic~" + finalPosition.ToString(), null);

				if (!spoocam.IsActive())
				{
					bool bSetPosToMe = false;
					AddOption("Set To Player Position", bSetPosToMe); if (bSetPosToMe)
					{
						Vector3 myPos = GTAentity(PLAYER_PED_ID()).GetPosition();
						SelectedMarker->m_position = myPos;
						SelectedMarker->m_attachmentArgs.attachedTo = 0;
					}
				}
				else
				{
					bool bSetPosToHitCoords = false;
					AddOption("Set To Camera Target", bSetPosToHitCoords); if (bSetPosToHitCoords)
					{
						Vector3 hitCoords = spoocam.RaycastForCoord(Vector2(0.0f, 0.0f), 0, 160.0f, 3.0f);
						SelectedMarker->m_position = hitCoords;
						SelectedMarker->m_attachmentArgs.attachedTo = 0;
					}
				}
				if (IS_WAYPOINT_ACTIVE())
				{
					bool bSetPosToWp = false;
					AddOption("Set To Waypoint", bSetPosToWp); if (bSetPosToWp)
					{
						GTAblip wpBlip = GET_FIRST_BLIP_INFO_ID(BlipIcon::Waypoint);
						Vector3 wpCoords = wpBlip.GetPosition();
						wpCoords.z = World::GetGroundHeight(wpCoords);
						SelectedMarker->m_position = wpCoords;
						SelectedMarker->m_attachmentArgs.attachedTo = 0;
					}
				}

				if (!Databases::EntityDb.empty())
				{
					AddOption("Attach To Entity", null, nullFunc, SUB::SPOONER_MANAGEMARKERS_INMARKER_ATTACH);
				}

				if (SelectedMarker->m_attachmentArgs.attachedTo.Exists())
				{
					bool bAdjustAttachmentForPosPressed = false;
					AddOption("Adjust Attachment", bAdjustAttachmentForPosPressed, nullFunc, SUB::SPOONER_VECTOR3_MANUALPLACEMENT); if (bAdjustAttachmentForPosPressed)
					{
						SpoonerVector3ManualPlacementPtrs = std::make_tuple<GTAentity, Vector3*, Vector3*>(0, &SelectedMarker->m_attachmentArgs.offset, &SelectedMarker->m_attachmentArgs.rotation);
					}
				}
				else
				{
					bool bManualPlacementForPosPressed = false;
					AddOption("Manual Placement", bManualPlacementForPosPressed, nullFunc, SUB::SPOONER_VECTOR3_MANUALPLACEMENT); if (bManualPlacementForPosPressed)
					{
						SpoonerVector3ManualPlacementPtrs = std::make_tuple<GTAentity, Vector3*, Vector3*>(0, &SelectedMarker->m_position, &SelectedMarker->m_rotation);
					}
				}
			}

			AddBreak("---Destination---");
			{
				AddOption("~italic~" + finalDest.ToString(), null);

				bool bSetPosToNull = false;
				AddTickol("No Destination", SelectedMarker->m_destinationVal.m_position.IsZero(), bSetPosToNull, bSetPosToNull); if (bSetPosToNull)
				{
					SelectedMarker->m_destinationVal.m_position.clear();
					SelectedMarker->m_destinationVal.m_attachmentArgs.attachedTo = 0;
					SelectedMarker->m_destinationPtr = nullptr;
				}

				if (!spoocam.IsActive())
				{
					bool bSetPosToMe = false;
					AddOption("Set To Player Position", bSetPosToMe); if (bSetPosToMe)
					{
						Vector3 myPos = GTAentity(PLAYER_PED_ID()).GetPosition();
						SelectedMarker->m_destinationVal.m_position = myPos;
						SelectedMarker->m_destinationVal.m_attachmentArgs.attachedTo = 0;
						SelectedMarker->m_destinationPtr = nullptr;
					}
				}
				else
				{
					bool bSetPosToHitCoords = false;
					AddOption("Set To Camera Target", bSetPosToHitCoords); if (bSetPosToHitCoords)
					{
						Vector3 hitCoords = spoocam.RaycastForCoord(Vector2(0.0f, 0.0f), 0, 160.0f, 3.0f);
						SelectedMarker->m_destinationVal.m_position = hitCoords;
						SelectedMarker->m_destinationVal.m_attachmentArgs.attachedTo = 0;
						SelectedMarker->m_destinationPtr = nullptr;
					}
				}
				if (IS_WAYPOINT_ACTIVE())
				{
					bool bSetPosToWp = false;
					AddOption("Set To Waypoint", bSetPosToWp); if (bSetPosToWp)
					{
						GTAblip wpBlip = GET_FIRST_BLIP_INFO_ID(BlipIcon::Waypoint);
						Vector3 wpCoords = wpBlip.GetPosition();
						wpCoords.z = World::GetGroundHeight(wpCoords);
						SelectedMarker->m_destinationVal.m_position = wpCoords;
						SelectedMarker->m_destinationVal.m_attachmentArgs.attachedTo = 0;
						SelectedMarker->m_destinationPtr = nullptr;
					}
				}

				if (Databases::MarkerDb.size() > 1)
				{
					AddOption("Link With Marker" + (SelectedMarker->m_destinationPtr == nullptr ? "" : " ('" + SelectedMarker->m_destinationPtr->m_name + "')"), null, nullFunc, SUB::SPOONER_MANAGEMARKERS_INMARKER_DEST2MARKER);
				}

				if (SelectedMarker->m_destinationPtr == nullptr)
				{
					if (SelectedMarker->m_destinationVal.m_attachmentArgs.attachedTo.Exists())
					{
						bool bAdjustAttachmentForPosPressed = false;
						AddOption("Adjust Attachment", bAdjustAttachmentForPosPressed, nullFunc, SUB::SPOONER_VECTOR3_MANUALPLACEMENT); if (bAdjustAttachmentForPosPressed)
						{
							SpoonerVector3ManualPlacementPtrs = std::make_tuple<GTAentity, Vector3*, Vector3*>(0, &SelectedMarker->m_destinationVal.m_attachmentArgs.offset, nullptr);
						}
					}
					else
					{
						bool bManualPlacementForPosPressed = false;
						AddOption("Manual Placement", bManualPlacementForPosPressed, nullFunc, SUB::SPOONER_VECTOR3_MANUALPLACEMENT); if (bManualPlacementForPosPressed)
						{
							SpoonerVector3ManualPlacementPtrs = std::make_tuple<GTAentity, Vector3*, Vector3*>(0, &SelectedMarker->m_destinationVal.m_position, nullptr);
						}
					}
				}

				bool bDestHeading_plus = false, bDestHeading_minus = false;
				AddNumber("Post-Teleport Direction (To Face)", SelectedMarker->m_destinationHeading, 1, null, bDestHeading_plus, bDestHeading_minus);
				if (bDestHeading_plus) { SelectedMarker->m_destinationHeading += 1.0f; }
				if (bDestHeading_minus) { SelectedMarker->m_destinationHeading -= 1.0f; }
			}

			AddBreak("---Other---");

			bool bCopyMarkerPressed = false;
			AddOption("Copy Marker", bCopyMarkerPressed); if (bCopyMarkerPressed)
			{
				SelectedMarker = MarkerManagement::CopyMarker(*SelectedMarker); // std::vector array location may change
				SelectedMarker->m_name = Game::InputBox(SelectedMarker->m_name, 26U, "Enter custom marker name:", SelectedMarker->m_name);
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SetArg1String, SelectedMarker->m_name, 26U, "Enter custom marker name:", SelectedMarker->m_name);
				//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&SelectedMarker->m_name);
				Menu::currentop_ar[Menu::currentArrayIndex]++;
			}
		}
		void Sub_ManageMarkers_InMarker_Dest2Marker()
		{
			if (SelectedMarker == nullptr)
			{
				Menu::SetPreviousMenu();
				return;
			}

			AddTitle("Select Marker");

			for (UINT i = 0; i < Databases::MarkerDb.size(); i++)
			{
				auto& m = Databases::MarkerDb[i];
				if (&m != SelectedMarker)
				{
					bool bMarkerPressed = false;
					AddOption(m.m_name, bMarkerPressed); if (bMarkerPressed)
					{
						SelectedMarker->m_destinationPtr = &m;
						SelectedMarker->m_destinationVal = m;
						Menu::SetPreviousMenu();
						return;
					}

					if (*Menu::currentopATM == Menu::printingop)
					{
						m.m_selectedInSub = true;

						Vector3 finalPosition, finalRotation;
						if (SelectedMarker->m_attachmentArgs.attachedTo.Exists())
						{
							finalPosition = SelectedMarker->m_attachmentArgs.attachedTo.GetOffsetInWorldCoords(SelectedMarker->m_attachmentArgs.offset);
							finalRotation = SelectedMarker->m_attachmentArgs.attachedTo.Rotation_get() + SelectedMarker->m_attachmentArgs.rotation;
						}
						else
						{
							finalPosition = SelectedMarker->m_position;
							finalRotation = SelectedMarker->m_rotation;
						}

						SpoonerMarkerPosition* dest = &m;
						Vector3 finalDest;
						float finalDestHeading;
						if (dest->m_attachmentArgs.attachedTo.Exists())
						{
							finalDest = dest->m_attachmentArgs.attachedTo.GetOffsetInWorldCoords(dest->m_attachmentArgs.offset);
							finalDestHeading = dest->m_attachmentArgs.attachedTo.Rotation_get().z + SelectedMarker->m_destinationHeading;
						}
						else
						{
							finalDest = dest->m_position;
							finalDestHeading = SelectedMarker->m_destinationHeading;
						}

						World::DrawLine(finalPosition, finalDest, RGBA(SelectedMarker->m_colour, 200));
						World::DrawLightWithRange(finalDest, RgbS(SelectedMarker->m_colour), 2.3f, 1.5f);
					}
				}
			}

		}
		void Sub_ManageMarkers_InMarker_Attach()
		{
			if (SelectedMarker == nullptr)
			{
				Menu::SetPreviousMenu();
				return;
			}

			AddTitle("Attach To Something");

			bool bToggleKeepPosWhenAttaching = false;
			AddTickol("Keep World Position When Attaching", Settings::bKeepPositionWhenAttaching, bToggleKeepPosWhenAttaching, bToggleKeepPosWhenAttaching, TICKOL::BOXTICK, TICKOL::BOXBLANK); if (bToggleKeepPosWhenAttaching)
				Settings::bKeepPositionWhenAttaching = !Settings::bKeepPositionWhenAttaching;

			bool bDetachPressed = false;
			AddTickol("Detach", !SelectedMarker->m_attachmentArgs.attachedTo.GetHandle(), bDetachPressed, bDetachPressed, TICKOL::TICK2); if (bDetachPressed)
			{
				if (SelectedMarker->m_attachmentArgs.attachedTo.Exists())
				{
					SelectedMarker->m_position = SelectedMarker->m_attachmentArgs.attachedTo.GetOffsetInWorldCoords(SelectedMarker->m_attachmentArgs.offset);
					SelectedMarker->m_rotation = SelectedMarker->m_attachmentArgs.attachedTo.Rotation_get() + SelectedMarker->m_attachmentArgs.rotation;
				}
				SelectedMarker->m_attachmentArgs.attachedTo = 0;
				SelectedMarker->m_attachmentArgs.offset.clear();
				SelectedMarker->m_attachmentArgs.rotation.clear();
			}

			if (!Databases::EntityDb.empty())
			{
				AddBreak("---Database---");
				for (auto& e : Databases::EntityDb)
				{
					if (e.handle.Exists())
					{
						bool bEntityPressed = false;
						AddTickol(e.hashName, SelectedMarker->m_attachmentArgs.attachedTo == e.handle, bEntityPressed, bEntityPressed, TICKOL::TICK2); if (bEntityPressed)
						{
							SelectedMarker->m_attachmentArgs.attachedTo = e.handle;
							if (Settings::bKeepPositionWhenAttaching)
							{
								SelectedMarker->m_attachmentArgs.offset = e.handle.GetOffsetGivenWorldCoords(SelectedMarker->m_position);
								SelectedMarker->m_attachmentArgs.rotation = SelectedMarker->m_rotation - e.handle.Rotation_get();
							}
							else
							{
								SelectedMarker->m_attachmentArgs.offset.clear();
								SelectedMarker->m_attachmentArgs.rotation.clear();
							}
							Menu::SetPreviousMenu();
							return;
						}

						if (*Menu::currentopATM == Menu::printingop) EntityManagement::ShowArrowAboveEntity(e.handle, RGBA(0, 255, 0, 200));
					}
					else
					{
						AddOption(e.hashName + " (Invalid)", null);
					}
				}
			}

		}

		void Sub_SpawnCategories()
		{
			_searchStr.clear();

			AddTitle("Spawn Into World");
			AddOption("Object", null, nullFunc, SUB::SPOONER_SPAWN_PROP);
			AddOption("Ped", null, nullFunc, SUB::SPOONER_SPAWN_PED);
			AddOption("Vehicle", null, nullFunc, SUB::SPOONER_SPAWN_VEHICLE);
		}
		void Sub_SpawnProp()
		{
			AddTitle("Spawn Object");

			AddOption("Favourites", null, nullFunc, SUB::SPOONER_SPAWN_PROP_FAVOURITES);

			bool bSearchPressed = false;
			AddOption(_searchStr.empty() ? "SEARCH" : boost::to_upper_copy(_searchStr), bSearchPressed, nullFunc, -1, true); if (bSearchPressed)
			{
				_searchStr = Game::InputBox(_searchStr, 126U, "SEARCH", _searchStr);
				boost::to_lower(_searchStr);
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SearchToLower, _searchStr, 126U, std::string(), _searchStr);
				//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&_searchStr);
			}

			// Display props models loop
			for (const auto& current : objectModels)
			{
				if (!_searchStr.empty()) { if (current.find(_searchStr) == std::string::npos) continue; }

				Model currentModel = GET_HASH_KEY(current);

				MenuOptions::AddOption_AddProp(current, currentModel.hash);

				if (Menu::printingop == *Menu::currentopATM)
				{
					bool bIsAFav = FavouritesManagement::IsPropAFavourite(current, currentModel.hash);
					if (Menu::bitController)
					{
						Menu::add_IB(INPUT_SCRIPT_RLEFT, (!bIsAFav ? "Add to" : "Remove from") + (std::string)" favourites");

						if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_SCRIPT_RLEFT))
						{
							!bIsAFav ? FavouritesManagement::AddPropToFavourites(current, currentModel.hash) : FavouritesManagement::RemovePropFromFavourites(current, currentModel.hash);
						}
					}
					else
					{
						Menu::add_IB(VirtualKey::B, (!bIsAFav ? "Add to" : "Remove from") + (std::string)" favourites");

						if (IsKeyJustUp(VirtualKey::B))
						{
							!bIsAFav ? FavouritesManagement::AddPropToFavourites(current, currentModel.hash) : FavouritesManagement::RemovePropFromFavourites(current, currentModel.hash);
						}
					}
				}
			}

			bool bInputPressed = false;
			AddOption("INPUT MODEL", bInputPressed); if (bInputPressed)
			{
				EntityManagement::InputEntityIntoDb(EntityType::PROP);
			}

		}
		void Sub_SpawnProp_Favourites()
		{
			AddTitle("Favourites");

			using FavouritesManagement::xmlFavouriteProps;
			pugi::xml_document doc;
			if (doc.load_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouriteProps).c_str()).status != pugi::status_ok)
			{
				doc.reset();
				auto nodeDecleration = doc.append_child(pugi::node_declaration);
				nodeDecleration.append_attribute("version") = "1.0";
				nodeDecleration.append_attribute("encoding") = "ISO-8859-1";
				auto nodeRoot = doc.append_child("FavouriteProps");
				doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouriteProps).c_str());
				return;
			}
			pugi::xml_node nodeRoot = doc.child("FavouriteProps");

			bool bInputAdd = false;
			AddOption("Add New Object Model", bInputAdd); if (bInputAdd)
			{
				std::string inputStr = Game::InputBox("", 40U, "Enter model name:");
				if (inputStr.length() > 0)
				{
					if (FavouritesManagement::AddPropToFavourites(inputStr, GET_HASH_KEY(inputStr)))
					{
						Game::Print::PrintBottomLeft("Model ~b~added~s~.");
					}
					else
						Game::Print::PrintBottomLeft("~r~Error:~s~ Unable to add model.");
				}
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::FavouritePropModelEntryName, std::string(), 40U, "Enter model name:");
			}

			if (nodeRoot.first_child())
			{
				AddBreak("---Added Object Models---");

				for (auto nodeLocToLoad = nodeRoot.first_child(); nodeLocToLoad; nodeLocToLoad = nodeLocToLoad.next_sibling())
				{
					const std::string& modelName = nodeLocToLoad.attribute("modelName").as_string();
					Model model = nodeLocToLoad.attribute("modelHash").as_uint(0);
					if (model.hash == 0)
						model = GET_HASH_KEY(modelName);
					MenuOptions::AddOption_AddProp(modelName, model);

					if (Menu::printingop == *Menu::currentopATM)
					{
						if (Menu::bitController)
						{
							Menu::add_IB(INPUT_SCRIPT_RLEFT, "Remove");

							if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_SCRIPT_RLEFT))
							{
								nodeLocToLoad.parent().remove_child(nodeLocToLoad);
								doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouriteProps).c_str());
								if (*Menu::currentopATM >= Menu::totalop)
									Menu::Up();
								return; // Yeah
							}
						}
						else
						{
							Menu::add_IB(VirtualKey::B, "Remove");

							if (IsKeyJustUp(VirtualKey::B))
							{
								nodeLocToLoad.parent().remove_child(nodeLocToLoad);
								doc.save_file((const char*)(GetPathffA(Pathff::Main, true) + xmlFavouriteProps).c_str());
								if (*Menu::currentopATM >= Menu::totalop)
									Menu::Up();
								return; // Yeah
							}
						}
					}
				}
			}

		}
		void Sub_SpawnPed()
		{
			AddTitle("Spawn Ped");

			AddOption("Favourites", null, nullFunc, SUB::MODELCHANGER_FAVOURITES);

			bool bSearchPressed = false;
			AddOption(_searchStr.empty() ? "SEARCH" : boost::to_upper_copy(_searchStr), bSearchPressed, nullFunc, -1, true); if (bSearchPressed)
			{
				_searchStr = Game::InputBox(_searchStr, 126U, "SEARCH", _searchStr);
				boost::to_lower(_searchStr);
				//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::SearchToLower, _searchStr, 126U, std::string(), _searchStr);
				//OnscreenKeyboard::State::arg1._ptr = reinterpret_cast<void*>(&_searchStr);
			}

			if (!_searchStr.empty())
			{
				for (auto& current : g_pedModels)
				{
					if (current.first.find(_searchStr) == std::string::npos && current.second.find(_searchStr) == std::string::npos)
						continue;

					Model currentModel = GET_HASH_KEY(current.first);

					if (currentModel.IsInCdImage())
					{
						MenuOptions::AddOptionAddPed(current.second, currentModel);
						if (*Menu::currentopATM == Menu::printingop) PedFavourites::ShowInstructionalButton(currentModel);
					}
				}
			}
			else
			{
				AddOption("Player", null, nullFunc, SUB::MODELCHANGER_PLAYER);
				AddOption("Animals", null, nullFunc, SUB::MODELCHANGER_ANIMAL);
				AddOption("Ambient Females", null, nullFunc, SUB::MODELCHANGER_AMBFEMALES);
				AddOption("Ambient Males", null, nullFunc, SUB::MODELCHANGER_AMBMALES);
				AddOption("Cutscene Models", null, nullFunc, SUB::MODELCHANGER_CS);
				AddOption("Gang Females", null, nullFunc, SUB::MODELCHANGER_GANGFEMALES);
				AddOption("Gang Males", null, nullFunc, SUB::MODELCHANGER_GANGMALES);
				AddOption("Story Models", null, nullFunc, SUB::MODELCHANGER_STORY);
				AddOption("Multiplayer Models", null, nullFunc, SUB::MODELCHANGER_MP);
				AddOption("Scenario Females", null, nullFunc, SUB::MODELCHANGER_SCENARIOFEMALES);
				AddOption("Scenario Males", null, nullFunc, SUB::MODELCHANGER_SCENARIOMALES);
				AddOption("Story Scenario Females", null, nullFunc, SUB::MODELCHANGER_ST_SCENARIOFEMALES);
				AddOption("Story Scenario Males", null, nullFunc, SUB::MODELCHANGER_ST_SCENARIOMALES);
				AddOption("Others", null, nullFunc, SUB::MODELCHANGER_OTHERS);
			}

			bool bInputPressed = false;
			AddOption("INPUT MODEL", bInputPressed); if (bInputPressed)
			{
				EntityManagement::InputEntityIntoDb(EntityType::PED);
			}
		}
		void Sub_SpawnVehicle()
		{
			using VehicleSpawner::AddVehicleCategoryOption;
			typedef VehicleSpawner::Indices Indices;

			g_Ped1 = PLAYER_PED_ID();

			AddTitle("Spawn Vehicle");
			AddOption("Favourites", null, nullFunc, SUB::SPAWNVEHICLE_FAVOURITES);

			AddBreak("---Cars---");
			AddVehicleCategoryOption("Open Wheel", Indices::OPENWHEEL);
			AddVehicleCategoryOption("Super", Indices::SUPER);
			AddVehicleCategoryOption("Sports", Indices::SPORT);
			AddVehicleCategoryOption("Sports Classics", Indices::SPORTSCLASSIC);
			AddVehicleCategoryOption("Coupes", Indices::COUPE);
			AddVehicleCategoryOption("Muscle", Indices::MUSCLE);
			AddVehicleCategoryOption("Offroad", Indices::OFFROAD);
			AddVehicleCategoryOption("SUVs", Indices::SUV);
			AddVehicleCategoryOption("Sedans", Indices::SEDAN);
			AddVehicleCategoryOption("Compacts", Indices::COMPACT);
			AddVehicleCategoryOption("Drift", Indices::DRIFT);

			AddBreak("---Industrial---");
			AddVehicleCategoryOption("Vans", Indices::VAN);
			AddVehicleCategoryOption("Services", Indices::SERVICE);
			AddVehicleCategoryOption("Industrial", Indices::INDUSTRIAL);
			AddVehicleCategoryOption("Commercial", Indices::COMMERCIAL);
			AddVehicleCategoryOption("Utility", Indices::UTILITY);
			AddVehicleCategoryOption("Trains", Indices::TRAIN);

			AddBreak("---Others---");
			AddVehicleCategoryOption("Emergency", Indices::EMERGENCY);
			AddVehicleCategoryOption("Military", Indices::MILITARY);
			AddVehicleCategoryOption("Motorcycles", Indices::MOTORCYCLE);
			AddVehicleCategoryOption("Bicycles", Indices::BICYCLE);
			AddVehicleCategoryOption("Planes", Indices::PLANE);
			AddVehicleCategoryOption("Helicopters", Indices::HELICOPTER);
			AddVehicleCategoryOption("Boats", Indices::BOAT);
			AddVehicleCategoryOption("Others", Indices::OTHER);

			bool bInputPressed = false;
			AddOption("INPUT MODEL", bInputPressed); if (bInputPressed)
			{
				EntityManagement::InputEntityIntoDb(EntityType::VEHICLE);
			}
		}
	}


	void EntityAlphaLevelSub_()
	{
		GTAentity entity = g_Ped4;
		auto alphaLevel = entity.GetAlpha();

		AddTitle("Opacity");

		bool bReset = false;
		AddOption("Reset", bReset); if (bReset)
		{
			entity.RequestControl();
			entity.ResetAlpha();
		}

		std::array<BYTE, 5> alphaLevels = { 0, 50, 89, 160, 255 };

		for (auto& lvl : alphaLevels)
		{
			bool bAlphaLevelPressed = false;
			AddTickol(std::to_string((int)(((float)(lvl) / 255.0f) * 100)) + "%", alphaLevel == lvl, bAlphaLevelPressed, null); if (bAlphaLevelPressed)
			{
				entity.RequestControl();
				entity.SetAlpha(lvl);
			}
		}

		AddBreak("---Manual---");
		bool bInputPressed = false, bPlusPressed = false, bMinusPressed = false;
		AddNumber("Level", alphaLevel, 0, bInputPressed, bPlusPressed, bMinusPressed);
		if (bPlusPressed) { if (alphaLevel < 255) { alphaLevel++; entity.RequestControl(); entity.SetAlpha(alphaLevel); } }
		if (bMinusPressed) { if (alphaLevel > 0) { alphaLevel--; entity.RequestControl(); entity.SetAlpha(alphaLevel); } }
		if (bInputPressed)
		{
			std::string inputStr = Game::InputBox(std::string(), 4U, "Enter an alpha level (0-255):", std::to_string(alphaLevel));
			if (inputStr.length() > 0)
			{
				try
				{
					alphaLevel = stoi(inputStr);
					if (alphaLevel < 0 || alphaLevel > 255)
						throw;
					entity.RequestControl();
					entity.SetAlpha(alphaLevel);
				}
				catch (...)
				{
					Game::Print::PrintErrorInvalidInput(inputStr);
				}
			}
			//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::AlphaLevelMenuInput, std::string(), 3U, "Enter an alpha level (0-255):", std::to_string(alphaLevel));
			//OnscreenKeyboard::State::arg1._int = entity.GetHandle();
			//OnscreenKeyboard::State::arg2._int = alphaLevel;
		}

	}

	void PedExplosionSub()
	{
		GTAped thisPed = g_Ped1;

		if (!thisPed.Exists())
		{
			Menu::SetPreviousMenu();
			return;
		}

		AddTitle("Explosions");

		static std::array<std::pair<EXPLOSION::EXPLOSION, std::string>, 16> vExplosionNames
		{ {
			{ EXPLOSION::MOLOTOV, "Molotov" },
			{ EXPLOSION::TANKSHELL, "Tankshell" },
			{ EXPLOSION::CAR, "Car" },
			{ EXPLOSION::PLANE, "Plane" },
			{ EXPLOSION::PETROL_PUMP, "Petrol Pump" },
			{ EXPLOSION::DIR_STEAM, "Steam" },
			{ EXPLOSION::DIR_FLAME, "Flame" },
			{ EXPLOSION::DIR_WATER_HYDRANT, "Water Hydrant" },
			{ EXPLOSION::SHIP_DESTROY, "Ship" },
			{ EXPLOSION::FLARE, "Flare" },
			{ EXPLOSION::BULLET, "Bullet" },
			{ EXPLOSION::TRAIN, "Train" },
			{ EXPLOSION::BLIMP, "Blimp" },
			{ EXPLOSION::DIR_FLAME_EXPLODE, "Stronger Flame" },
			{ EXPLOSION::FIREWORK, "Firework" },
			{ EXPLOSION::VALKYRIE_CANNON, "Valkyrie Cannon" },
			} };

		for (auto& i : vExplosionNames)
		{
			bool bPressed = false;
			AddOption(i.second, bPressed); if (bPressed)
			{
				World::AddExplosion(thisPed.GetPosition(), i.first, 10.0f, 1.5f, true, true);
			}
		}

		bool bInvisible_pressed = false;
		AddOption("Invisible", bInvisible_pressed); if (bInvisible_pressed)
		{
			World::AddExplosion(thisPed.GetPosition(), EXPLOSION::BLIMP, 10.0f, 0.1f, false, false);
		}

		bool bTaze_pressed = false;
		AddOption("Taze Ped", bTaze_pressed); if (bTaze_pressed)
		{
			auto& ped = thisPed;
			Vector3 startPos = ped.GetOffsetInWorldCoords(0.5f, 2.8f, 1.0f);
			Vector3 targPos = ped.GetPosition(); //GET_PED_BONE_COORDS(ped.Handle(), Bone::Chest, 0, 0, 0);
			 //ped.RequestControl();
			CLEAR_AREA_OF_PROJECTILES(startPos.x, startPos.y, startPos.z, 4.0f, 0);
			World::ShootBullet(startPos, targPos, 0, WEAPON_STUNGUN, 5, 2000.0f, false, true);
		}

		bool bApplyDamage_Pressed = false;
		AddOption("Hurt Ped (Do Damage)", bApplyDamage_Pressed); if (bApplyDamage_Pressed)
		{
			thisPed.ApplyDamage(600);
		}
	}

	void AttachFunnyObjectSub()
	{
		bool Network_ObjectSub_Coffin = 0,
			Network_ObjectSub_Gnome = 0,
			Network_ObjectSub_PeeHose = 0,
			Network_ObjectSub_Marker1 = 0,
			Network_ObjectSub_Cone = 0,
			Network_ObjectSub_Egg = 0,
			Network_ObjectSub_Weed = 0,
			Network_ObjectSub_OrangeBall = 0,
			Network_ObjectSub_InflatedLenny = 0,
			Network_ObjectSub_fatTV = 0,
			Network_ObjectSub_TV = 0,
			Network_ObjectSub_Headset = 0,
			Network_ObjectSub_Snowman = 0,
			Network_ObjectSub_Dildo = 0,
			Network_ObjectSub_BigClock = 0,
			Network_ObjectSub_Glove = 0,
			Network_ObjectSub_Katana = 0,
			Network_ObjectSub_BigShit = 0,
			Network_ObjectSub_YetiMask = 0,
			Network_ObjectSub_SkiMask = 0,
			Network_ObjectSub_CowboyHat = 0,
			Network_ObjectSub_HardHat = 0,
			Network_ObjectSub_BeachFire = 0,
			Network_ObjectSub_Custom = 0,
			Network_ObjectSub_Clear = 0,
			Network_ObjectSub_Mani = 0,
			Network_ObjectSub_Chop = 0,
			Network_ObjectSub_Alien = 0,
			Network_ObjectSub_Cow = 0,
			Network_ObjectSub_CustomPed = 0,
			Network_ObjectSub_ClearPeds = 0,
			Network_ObjectSub_ClearVehicles = 0;


		AddTitle("Attach Objects");
		AddOption("Coffin", Network_ObjectSub_Coffin);
		AddOption("Garden Gnome", Network_ObjectSub_Gnome); // NEEDS TO BE DONE WITH BONE ID 31086
		AddOption("Pee-Hose", Network_ObjectSub_PeeHose);
		AddOption("Placement Marker", Network_ObjectSub_Marker1);
		AddOption("Traffic Cone", Network_ObjectSub_Cone);
		AddOption("Alien Egg", Network_ObjectSub_Egg);
		AddOption("W33d", Network_ObjectSub_Weed);
		AddOption("Orange Ball", Network_ObjectSub_OrangeBall);
		AddOption("Inflated Lenny", Network_ObjectSub_InflatedLenny);
		AddOption("Fat-screen TV", Network_ObjectSub_fatTV);
		AddOption("Flatscreen TV", Network_ObjectSub_TV);
		AddOption("Headset", Network_ObjectSub_Headset);
		AddOption("Snowman", Network_ObjectSub_Snowman);
		AddOption("Dildo", Network_ObjectSub_Dildo);
		AddOption("Big Clock", Network_ObjectSub_BigClock);
		AddOption("Boxing Gloves", Network_ObjectSub_Glove);
		AddOption("Katanas", Network_ObjectSub_Katana);
		AddOption("Poo", Network_ObjectSub_BigShit);
		AddOption("Yeti Mask", Network_ObjectSub_YetiMask);
		AddOption("Ski Mask", Network_ObjectSub_SkiMask);
		AddOption("Cowboy Hat", Network_ObjectSub_CowboyHat);
		AddOption("Hard Hat", Network_ObjectSub_HardHat);
		AddOption("Mad Fire", Network_ObjectSub_BeachFire);
		AddOption("Mani", Network_ObjectSub_Mani);
		AddOption("Chop", Network_ObjectSub_Chop);
		AddOption("Alien", Network_ObjectSub_Alien);
		AddOption("Cow", Network_ObjectSub_Cow);
		AddOption("Input Object Model", Network_ObjectSub_Custom);
		AddOption("Input Ped Model", Network_ObjectSub_CustomPed);
		AddOption("Clear Objects", Network_ObjectSub_Clear);
		AddOption("Clear Peds", Network_ObjectSub_ClearPeds);
		AddOption("Clear Vehicles", Network_ObjectSub_ClearVehicles);


		if (Network_ObjectSub_Coffin) {
			Hash tempHash = 460248592; // prop_coffin_02
			int tempBone = 1;
			float att_x = 0.0f;
			float att_y = 0.0f;
			float att_z = 0.01f;
			float att_rx = 89.0f;
			float att_ry = 181.0f;
			float att_rz = 0.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_Gnome) {
			Hash tempHash = 4283117964; // gnome
			int tempBone = 31086;
			float att_x = -0.17f;
			float att_y = 0.14f;
			float att_z = -0.01f;
			float att_rx = 14.21f;
			float att_ry = -92.0f;
			float att_rz = 176.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_PeeHose) {
			Hash tempHash = 1864388154; // sprinkler
			int tempBone = 1;
			float att_x = 0.0f;
			float att_y = 0.0f;
			float att_z = -0.243f;
			float att_rx = -0.046f;
			float att_ry = 0.0f;
			float att_rz = 283.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_Marker1) {
			Hash tempHash = 3712170306; // prop_mp_placement_sm
			int tempBone = 0;
			float att_x = 0.0f;
			float att_y = 0.0f;
			float att_z = -0.54f;
			float att_rx = 0.0f;
			float att_ry = 0.0f;
			float att_rz = 0.0f;
			Model(tempHash).Load(2000);
			//tempBone = GET_PED_BONE_INDEX(Static_241, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_Cone) {
			Hash tempHash = 3760607069;
			int tempBone = 31086;
			float att_x = 0.1f;
			float att_y = 0.02f;
			float att_z = 0.0f;
			float att_rx = 0.0f;
			float att_ry = 90.0f;
			float att_rz = 0.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_Egg) {
			Hash tempHash = 1803116220;
			int tempBone = 31086;
			float att_x = 0.1f;
			float att_y = 0.02f;
			float att_z = 0.0f;
			float att_rx = 0.0f;
			float att_ry = 90.0f;
			float att_rz = 0.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_Weed) {
			Hash tempHash = 452618762;
			int tempBone = 31086;
			float att_x = 0.1f;
			float att_y = 0.02f;
			float att_z = 0.0f;
			float att_rx = 0.0f;
			float att_ry = 90.0f;
			float att_rz = 0.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_OrangeBall) {
			Hash tempHash = 148511758; // prop_juicestand
			int tempBone = 31086;
			float att_x = 0.0f;
			float att_y = 0.0f;
			float att_z = 0.0f;
			float att_rx = 0.0f;
			float att_ry = 0.0f;
			float att_rz = 0.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_InflatedLenny) {
			Hash tempHash = 2402097066;
			int tempBone = 31086;
			float att_x = -0.01f;
			float att_y = -0.03f;
			float att_z = -0.02f;
			float att_rx = 181.6272f;
			float att_ry = 92.7292f;
			float att_rz = -0.39f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_fatTV) {
			Hash tempHash = 480355301;
			int tempBone = 31086;
			float att_x = 1.0f;
			float att_y = 0.03f;
			float att_z = -0.08f;
			float att_rx = 181.0999f;
			float att_ry = 86.0f;
			float att_rz = 2.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_TV) {
			Hash tempHash = 1036195894;
			int tempBone = 31086;
			float att_x = 0.22f;
			float att_y = 0.0f;
			float att_z = 0.0f;
			float att_rx = 180.0f;
			float att_ry = 90.0f;
			float att_rz = -11.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_Headset) {
			Hash tempHash = 3885918439;
			int tempBone = 31086;
			float att_x = 0.07f;
			float att_y = -0.01f;
			float att_z = 0.0f;
			float att_rx = -175.0f;
			float att_ry = 85.0f;
			float att_rz = -11.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_Snowman) {
			Hash tempHash = 2677555217;
			int tempBone = -1;
			float att_x = 0.0f;
			float att_y = 0.0f;
			float att_z = 0.0f;
			float att_rx = 0.0f;
			float att_ry = 0.0f;
			float att_rz = -32.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_Dildo) {
			Hash tempHash = 3872089630;
			int tempBone = 31086;
			float att_x = 0.22f;
			float att_y = 0.0f;
			float att_z = 0.0f;
			float att_rx = 180.0f;
			float att_ry = 90.0f;
			float att_rz = -11.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_BigClock) {
			Hash tempHash = 3948540099;
			int tempBone = 31086;
			float att_x = 0.1f;
			float att_y = 0.02f;
			float att_z = 0.0f;
			float att_rx = 0.0f;
			float att_ry = 90.0f;
			float att_rz = 0.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_Glove) {
			Hash tempHash = 335898267; // prop_boxing_glove_01
			int tempBone = Bone::RightHand;
			float att_x = -0.06f;
			float att_y = 0.0f;
			float att_z = -0.03f;
			float att_rx = 90.0f;
			float att_ry = 90.0f;
			float att_rz = 0.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			tempBone = Bone::LeftHand;
			att_x = -0.05f;
			att_y = 0.0f;
			att_z = 0.03f;
			att_rx = -90.0f;
			att_ry = 90.0f;
			att_rz = 0.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			SET_MODEL_AS_NO_LONGER_NEEDED(tempHash);
			return;
		}

		if (Network_ObjectSub_Katana) {
			Hash tempHash = 3803840879;
			int tempBone = 28422;
			float att_x = 0.056f;
			float att_y = 0.05f;
			float att_z = -0.01f;
			float att_rx = -77.0f;
			float att_ry = 10.6f;
			float att_rz = 0.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			tempHash = 3803840879;
			tempBone = 60309;
			att_x = 0.0500f;
			att_y = 0.0400f;
			att_z = 0.0000f;
			att_rx = -122.5596f;
			att_ry = 0.0000f;
			att_rz = 0.0000f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_BigShit) {
			for (int i = 0; i < 100; i++)
			{
				Hash tempHash = 2223607550;
				int tempBone = 31086;
				float att_x = 0.1f;
				float att_y = 0.03f;
				float att_z = 0.0f;
				float att_rx = 0.0f;
				float att_ry = 90.0f;
				float att_rz = 0.0f;
				Model(tempHash).Load(2000);
				tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
				int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
				ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
				SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			}
			return;
		}

		if (Network_ObjectSub_YetiMask) {
			Hash tempHash = 3379896055;
			int tempBone = 31086;
			float att_x = 0.007f;
			float att_y = -0.019f;
			float att_z = 0.001f;
			float att_rx = 168.999f;
			float att_ry = 93.0001f;
			float att_rz = -3.0011f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_SkiMask) {
			Hash tempHash = 3083173879;
			int tempBone = 31086;
			float att_x = 0.02f;
			float att_y = 0.022f;
			float att_z = 0.002f;
			float att_rx = 178.13f;
			float att_ry = 88.4692f;
			float att_rz = 6.7001f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_CowboyHat) {
			Hash tempHash = 2365581599;
			int tempBone = 31086;
			float att_x = 0.12f;
			float att_y = 0.0f;
			float att_z = 0.0f;
			float att_rx = -173.47f;
			float att_ry = 92.0799f;
			float att_rz = -10.92f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_HardHat) {
			Hash tempHash = 3757476377;
			int tempBone = 31086;
			float att_x = 0.1f;
			float att_y = 0.0f;
			float att_z = 0.0f;
			float att_rx = 180.0f;
			float att_ry = 85.0f;
			float att_rz = -11.0f;
			Model(tempHash).Load(2000);
			tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
			int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
			SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			return;
		}

		if (Network_ObjectSub_BeachFire) {
			for (int i = 0; i < 3; i++)
			{
				Hash tempHash = 3229200997;
				int tempBone = -1;
				float att_x = 0.0f;
				float att_y = 0.0f;
				float att_z = 0.0f;
				float att_rx = 0.0f;
				float att_ry = 0.0f;
				float att_rz = 0.0f;
				Model(tempHash).Load(2000);
				tempBone = GET_PED_BONE_INDEX(g_Ped1, tempBone);
				int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
				ATTACH_ENTITY_TO_ENTITY(tempObject, g_Ped1, tempBone, att_x, att_y, att_z - 0.26f, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
				SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
			}
			return;
		}

		if (Network_ObjectSub_Custom) {
			std::string inputStr = Game::InputBox("", 64U, "Enter prop name:");
			if (inputStr.length() > 0)
			{
				Entity tempEntity = g_Ped1;
				Hash tempHash = GET_HASH_KEY(inputStr);
				if (!IS_MODEL_IN_CDIMAGE(tempHash)) Game::Print::PrintErrorInvalidModel(inputStr);
				else
				{
					if (IS_PED_IN_ANY_VEHICLE(g_Ped1, 0)) tempEntity = GET_VEHICLE_PED_IS_IN(g_Ped1, 0);

					float att_x = 0.0f;
					float att_y = 0.0f;
					float att_z = 0.0f;
					float att_rx = 0.0f;
					float att_ry = 0.0f;
					float att_rz = 0.0f;
					if (Model(tempHash).Load(2000))
					{
						int tempObject = CREATE_OBJECT(tempHash, 0.0f, 0.0f, 0.0f, 1, 1, 0);
						ATTACH_ENTITY_TO_ENTITY(tempObject, tempEntity, 0, att_x, att_y, att_z, att_rx, att_ry, att_rz, 1, 1, 0, 0, 2, 1, 0);
						SET_OBJECT_AS_NO_LONGER_NEEDED(&tempObject);
					}
				}
			}
			//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::NetworkAttachObjectsCustomProp, "", 64U, "Enter prop name:");
			return;
		}

		if (Network_ObjectSub_Clear) {
			Vector3 temp = GET_ENTITY_COORDS(g_Ped1, 1);
			ClearAttachmentsOffEntity(g_Ped1, EntityType::PROP);
			CLEAR_AREA_OF_OBJECTS(temp.x, temp.y, temp.z, 2.5f, 0);
			return;
		}

		if (Network_ObjectSub_Mani) {
			AttachPedToVehicle(PedHash::Mani, g_Ped1, Vector3(), Vector3(), false, true);
			return;
		}

		if (Network_ObjectSub_Chop) {
			AttachPedToVehicle(PedHash::Chop, g_Ped1, Vector3(), Vector3(), false, true);
			return;
		}

		if (Network_ObjectSub_Alien) {
			AttachPedToVehicle(PedHash::MovAlien01, g_Ped1, Vector3(), Vector3(), false, true);
			return;
		}

		if (Network_ObjectSub_Cow) {
			AttachPedToVehicle(PedHash::Cow, g_Ped1, Vector3(), Vector3(), false, false);
			return;
		}

		if (Network_ObjectSub_CustomPed) {
			std::string inputStr = Game::InputBox("", 64U, "Enter ped name:");
			if (inputStr.length() > 0)
			{
				Hash tempHash = GET_HASH_KEY(inputStr);
				if (!IS_MODEL_IN_CDIMAGE(tempHash)) Game::Print::PrintErrorInvalidModel(inputStr);
				else
				{
					Entity tempEntity = g_Ped1;
					if (IS_PED_IN_ANY_VEHICLE(g_Ped1, 0)) tempEntity = GET_VEHICLE_PED_IS_IN(g_Ped1, 0);
					AttachPedToVehicle(tempHash, tempEntity, Vector3(), Vector3(), false, true);
				}
			}
			//OnscreenKeyboard::State::Set(OnscreenKeyboard::Purpose::NetworkAttachObjectsCustomPed, "", 64U, "Enter ped name:");
			return;
		}

		if (Network_ObjectSub_ClearPeds) {
			Vector3 temp = GET_ENTITY_COORDS(g_Ped1, 1);
			ClearAttachmentsOffEntity(g_Ped1, EntityType::PED);
			CLEAR_AREA_OF_PEDS(temp.x, temp.y, temp.z, 2.5f, 0);
			return;
		}

		if (Network_ObjectSub_ClearVehicles) {
			Vector3 temp = GET_ENTITY_COORDS(g_Ped1, 1);
			ClearAttachmentsOffEntity(g_Ped1, EntityType::VEHICLE);
			CLEAR_AREA_OF_PEDS(temp.x, temp.y, temp.z, 2.5f, 0);
			return;
		}



	}

}


#include "..\..\Menu\submenu_switch.h"
#include "..\..\Menu\submenu_enum.h"
REGISTER_SUBMENU(PEDEXPLOSIONSUB,      									sub::PedExplosionSub)
REGISTER_SUBMENU(ATTACHFUNNYOBJECTSUB, 									sub::AttachFunnyObjectSub)
REGISTER_SUBMENU(ENTITYALPHALEVEL,     									sub::EntityAlphaLevelSub_)
REGISTER_SUBMENU(SPOONER_MAIN,                                        	sub::Spooner::Submenus::Sub_SpoonerMain)
REGISTER_SUBMENU(SPOONER_SPAWN_CATEGORIES,                            	sub::Spooner::Submenus::Sub_SpawnCategories)
REGISTER_SUBMENU(SPOONER_SPAWN_PROP,                                  	sub::Spooner::Submenus::Sub_SpawnProp)
REGISTER_SUBMENU(SPOONER_SPAWN_PROP_FAVOURITES,                       	sub::Spooner::Submenus::Sub_SpawnProp_Favourites)
REGISTER_SUBMENU(SPOONER_SPAWN_PED,                                   	sub::Spooner::Submenus::Sub_SpawnPed)
REGISTER_SUBMENU(SPOONER_SPAWN_VEHICLE,                               	sub::Spooner::Submenus::Sub_SpawnVehicle)
REGISTER_SUBMENU(SPOONER_MANAGEMARKERS,                               	sub::Spooner::Submenus::Sub_ManageMarkers)
REGISTER_SUBMENU(SPOONER_MANAGEMARKERS_REMOVAL,                       	sub::Spooner::Submenus::Sub_ManageMarkers_Removal)
REGISTER_SUBMENU(SPOONER_MANAGEMARKERS_INMARKER,                      	sub::Spooner::Submenus::Sub_ManageMarkers_InMarker)
REGISTER_SUBMENU(SPOONER_MANAGEMARKERS_INMARKER_DEST2MARKER,          	sub::Spooner::Submenus::Sub_ManageMarkers_InMarker_Dest2Marker)
REGISTER_SUBMENU(SPOONER_MANAGEMARKERS_INMARKER_ATTACH,               	sub::Spooner::Submenus::Sub_ManageMarkers_InMarker_Attach)
REGISTER_SUBMENU(SPOONER_MANAGEDB,                                    	sub::Spooner::Submenus::Sub_ManageEntities)
REGISTER_SUBMENU(SPOONER_MANAGEDB_REMOVAL,                            	sub::Spooner::Submenus::Sub_ManageEntities_Removal)
REGISTER_SUBMENU(SPOONER_SAVEFILES,                                   	sub::Spooner::Submenus::Sub_SaveFiles)
REGISTER_SUBMENU(SPOONER_SAVEFILES_LOAD,                              	sub::Spooner::Submenus::Sub_SaveFiles_Load)
REGISTER_SUBMENU(SPOONER_SAVEFILES_LOAD_LEGACYSP00N,                  	sub::Spooner::Submenus::Sub_SaveFiles_Load_LegacySP00N)
REGISTER_SUBMENU(SPOONER_VECTOR3_MANUALPLACEMENT,                     	sub::Spooner::Submenus::Sub_Vector3_ManualPlacement)
REGISTER_SUBMENU(SPOONER_QUICKMANUALPLACEMENT,                        	sub::Spooner::Submenus::Sub_QuickManualPlacement)
REGISTER_SUBMENU(SPOONER_GROUPSPOON,                                  	sub::Spooner::Submenus::Sub_GroupSpoon)
REGISTER_SUBMENU(SPOONER_GROUPSPOON_SELECTENTITIES,                   	sub::Spooner::Submenus::Sub_GroupSpoon_SelectEntities)
REGISTER_SUBMENU(SPOONER_GROUPSPOON_ATTACHTO,                         	sub::Spooner::Submenus::Sub_GroupSpoon_AttachTo)
REGISTER_SUBMENU(SPOONER_SETTINGS,                                    	sub::Spooner::Submenus::Sub_Settings)
REGISTER_SUBMENU(SPOONER_SELECTEDENTITYOPS,                           	sub::Spooner::Submenus::Sub_SelectedEntityOps)
REGISTER_SUBMENU(SPOONER_PEDOPS,                                      	sub::Spooner::Submenus::Sub_PedOps)
REGISTER_SUBMENU(SPOONER_PEDOPS_WEAPON,                               	sub::Spooner::Submenus::Sub_PedOps_Weapon)
REGISTER_SUBMENU(SPOONER_PEDOPS_WEAPON_INCATEGORY,                   	sub::Spooner::Submenus::Sub_PedOps_Weapon_InCategory)
REGISTER_SUBMENU(SPOONER_ATTACHMENTOPS,                               	sub::Spooner::Submenus::Sub_AttachmentOps)
REGISTER_SUBMENU(SPOONER_ATTACHMENTOPS_ATTACHTO,                      	sub::Spooner::Submenus::Sub_AttachmentOps_AttachTo)
REGISTER_SUBMENU(SPOONER_ATTACHMENTOPS_SELECTBONE,                    	sub::Spooner::Submenus::Sub_AttachmentOps_SelectBone)
REGISTER_SUBMENU(SPOONER_MANUALPLACEMENT,                             	sub::Spooner::Submenus::Sub_ManualPlacement)
REGISTER_SUBMENU(SPOONER_SIZEMANIPULATION,                            	sub::Spooner::Submenus::Sub_SizeManipulation)