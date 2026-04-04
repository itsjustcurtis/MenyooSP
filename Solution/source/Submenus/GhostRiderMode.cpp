/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "GhostRiderMode.h"

namespace sub
{
	namespace GhostRiderMode
	{
		class GhostRiderMode final : public GenericLoopedMode
		{
		private:
			std::string freakOutDict;
			std::string freakOutName;
			GTAped playerPed;
			GTAvehicle playerVehicle;
			PTFX::sFxData fxDataPurpleSmoke;
			PTFX::sFxData fxDataFireTrail;
			PTFX::NonLoopedPTFX fxHead2;
			PTFX::NonLoopedPTFX fxHandLeft;
			PTFX::NonLoopedPTFX fxHandRight;
			PTFX::LoopedPTFX fxCarWheelFrontLeft;
			PTFX::LoopedPTFX fxCarWheelFrontRight;
			PTFX::LoopedPTFX fxCarWheelRearLeft;
			PTFX::LoopedPTFX fxCarWheelRearRight;
		public:
			float tyreScale;
			float headScale;
			float handScale;
			Vector3 tyreRot;
			Vector3 headRot;
			Vector3 handRot;

			GhostRiderMode();

			void TurnOn() override;
			void TurnOff() override;

			void Tick() override;
			inline void DoGhostRiderModeTick();
		};

		GhostRiderMode::GhostRiderMode()
			: freakOutDict("ANIM@MP_PLAYER_INTUPPERFREAKOUT"),
			freakOutName("EXIT_FP"),
			fxDataPurpleSmoke("scr_rcbarry2", "scr_clown_appears"),
			fxDataFireTrail("scr_martin1", "scr_sol1_fire_trail"),
			fxHead2(fxDataPurpleSmoke),
			fxHandLeft(fxDataPurpleSmoke),
			fxHandRight(fxDataPurpleSmoke),
			fxCarWheelFrontLeft(fxDataFireTrail),
			fxCarWheelFrontRight(fxDataFireTrail),
			fxCarWheelRearLeft(fxDataFireTrail),
			fxCarWheelRearRight(fxDataFireTrail),

			tyreScale(-2.00f),
			headScale(0.075f),
			handScale(0.045f),
			tyreRot(-91.06, 0, -90.0f),
			headRot(-90.0f, 0, -90.0f),
			handRot(-90.0f, 0, -90.0f)
		{
		}

		void GhostRiderMode::TurnOn()
		{
			GenericLoopedMode::TurnOn();
			Game::Print::PrintBottomLeft("~b~Ghost Rider Mode~s~: I was short on actual fire so purple's the next best thing.");

			playerPed = PLAYER_PED_ID();
			playerPed.Task().PlayAnimation(freakOutDict, freakOutName, 4.0f, -4.0f, 2000, 1, 0.2f, false);
		}
		void GhostRiderMode::TurnOff()
		{
			GenericLoopedMode::TurnOff();

			if (playerPed.Exists())
			{
				playerPed.SetFireProof(false);
			}
			if (playerVehicle.Exists())
			{
				playerVehicle.SetFireProof(false);
			}

			fxCarWheelFrontLeft.Remove();
			fxCarWheelFrontRight.Remove();
			fxCarWheelRearLeft.Remove();
			fxCarWheelRearRight.Remove();
		}

		void GhostRiderMode::Tick()
		{
			if (bEnabled)
			{
				DoGhostRiderModeTick();
			}
		}
		inline void GhostRiderMode::DoGhostRiderModeTick()
		{
			playerPed = PLAYER_PED_ID();

			if (playerPed.Exists())
			{
				playerPed.SetFireProof(true);

				fxHead2.EasyStart(playerPed, headScale, Vector3(), headRot, RGBA(RGBA::AllWhite(), 186), Bone::IK_Head);
				fxHandLeft.EasyStart(playerPed, handScale, Vector3(), handRot, RGBA(RGBA::AllWhite(), 186), Bone::IK_L_Hand);
				fxHandRight.EasyStart(playerPed, handScale, Vector3(), handRot, RGBA(RGBA::AllWhite(), 186), Bone::IK_R_Hand);

				if (playerPed.IsInVehicle())
				{
					playerVehicle = playerPed.CurrentVehicle();
					const auto& playerVehicleModel = playerVehicle.Model();
					playerVehicle.SetFireProof(true);

					if (playerVehicleModel.IsBicycle() || playerVehicleModel.IsBike())
					{
						fxCarWheelFrontLeft.EasyStart(playerVehicle, tyreScale, Vector3(), tyreRot, RGBA::AllWhite(), playerVehicle.GetBoneIndex(VBone::wheel_lf));
						fxCarWheelFrontRight.EasyStart(playerVehicle, tyreScale, Vector3(), tyreRot, RGBA::AllWhite(), playerVehicle.GetBoneIndex(VBone::wheel_lr));
					}
					else // isCar
					{
						fxCarWheelFrontLeft.EasyStart(playerVehicle, tyreScale, Vector3(), tyreRot, RGBA::AllWhite(), playerVehicle.GetBoneIndex(VBone::wheel_lf));
						fxCarWheelFrontRight.EasyStart(playerVehicle, tyreScale, Vector3(), tyreRot, RGBA::AllWhite(), playerVehicle.GetBoneIndex(VBone::wheel_rf));
						fxCarWheelRearLeft.EasyStart(playerVehicle, tyreScale, Vector3(), tyreRot, RGBA::AllWhite(), playerVehicle.GetBoneIndex(VBone::wheel_lr));
						fxCarWheelRearRight.EasyStart(playerVehicle, tyreScale, Vector3(), tyreRot, RGBA::AllWhite(), playerVehicle.GetBoneIndex(VBone::wheel_rr));
					}
				}
				else
				{
					fxCarWheelFrontLeft.Remove();
					fxCarWheelFrontRight.Remove();
					fxCarWheelRearLeft.Remove();
					fxCarWheelRearRight.Remove();
				}

			}
		}

		GhostRiderMode g_ghostRiderMode;

		void ToggleOnOff()
		{
			g_ghostRiderMode.Toggle();
		}
		void Tick()
		{
			g_ghostRiderMode.Tick();
		}


		std::string outfitFileName = ("GhostRider");
		void ApplyGhostRiderOutfit()
		{
			sub::ComponentChangerOutfit::Apply(PLAYER_PED_ID(), GetPathffA(Pathff::Outfit, true) + outfitFileName, true, true, true, true, true, true);
		}
		void SpawnGhostRiderRide()
		{
			sub::VehicleSaver::VehicleReadFromFile(GetPathffA(Pathff::Vehicle, true) + outfitFileName, PLAYER_PED_ID());
		}


		void Sub_GhostRiderMode()
		{
			AddTitle("Ghost Rider Mode");
			AddLocal("Toggle", g_ghostRiderMode.Enabled(), ToggleOnOff, ToggleOnOff);
			AddOption("Apply Outfit (With Flames)", null, ApplyGhostRiderOutfit);
			AddOption("Spawn Ride (With Flames)", null, SpawnGhostRiderRide);
		}
	}
}


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(GHOSTRIDERMODE,          sub::GhostRiderMode::Sub_GhostRiderMode)