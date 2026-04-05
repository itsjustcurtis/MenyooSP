/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "BreatheStuff.h"

namespace sub
{
	namespace BreatheStuff
	{
		const std::vector<std::pair<std::string, BreathePtfxType>> captionsBreatheStuff
		{
			{ "None", BreathePtfxType::None },
			{ "Bloody Puke", BreathePtfxType::Blood },
			{ "Fire", BreathePtfxType::Fire }
		};

		PTFX::LoopedPTFX g_breatheStuffPTFX;
		BreathePtfxType playerBreatheStuff = BreathePtfxType::None;

		void SetSelfBreathePTFX(const BreathePtfxType& type)
		{
			PTFX::LoopedPTFX& ptfx = g_breatheStuffPTFX;

			if (Menu::bitController ? !IS_CONTROL_PRESSED(2, INPUT_FRONTEND_LS) : !IsKeyDown(VirtualKey::J))
			{
				if (ptfx.Exists())
				{
					ptfx.Remove();
				}
				return;
			}

			GTAped playerPed = PLAYER_PED_ID();
			float scale, X = 0.0f, Y = 0.0f, Z = 0.0f, aX = 0.0f, aY = 0.0f, aZ = 0.0f;

			switch (type)
			{
			case BreathePtfxType::Blood:
				ptfx.SetFxData({ "scr_solomon3", "scr_trev4_747_blood_impact" });
				scale = 0.16f;
				aX = -90.0f;
				aZ = -110.0f;
				break;
			case BreathePtfxType::Fire:default:
				ptfx.SetFxData({ "core", "ent_sht_flame" });
				scale = 1.0f;
				break;

			}

			if (!ptfx.IsAssetLoaded())
			{
				ptfx.LoadAsset();
			}

			if (playerPed.IsHuman())
			{
				X += -0.02;
				Y += 0.2;
				Z += 0.0;
				aX += 90.0;
				aY += 100.0;
				aZ += 90.0;
			}
			else
			{
				switch (playerPed.Model().hash)
				{
				default:
					X += 0.26;
					Y += 0.0;
					Z += -0.06;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Chop:
					X += 0.26;
					Y += 0.0;
					Z += -0.06;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Cow:
					X += 0.3;
					Y += 0.0;
					Z += -0.05;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Boar:
					X += 0.3;
					Y += 0.0;
					Z += -0.05;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Pig:
					X += 0.3;
					Y += 0.0;
					Z += -0.05;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Chimp:
					X += 0.0;
					Y += 0.0;
					Z += 0.0;
					aX += 90.0;
					aY += 90.0;
					aZ += 90.0;
					break;
				case PedHash::Rhesus:
					X += 0.0;
					Y += 0.0;
					Z += 0.0;
					aX += 90.0;
					aY += 90.0;
					aZ += 90.0;
					break;
				case PedHash::Husky:
					X += 0.26;
					Y += 0.0;
					Z += -0.06;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Milton:
					X += 0.26;
					Y += 0.0;
					Z += -0.06;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Retriever:
					X += 0.26;
					Y += 0.0;
					Z += -0.06;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Shepherd:
					X += 0.26;
					Y += 0.0;
					Z += -0.06;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Coyote:
					X += 0.2;
					Y += 0.0;
					Z += 0.0;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Cat:
					X += 0.1;
					Y += -0.06;
					Z += 0.0;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Deer:
					X += 0.2;
					Y += 0.0;
					Z += 0.0;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Hen:
					X += 0.13;
					Y += 0.0;
					Z += -0.06;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Rat:
					X += 0.13;
					Y += 0.0;
					Z += -0.06;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::ChickenHawk:
					X += 0.13;
					Y += 0.0;
					Z += -0.06;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Pigeon:
					X += 0.13;
					Y += 0.0;
					Z += 0.00;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Crow:
					X += 0.13;
					Y += 0.0;
					Z += -0.03;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Seagull:
					X += 0.13;
					Y += 0.0;
					Z += -0.03;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Cormorant:
					X += 0.13;
					Y += 0.0;
					Z += -0.03;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Dolphin:
					X += 0.26;
					Y += 0.0;
					Z += -0.06;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::HammerShark:
					X += 0.26;
					Y += 0.0;
					Z += -0.06;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Fish:
					X += 0.26;
					Y += 0.0;
					Z += -0.06;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::TigerShark:
					X += 0.4;
					Y += 0.0;
					Z += -0.2;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::KillerWhale:
					X += 0.4;
					Y += 0.0;
					Z += -0.2;
					aX += 0.0;
					aY += 90.0;
					aZ += 0.0;
					break;
				case PedHash::Humpback:
					X += 1.1;
					Y += -1.0;
					Z += 0.0;
					aX += 0.0;
					aY += 0.0;
					aZ += 0.0;
					break;
				}
			}

			if (!ptfx.Exists())
			{
				ptfx.Start(playerPed, scale, Vector3(X, Y, Z), Vector3(aX, aY, aZ), RGBA::AllWhite(), Bone::Head);
			}

			if (GET_GAME_TIMER() >= Menu::delayedTimer)
			{
				ptfx.SetColour(RgbS::Random());
			}
		}


		void AddOption(const std::string& text, const BreathePtfxType& type)
		{
			null = 0;
			AddTickol(text, type == playerBreatheStuff, null, null); if (null)
			{
				if (playerBreatheStuff == BreathePtfxType::None && type != BreathePtfxType::None)
				{
					Game::Print::PrintBottomLeft(oss_ << "Hold " << "~b~" << (Menu::bitController ? "LS" : "J") << "~s~" << " to breathe out stuff!");
				}

				if (g_breatheStuffPTFX.Exists())
				{
					g_breatheStuffPTFX.Remove();
				}

				playerBreatheStuff = type;
			}
		}

		void BreatheStuffMenu()
		{
			AddTitle("Breathe StufF");
			for (auto& bsfxn : captionsBreatheStuff)
			{
				AddOption(bsfxn.first, bsfxn.second);
			}
		}
	}
}

#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(BREATHESTUFF,            sub::BreatheStuff::BreatheStuffMenu)