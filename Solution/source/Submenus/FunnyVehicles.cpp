/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "FunnyVehicles.h"

namespace sub
{
	void AttachPedToVehicle(GTAmodel::Model model, GTAvehicle vehicle, const Vector3& offset, const Vector3& rotation, bool invis, bool piggyback)
	{
		if (vehicle.Exists())
		{
			if (model.Load(2000))
			{
				if (invis && vehicle.IsVisible())
				{
					vehicle.SetVisible(false);
					for (int i = -1; i <= (vehicle.MaxPassengers() - 2); i++)
					{
						if (vehicle.IsSeatFree(VehicleSeat(i)))
						{
							continue;
						}
						GTAentity sped = vehicle.GetPedOnSeat(VehicleSeat(i));
						sped.RequestControl();
						sped.SetVisible(true);
					}
				}

				Ped ped = CREATE_PED(PedType::Human, model.hash, 0.0f, 0.0f, 0.0f, vehicle.GetHeading(), 1, 1);
				PED_TO_NET(ped);
				SET_ENTITY_LOD_DIST(ped, 696969);
				ATTACH_ENTITY_TO_ENTITY(ped, vehicle.GetHandle(), -1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 180.0f, 0, 0, 0, 0, 2, 1, 0);
				DETACH_ENTITY(ped, 1, 1);
				ATTACH_ENTITY_TO_ENTITY(ped, vehicle.GetHandle(), -1, offset.x, offset.y, offset.z, rotation.x, rotation.y, rotation.z, 0, 0, 0, 0, 2, 1, 0);
				SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, 1);
				if (piggyback)
				{
					ATTACH_ENTITY_TO_ENTITY(ped, vehicle.GetHandle(), -1, 0.0f, -0.3f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0, 2, 1, 0);
					REQUEST_ANIM_DICT("mini@prostitutes@sexnorm_veh");
					WAIT(50);
					TASK_PLAY_ANIM(ped, "mini@prostitutes@sexnorm_veh", "bj_loop_male", 8.f, 0.f, -1, 9, 0, 0, 0, 0);
				}
				else
				{
					TASK_STAND_STILL(ped, -1);
				}
				SetPedInvincibleOn(ped);
				SET_PED_AS_NO_LONGER_NEEDED(&ped);
			}
		}
	}

	void AttachObjectToVehicle(GTAmodel::Model model, GTAvehicle vehicle, float X, float Y, float Z, float Pitch, float Roll, float Yaw, bool invis, int boneIndex, bool dynamic, bool collisionEnabled, bool destroyVar)
	{
		if (vehicle.Exists())
		{
			model.Load(3000);

			GTAprop object = CREATE_OBJECT(model.hash, 0.0f, 0.0f, 0.0f, 1, 1, dynamic);
			object.SetLODDistance(1000000);

			GTAped sped;
			if (invis)
			{
				if (model.IsBicycle() || model.IsBike())
				{
					vehicle.SetAlpha(0);
				}
				else if (vehicle.IsVisible())
				{
					vehicle.SetVisible(false);
					for (int i = -1; i <= ((int)(GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(vehicle.Handle())) - 2); i++)
					{
						if (IS_VEHICLE_SEAT_FREE(vehicle.Handle(), i, 0))
						{
							continue;
						}
						sped.Handle() = GET_PED_IN_VEHICLE_SEAT(vehicle.Handle(), i, 0);
						sped.RequestControl();
						sped.SetVisible(true);
					}
				}
			}

			object.AttachTo(vehicle, boneIndex, false, Vector3(X, Y, Z), Vector3(Pitch, Roll, Yaw));
			SET_ENTITY_LIGHTS(object.Handle(), 0);
			if (collisionEnabled) object.SetIsCollisionEnabled(collisionEnabled);
			if (destroyVar) SET_OBJECT_AS_NO_LONGER_NEEDED(&object.Handle());
		}

	}

	void AttachVehicleToVehicle(GTAmodel::Model model, GTAvehicle vehicle, int primColour, int secColour, float X, float Y, float Z, float Pitch, float Roll, float Yaw, bool invis, int boneIndex, bool collisionEnabled)
	{
		if (vehicle.Exists())
		{
			model.Load(3000);

			GTAvehicle veh = CREATE_VEHICLE(model.hash, 0.0f, 0.0f, 0.0f, 0.0f, 1, 1, 0);
			veh.SetLODDistance(1000000);

			GTAped sped;
			if (invis)
			{
				if (model.IsBicycle() || model.IsBike())
				{
					vehicle.SetAlpha(0);
				}
				else if (vehicle.IsVisible())
				{
					vehicle.SetVisible(false);
					for (int i = -1; i <= ((int)(GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(vehicle.Handle())) - 2); i++)
					{
						if (IS_VEHICLE_SEAT_FREE(vehicle.Handle(), i, 0)) 
						{
							continue;
						}
						sped.Handle() = GET_PED_IN_VEHICLE_SEAT(vehicle.Handle(), i, 0);
						sped.RequestControl();
						sped.SetVisible(true);
					}
				}
			}

			veh.AttachTo(vehicle, boneIndex, false, Vector3(X, Y, Z), Vector3(Pitch, Roll, Yaw));
			SET_ENTITY_LIGHTS(veh.Handle(), 0);
			veh.SetPrimaryColour(primColour);
			veh.SetSecondaryColour(secColour);
			if (collisionEnabled) 
			{
				veh.SetIsCollisionEnabled(collisionEnabled);
			}
			SET_VEHICLE_AS_NO_LONGER_NEEDED(&veh.Handle());
		}

	}

	namespace FunnyVehicles
	{
		void HaxBy(const std::string& name)
		{
			Game::Print::PrintBottomLeft("Hax by ~b~" + name);
		}

		int PlaceFunnyVehicle(Hash hash)
		{
			return SpawnVehicle(hash, g_Ped1);
		}

		void GoKart()
		{
			Vehicle tempVehicle = PlaceFunnyVehicle(VEHICLE_ADDER); // Adder
			GTAvehicle(tempVehicle).BreakAllDoors(true);

			// prop_yoga_mat_02
			Hash tempHash = 2057317573;
			FLOAT X = -0.2f; // 23
			FLOAT Y = 0.3f; // 465
			FLOAT Z = -0.45f; // 466
			FLOAT Pitch = 0.0f; // 467
			FLOAT Roll = 0.0f; // 476
			FLOAT Yaw = 90.0f; // 481
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 2057317573;
			X = -0.6000;
			Y = 0.3000;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 2057317573;
			X = -0.6000;
			Y = 0.2000;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 2057317573;
			X = -0.2000;
			Y = 0.2000;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_bumper_02
			tempHash = 2574278700;
			X = 0.1800;
			Y = 0.2500;
			Z = -0.4100;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 2574278700;
			X = -0.9800;
			Y = 0.2500;
			Z = -0.4100;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = -90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_weight_15k
			tempHash = 933757793;
			X = 0.2400;
			Y = -0.4400;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = 0.2700;
			Y = -0.4400;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = 0.3000;
			Y = -0.4400;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = 0.3000;
			Y = -0.4400;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = 0.3300;
			Y = -0.4400;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = 0.3600;
			Y = -0.4400;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = 0.2400;
			Y = 0.9300;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = 0.2700;
			Y = 0.9300;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = 0.3000;
			Y = 0.9300;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = 0.3300;
			Y = 0.9300;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = 0.3600;
			Y = 0.9300;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = -1.0200;
			Y = 0.9300;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = -1.0500;
			Y = 0.9300;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = -1.0800;
			Y = 0.9300;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = -1.1100;
			Y = 0.9300;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = -1.1400;
			Y = 0.9300;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = -1.0200;
			Y = -0.4400;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = -1.0500;
			Y = -0.4400;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = -1.0800;
			Y = -0.4400;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = -1.1100;
			Y = -0.4400;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 933757793;
			X = -1.1400;
			Y = -0.4400;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_weight_15k
			tempHash = 933757793;
			X = -0.4000;
			Y = 0.3800;
			Z = 0.2000;
			Pitch = -30.000;
			Roll = 0.0000;
			Yaw = 0.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_cs_bar
			tempHash = 3062227748;
			X = -0.4000;
			Y = 0.5900;
			Z = -0.1400;
			Pitch = 30.000;
			Roll = 0.0000;
			Yaw = 0.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_car_seat
			tempHash = 1382419899;
			X = -0.4000;
			Y = -0.1500;
			Z = -0.5000;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 180.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_power_cell
			tempHash = 2235081574;
			X = -0.1500;
			Y = 1.0700;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 2235081574;
			X = -0.6500;
			Y = 1.0700;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 2235081574;
			X = -0.4000;
			Y = 1.0700;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 2235081574;
			X = -0.1500;
			Y = -0.5700;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 2235081574;
			X = -0.6500;
			Y = -0.5700;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 2235081574;
			X = -0.4000;
			Y = -0.5700;
			Z = -0.4500;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			HaxBy("Stiff_"); // Credits

		}

		void DragsterBike()
		{
			Vehicle tempVehicle = PlaceFunnyVehicle(VEHICLE_AKUMA); // Akuma
			Hash tempHash = 4137416026;
			FLOAT X = -0.07f; // 23
			FLOAT Y = 1.1f; // 465
			FLOAT Z = 0.03f; // 466
			FLOAT Pitch = 0.0f; // 467
			FLOAT Roll = 90.0f; // 476
			FLOAT Yaw = 0.0f; // 481
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_cs_bar
			tempHash = 3062227748;
			X = -0.1500;
			Y = 0.7800;
			Z = 0.2100;
			Pitch = 60.0000;
			Roll = 0.0000;
			Yaw = 0.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 3062227748;
			X = 0.1500;
			Y = 0.7800;
			Z = 0.2100;
			Pitch = 60.0000;
			Roll = 0.0000;
			Yaw = 0.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 3062227748;
			X = 0.1500;
			Y = 0.2800;
			Z = 0.0800;
			Pitch = -30.0000;
			Roll = 0.0000;
			Yaw = 0.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 3062227748;
			X = -0.1500;
			Y = 0.2800;
			Z = 0.0800;
			Pitch = -30.0000;
			Roll = 0.0000;
			Yaw = 0.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 3062227748;
			X = -0.1500;
			Y = -0.2900;
			Z = -0.2400;
			Pitch = -93.0000;
			Roll = 0.0000;
			Yaw = 0.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 3062227748;
			X = 0.1500;
			Y = -0.2900;
			Z = -0.2400;
			Pitch = -93.0000;
			Roll = 0.0000;
			Yaw = 0.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 3062227748;
			X = 0.1500;
			Y = -0.3400;
			Z = -0.0200;
			Pitch = -59.0000;
			Roll = 0.0000;
			Yaw = 0.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 3062227748;
			X = -0.1500;
			Y = -0.3400;
			Z = -0.0200;
			Pitch = -59.0000;
			Roll = 0.0000;
			Yaw = 0.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 3062227748;
			X = -0.1500;
			Y = 0.1400;
			Z = 0.2600;
			Pitch = -59.0000;
			Roll = 0.0000;
			Yaw = 0.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			tempHash = 3062227748;
			X = 0.1500;
			Y = 0.1400;
			Z = 0.2600;
			Pitch = -59.0000;
			Roll = 0.0000;
			Yaw = 0.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_cs_bar
			tempHash = 3062227748;
			X = 0.0000;
			Y = 0.4600;
			Z = 0.4000;
			Pitch = 0.0000;
			Roll = 90.0000;
			Yaw = 0.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_wheel_06
			tempHash = 3730985457;
			X = 0.0000;
			Y = -0.6600;
			Z = -0.2100;
			Pitch = 0.0000;
			Roll = 0.0000;
			Yaw = 90.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_power_cell
			tempHash = 2235081574;
			X = 0.0000;
			Y = -0.3900;
			Z = 0.1600;
			Pitch = -40.0000;
			Roll = 0.0000;
			Yaw = 0.0000;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			SET_ENTITY_VISIBLE(tempVehicle, true, false);
			SET_ENTITY_ALPHA(tempVehicle, 0, 0);

			HaxBy("Stiff_"); // Credits

		}
		void WeedWheelsBike()
		{
			GTAvehicle tempVehicle = PlaceFunnyVehicle(VEHICLE_BATI); // Bati
			auto wheel_rf = tempVehicle.GetBoneIndex(VBone::wheel_rf); // Still on bike
			auto wheel_lf = tempVehicle.GetBoneIndex(VBone::wheel_lf); // The rotating bone
			auto handlebar = tempVehicle.GetBoneIndex(VBone::handlebars);

			// prop_rub_wheel_02
			Hash tempHash = 3437004565;
			FLOAT X = 0.0f;
			FLOAT Y = 0.0f;
			FLOAT Z = 0.0f;
			FLOAT Pitch = 0.0f;
			FLOAT Roll = 0.0f;
			FLOAT Yaw = 0.0f;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1, wheel_rf);
			tempHash = 3437004565;
			X = 0.0f;
			Y = 0.0f;
			Z = 0.0f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 0.0f;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1, wheel_lf);

			// prop_byard_pipe_01
			tempHash = 2971578861;
			X = 0.0f;
			Y = 0.0f;
			Z = 0.0f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 0.0f;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1, wheel_lf);
			tempHash = 2971578861;
			X = 0.0f;
			Y = 0.0f;
			Z = 0.0f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 0.0f;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1, wheel_lf);

			// prop_rub_wheel_01
			tempHash = 103020963;
			X = 2.1f;
			Y = 0.0f;
			Z = 0.0f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 0.0f;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1, wheel_lf);
			tempHash = 103020963;
			X = -2.0f;
			Y = 0.0f;
			Z = 0.0f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 0.0f;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1, wheel_lf);

			// prop_minigun_01
			tempHash = 3365286072;
			X = 1.0f;
			Y = 0.0f;
			Z = 0.5f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 90.0f;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1, -1);
			tempHash = 3365286072;
			X = -1.0f;
			Y = 0.0f;
			Z = 0.5f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 90.0f;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1, -1);

			// prop_weed_01
			tempHash = 452618762;
			X = 2.1f;
			Y = 0.0f;
			Z = -1.0f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 0.0f;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1, wheel_lf);
			tempHash = 452618762;
			X = -2.0f;
			Y = 0.0f;
			Z = -1.0f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 0.0f;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1, wheel_lf);

			// prop_car_seat
			tempHash = 1382419899;
			X = 0.0f;
			Y = -0.4f;
			Z = 0.4f;
			Pitch = 22.0f;
			Roll = 0.0f;
			Yaw = 180.0f;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1, -1);

			// prop_cs_dildo_01
			tempHash = 3872089630;
			X = 0.29f;
			Y = -0.09f;
			Z = -0.36f;
			Pitch = 33.31f;
			Roll = 279.39f;
			Yaw = -34.43f;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1, handlebar);
			tempHash = 3872089630;
			X = 0.3f;
			Y = -0.08f;
			Z = -0.37f;
			Pitch = 111.699f;
			Roll = 0.0f;
			Yaw = -8.3f;
			AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1, handlebar);

			tempVehicle.SetVisible(true);
			tempVehicle.SetAlpha(0);

			HaxBy("Wahkah Enyeto");
		}

		void YachtAirship()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_BLIMP); // Blimp
			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);
			Hash hash;

			hash = 0x4FCAD2E0; // apa_mp_apa_yacht
			AttachObjectToVehicle(hash, vehicle, 0, 0, 0, 0, 0, 90.0f, true, bone_bodyshell, false, true);

			hash = 0xBCDAC9E7; // apa_mp_apa_yacht_win
			AttachObjectToVehicle(hash, vehicle, 0, 0, 0, 0, 0, 90.0f, true, bone_bodyshell, false, true);

			hash = 0xB4AA481D; // apa_mp_apa_yacht_option2
			AttachObjectToVehicle(hash, vehicle, 0, 0, 14.57f, 0, 0, 90.0f, true, bone_bodyshell, false, true);

			hash = 0x75F724B2; // apa_mp_apa_yacht_o2_rail_a
			AttachObjectToVehicle(hash, vehicle, 0, 0, 14.57f, 0, 0, 90.0f, true, bone_bodyshell, false, true);

			hash = 0x98B5E3D4; // apa_mp_apa_yacht_jacuzzi_ripple1
			AttachObjectToVehicle(hash, vehicle, 2.00f, -51.00f, 0, 0, 0, 90.0f, true, bone_bodyshell, false, true);

			hash = 0x949F49C7; // apa_mp_apa_y1_l1a
			AttachObjectToVehicle(hash, vehicle, 0, 0, 14.57f, 0, 0, 90.0f, true, bone_bodyshell, false, true);

			hash = 0x2E51B0EA; // apa_mp_apa_y3_l2b
			AttachObjectToVehicle(hash, vehicle, 0, 0, 14.57f, 0, 0, 90.0f, true, bone_bodyshell, false, true);

			hash = 0x956974FD; // apa_mp_apa_yacht_door2
			AttachObjectToVehicle(hash, vehicle, 1.29f, -36.85f, 0.65f, 0, 0, 90.0f, true, bone_bodyshell, false, true);
			AttachObjectToVehicle(hash, vehicle, 3.40f, 0, 6.70, 0, 0, -180.0f, true, bone_bodyshell, false, true);
			AttachObjectToVehicle(hash, vehicle, 0.60f, 0, 6.70f, 0, 0, 0, true, bone_bodyshell, false, true);

			HaxBy("abstractmode");
		}

		void YachtAirshipWithFans()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_BLIMP); // Blimp

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			Hash hash;

			hash = 0x4FCAD2E0; // apa_mp_apa_yacht
			AttachObjectToVehicle(hash, vehicle, 0, 0, 0, 0, 0, 90.0f, true, bone_bodyshell, false, true);

			hash = 0xBCDAC9E7; // apa_mp_apa_yacht_win
			AttachObjectToVehicle(hash, vehicle, 0, 0, 0, 0, 0, 90.0f, true, bone_bodyshell, false, true);

			hash = 0xB4AA481D; // apa_mp_apa_yacht_option2
			AttachObjectToVehicle(hash, vehicle, 0, 0, 14.57f, 0, 0, 90.0f, true, bone_bodyshell, false, true);

			hash = 0x75F724B2; // apa_mp_apa_yacht_o2_rail_a
			AttachObjectToVehicle(hash, vehicle, 0, 0, 14.57f, 0, 0, 90.0f, true, bone_bodyshell, false, true);

			hash = 0x98B5E3D4; // apa_mp_apa_yacht_jacuzzi_ripple1
			AttachObjectToVehicle(hash, vehicle, 2.00f, -51.00f, 0, 0, 0, 90.0f, true, bone_bodyshell, false, true);

			hash = 0x949F49C7; // apa_mp_apa_y1_l1a
			AttachObjectToVehicle(hash, vehicle, 0, 0, 14.57f, 0, 0, 90.0f, true, bone_bodyshell, false, true);

			hash = 0x2E51B0EA; // apa_mp_apa_y3_l2b
			AttachObjectToVehicle(hash, vehicle, 0, 0, 14.57f, 0, 0, 90.0f, true, bone_bodyshell, false, true);

			hash = 0x956974FD; // apa_mp_apa_yacht_door2
			AttachObjectToVehicle(hash, vehicle, 1.29f, -36.85f, 0.65f, 0, 0, 90.0f, true, bone_bodyshell, false, true);
			AttachObjectToVehicle(hash, vehicle, 3.40f, 0, 6.70, 0, 0, -180.0f, true, bone_bodyshell, false, true);
			AttachObjectToVehicle(hash, vehicle, 0.60f, 0, 6.70f, 0, 0, 0, true, bone_bodyshell, false, true);

			hash = 0x745F3383; // prop_windmill_01
			AttachObjectToVehicle(hash, vehicle, 4.30f, 36.10f, 0.50f, -90.0, 0, 90.0f, true, bone_bodyshell, false, true);
			AttachObjectToVehicle(hash, vehicle, 0.70f, 36.10f, 0.50f, -90.0, 0, -90.0f, true, bone_bodyshell, false, true);
			AttachObjectToVehicle(hash, vehicle, 4.30f, -8.40f, -4.30f, -90.0, 0, 90.0f, true, bone_bodyshell, false, true);
			AttachObjectToVehicle(hash, vehicle, 0.70f, -8.40f, -4.30f, -90.0, 0, -90.0f, true, bone_bodyshell, false, true);
			AttachObjectToVehicle(hash, vehicle, 5.00f, -47.0f, -4.0f, 0, -110.0f, 0, true, bone_bodyshell, false, true);
			AttachObjectToVehicle(hash, vehicle, -2.10f, -47.0f, -4.0f, 0, 110.0f, 0, true, bone_bodyshell, false, true);

			HaxBy("abstractmode");
		}

		void FibBuilding()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_BLIMP); // Blimp

			auto bone_chassis = vehicle.GetBoneIndex(VBone::chassis_dummy);

			Model building = 0xAC4365DD; // dt1_05_build1_damage
			const ModelDimensions& buildingDim = building.Dimensions();
			const ModelDimensions& vehicleDim = vehicle.ModelDimensions();

			AttachObjectToVehicle(building, vehicle, 0, 0, vehicleDim.Dim1.z - buildingDim.Dim2.z, 0, 0, 0, 1, bone_chassis, false, true, false); // Don't set as no longer needed

			HaxBy("scorz (originally)");
		}

		void BlackNoisyUFO()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_VALKYRIE); // Valkyrie
			auto bone_chassis = vehicle.GetBoneIndex(VBone::chassis_dummy);
			Hash hash;

			hash = 0x7D79DAD4; // dt1_tc_dufo_core
			AttachObjectToVehicle(hash, vehicle, 0, 0, 0, 0, 0, 0, 1, bone_chassis, true);

			HaxBy("scorz");
		}

		void ToyCar()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_T20); // T20

			vehicle.BreakAllDoors(true);

			// ind_prop_dlc_roller_car
			Hash hash = 0x5C05F6C1;
			FLOAT X = 0.0f;
			FLOAT Y = 0.2f;
			FLOAT Z = -0.5f;
			FLOAT Pitch = 0.0f;
			FLOAT Roll = 0.0f;
			FLOAT Yaw = 180.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_hd_seats_01
			hash = 0x0A5654F6;
			X = 0.0f;
			Y = 0.28f;
			Z = -0.13f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 0.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_wheel_03
			hash = 0xD2FB3B23;
			X = 0.95f;
			Y = 1.2f;
			Z = -0.2f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = -90.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			hash = 0xD2FB3B23;
			X = 1.06f;
			Y = 1.2f;
			Z = -0.2f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = -90.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			hash = 0xD2FB3B23;
			X = 0.98f;
			Y = -0.91f;
			Z = -0.21f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = -90.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			hash = 0xD2FB3B23;
			X = 1.15f;
			Y = -0.91f;
			Z = -0.21f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = -90.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			hash = 0xD2FB3B23;
			X = -0.98f;
			Y = 1.2f;
			Z = -0.21f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = -90.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			hash = 0xD2FB3B23;
			X = -1.15f;
			Y = 1.2f;
			Z = -0.21f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = -90.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			hash = 0xD2FB3B23;
			X = -0.98f;
			Y = -0.91f;
			Z = -0.21f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = -90.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			hash = 0xD2FB3B23;
			X = -1.10f;
			Y = -0.91f;
			Z = -0.21f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = -90.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_bumper_03
			hash = 0xB9579FFA;
			X = 0.0f;
			Y = 1.33f;
			Z = -1.8f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 180.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_compressor_03
			hash = 0xE08EF8F2;
			X = 0.03f;
			Y = 1.34f;
			Z = -0.82f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = -90.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// hei_prop_wall_alarm_off
			hash = 0x889E3E33;
			X = -0.55f;
			Y = -1.38f;
			Z = -0.38f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 180.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			hash = 0x889E3E33;
			X = 0.55f;
			Y = -1.38f;
			Z = -0.38f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 180.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_car_exhaust_01
			hash = 0xFC612F85;
			X = 0.26f;
			Y = -0.76f;
			Z = -0.49f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 0.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			hash = 0xFC612F85;
			X = 0.18f;
			Y = -0.76f;
			Z = -0.49f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 0.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			hash = 0xFC612F85;
			X = -0.40f;
			Y = -0.76f;
			Z = -0.49f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 0.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			hash = 0xFC612F85;
			X = -0.32f;
			Y = -0.76f;
			Z = -0.49f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 0.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_sh_mr_rasp_01
			hash = 0xD59D6B1A;
			X = -0.01f;
			Y = -0.82f;
			Z = 0.11f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 180.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_voltmeter_01
			hash = 0x8F4674EC;
			X = 0.0f;
			Y = 0.98f;
			Z = 0.02f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 0.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			// prop_spot_01
			hash = 0x5376930C;
			X = -0.57f;
			Y = 1.49f;
			Z = -0.04f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 180.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
			hash = 0x5376930C;
			X = 0.55f;
			Y = 1.49f;
			Z = -0.04f;
			Pitch = 0.0f;
			Roll = 0.0f;
			Yaw = 180.0f;
			AttachObjectToVehicle(hash, vehicle, X, Y, Z, Pitch, Roll, Yaw, 1);

			HaxBy("djbob77");
		}

		void Adderuma()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_ADDER); // Adder

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_KURUMA2, vehicle, VehicleColoursMatte::COLOR_MATTE_BLACK, VehicleColoursMatte::COLOR_MATTE_BLACK, 0, 0, 0, 0, 0, 0, false, bone_bodyshell);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_BLACK);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_BLACK);

			HaxBy("KeyWest2014");
		}

		void Zentornuma()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_ZENTORNO); // Zentorno

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_KURUMA2, vehicle, VehicleColoursMatte::COLOR_MATTE_ICE_WHITE, VehicleColoursMatte::COLOR_MATTE_BLACK, 0, 0.2230f, -0.0280f, 0, 0, 0, false, bone_bodyshell);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_ICE_WHITE);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_ICE_WHITE);

			HaxBy("KeyWest2014");
		}

		void TurismoRuma()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_TURISMOR); // TurismoR

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_KURUMA2, vehicle, VehicleColoursMatte::COLOR_MATTE_ICE_WHITE, VehicleColoursMatte::COLOR_MATTE_BLACK, 0, 0.0400f, 0.4000f, 0, 0, 0, false, bone_bodyshell);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_ICE_WHITE);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_ICE_WHITE);

			HaxBy("KeyWest2014");
		}

		void EnturumaXF()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_ENTITYXF); // EntityXF

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_KURUMA2, vehicle, VehicleColoursMatte::COLOR_MATTE_BLACK, VehicleColoursMatte::COLOR_MATTE_BLACK, 0, 0.1410f, 0.1700f, 0, 0, 0, false, bone_bodyshell);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_BLACK);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_BLACK);

			HaxBy("KeyWest2014");
		}

		void Osirisuma()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_OSIRIS); // Osiris

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_KURUMA2, vehicle, VehicleColoursMatte::COLOR_MATTE_YELLOW, VehicleColoursMatte::COLOR_MATTE_YELLOW, 0, 0.1300f, 0.0400f, 0, 0, 0, false, bone_bodyshell);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_YELLOW);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_RED);

			SetVehicleMaxUpgrades(vehicle.Handle(), true);

			HaxBy("KeyWest2014");
		}

		void T20uma()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_T20); // T20

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_KURUMA2, vehicle, VehicleColoursMatte::COLOR_MATTE_YELLOW, VehicleColoursMatte::COLOR_MATTE_BLACK, 0, 0.1400f, -0.0800f, 0, 0, 0, false, bone_bodyshell);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_YELLOW);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_BLACK);

			HaxBy("KeyWest2014");
		}

		void Feltzeruma()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_FELTZER2); // Feltzer2

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_KURUMA2, vehicle, VehicleColoursMatte::COLOR_MATTE_LIME_GREEN, VehicleColoursMatte::COLOR_MATTE_SCHAFTER_PURPLE, 0, -0.1530f, 0.0770f, 0, 0, 0, false, bone_bodyshell);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_LIME_GREEN);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_SCHAFTER_PURPLE);

			HaxBy("KeyWest2014");
		}

		void Banshuma()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_BANSHEE); // Banshee

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_KURUMA2, vehicle, VehicleColoursMatte::COLOR_MATTE_RED, VehicleColoursMatte::COLOR_MATTE_BLACK, 0, -0.0500f, 0.0500f, 4.500f, 0, 0, false, bone_bodyshell);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_RED);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_BLACK);

			HaxBy("KeyWest2014");
		}

		void Nightshuma()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_NIGHTSHADE); // Nightshade

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_KURUMA2, vehicle, VehicleColoursMatte::COLOR_MATTE_YELLOW, VehicleColoursMatte::COLOR_MATTE_BLACK, 0, 0.0100f, 0.3100f, 5.1600f, 0, 0, false, bone_bodyshell);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_YELLOW);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_BLACK);

			SetVehicleMaxUpgrades(vehicle.Handle(), true);

			HaxBy("KeyWest2014");
		}

		void Bulletuma()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_BULLET); // Bullet

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_KURUMA2, vehicle, VehicleColoursClassic::COLOR_CLASSIC_GALAXY_BLUE, VehicleColoursMatte::COLOR_MATTE_BLACK, 0, -0.1900f, 0.2600f, 1.5200f, 0, 0, false, bone_bodyshell);

			vehicle.SetPrimaryColour(VehicleColoursClassic::COLOR_CLASSIC_GALAXY_BLUE);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_BLACK);

			SetVehicleMaxUpgrades(vehicle.Handle(), true);

			HaxBy("KeyWest2014");
		}

		void LandJetski()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_BLAZER); // Blzer

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_SEASHARK2, vehicle, VehicleColoursClassic::COLOR_CLASSIC_PURE_WHITE, VehicleColoursClassic::COLOR_CLASSIC_CANDY_RED, 0, -0.1800f, -0.4600f, 0, 0, 0, false, bone_bodyshell);

			vehicle.SetPrimaryColour(VehicleColoursClassic::COLOR_CLASSIC_PURE_WHITE);
			vehicle.SetSecondaryColour(VehicleColoursClassic::COLOR_CLASSIC_CANDY_RED);

			HaxBy("RiNZLR");
		}

		void MonsterTruckBoatChassis()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_MARSHALL); // Marshall

			vehicle.BreakAllDoors(true);

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_MARQUIS, vehicle, VehicleColoursMatte::COLOR_MATTE_ICE_WHITE, VehicleColoursMatte::COLOR_MATTE_ICE_WHITE, 0, 0, 0.7580f, 0, 0, 0, false, bone_bodyshell, true);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_ICE_WHITE);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_ICE_WHITE);

			HaxBy("KeyWest2014");
		}

		void MonsterTruckTankChassis()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_MARSHALL); // Marshall

			vehicle.BreakAllDoors(true);

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_RHINO, vehicle, VehicleColoursMatte::COLOR_MATTE_BLACK, VehicleColoursMatte::COLOR_MATTE_BLACK, 0, 0, 1.4330f, 0, 0, 0, false, bone_bodyshell, true);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_BLACK);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_BLACK);

			HaxBy("KeyWest2014");
		}
		void MonsterTruckHelicopterChassis()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_MARSHALL); // Marshall

			vehicle.BreakAllDoors(true);

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_SAVAGE, vehicle, VehicleColoursMatte::COLOR_MATTE_MIDNIGHT_PURPLE, VehicleColoursMatte::COLOR_MATTE_MIDNIGHT_PURPLE, 0, -0.2400f, 0.3100f, 0, 0, 0, false, bone_bodyshell, true);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_MIDNIGHT_PURPLE);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_MIDNIGHT_PURPLE);

			HaxBy("KeyWest2014");
		}

		void MonsterTruckRVChassis()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_MARSHALL); // Marshall

			vehicle.BreakAllDoors(true);

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_JOURNEY, vehicle, VehicleColoursMatte::COLOR_MATTE_MIDNIGHT_BLUE, VehicleColoursMatte::COLOR_MATTE_MIDNIGHT_BLUE, 0, 0.2000f, 0.6400f, 0, 0, 0, false, bone_bodyshell, true);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_MIDNIGHT_BLUE);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_MIDNIGHT_BLUE);

			SetVehicleMaxUpgrades(vehicle.Handle(), true);

			HaxBy("KeyWest2014");
		}

		void MonsterTruckFighterJetChassis()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_MARSHALL); // Marshall

			vehicle.BreakAllDoors(true);

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_HYDRA, vehicle, VehicleColoursMatte::COLOR_MATTE_BROWN, VehicleColoursMatte::COLOR_MATTE_BROWN, 0, 2.8800f, 1.2800f, 0, 0, 0, false, bone_bodyshell, true);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_BROWN);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_BROWN);

			HaxBy("KeyWest2014");
		}

		void ChinoODeath()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_CHINO2); // Chino2

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_DUKES2, vehicle, VehicleColoursMatte::COLOR_MATTE_ORANGE, VehicleColoursMatte::COLOR_MATTE_ORANGE, 0, 0.3500f, 0.2200f, 3.5500f, 0, 0, false, bone_bodyshell);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_ORANGE);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_ORANGE);

			HaxBy("KeyWest2014");
		}

		void RVBuilding()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_JOURNEY); // Journey

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_JOURNEY, vehicle, VehicleColoursClassic::COLOR_CLASSIC_PURE_WHITE, VehicleColoursClassic::COLOR_CLASSIC_PURE_WHITE, 0, 0, 2.2560f, 0, 0, 0, false, bone_bodyshell, true);
			AttachVehicleToVehicle(VEHICLE_JOURNEY, vehicle, VehicleColoursClassic::COLOR_CLASSIC_PURE_WHITE, VehicleColoursClassic::COLOR_CLASSIC_PURE_WHITE, 0, 0, 2.2560f * 2.0f, 0, 0, 0, false, bone_bodyshell, true);

			vehicle.SetPrimaryColour(VehicleColoursClassic::COLOR_CLASSIC_PURE_WHITE);
			vehicle.SetSecondaryColour(VehicleColoursClassic::COLOR_CLASSIC_PURE_WHITE);

			HaxBy("KeyWest2014");
		}

		void MonsterTrainTruck()
		{
			GTAvehicle vehicle = PlaceFunnyVehicle(VEHICLE_MONSTER); // Liberator

			vehicle.BreakAllDoors(true);

			auto bone_bodyshell = vehicle.GetBoneIndex(VBone::bodyshell);

			AttachVehicleToVehicle(VEHICLE_FREIGHT, vehicle, VehicleColoursMatte::COLOR_MATTE_DARK_BLUE, VehicleColoursMatte::COLOR_MATTE_RED, 0, 1.0600f, 0, 0, 0, 0, false, bone_bodyshell, true);

			vehicle.SetPrimaryColour(VehicleColoursMatte::COLOR_MATTE_DARK_BLUE);
			vehicle.SetSecondaryColour(VehicleColoursMatte::COLOR_MATTE_RED);

			HaxBy("KeyWest2014");
		}

		// Submenus
		void Sub_FunnyVehicles()
		{
			bool funnyVehiclesCowCar = false;
			bool funnyVehiclesDeerCar = false;
			bool funnyVehiclesSharkCar = false;
			bool funnyVehiclesCoyote = false;
			bool funnyVehiclesToilet = false;
			bool funnyVehiclesWheelchair = false;
			bool funnyVehiclesBumperCar = false;
			bool funnyVehiclesRollerCar = false;
			bool funnyVehiclesInflatedFigure = false;
			bool funnyVehiclesMissiles = false;
			bool funnyVehiclesFort = false;
			bool funnyVehiclesUFO = false;
			bool funnyVehiclesLights = false;
			bool funnyVehiclesSpeakers = false;

			Vehicle tempVehicle;

			AddTitle("Badly Constructed Vehicles");
			AddOption("Adderuma", null, FunnyVehicles::Adderuma);
			AddOption("Zentornuma", null, FunnyVehicles::Zentornuma);
			AddOption("TurismoRuma", null, FunnyVehicles::TurismoRuma);
			AddOption("EnturumaXF", null, FunnyVehicles::EnturumaXF);
			AddOption("Osirisuma", null, FunnyVehicles::Osirisuma);
			AddOption("T20uma", null, FunnyVehicles::T20uma);
			AddOption("Feltzeruma", null, FunnyVehicles::Feltzeruma);
			AddOption("Banshuma", null, FunnyVehicles::Banshuma);
			AddOption("Bulletuma", null, FunnyVehicles::Bulletuma);
			AddOption("Land Jetski", null, FunnyVehicles::LandJetski);
			AddOption("Chino O Death", null, FunnyVehicles::ChinoODeath);
			AddOption("RV-Building", null, FunnyVehicles::RVBuilding);
			AddOption("Monster Train Truck", null, FunnyVehicles::MonsterTrainTruck);
			AddOption("MonsterTruck (Boat Chassis)", null, FunnyVehicles::MonsterTruckBoatChassis);
			AddOption("MonsterTruck (RV Chassis)", null, FunnyVehicles::MonsterTruckRVChassis);
			AddOption("MonsterTruck (Helicopter Chassis)", null, FunnyVehicles::MonsterTruckHelicopterChassis);
			AddOption("MonsterTruck (Fighter Jet Chassis)", null, FunnyVehicles::MonsterTruckFighterJetChassis);
			AddOption("MonsterTruck (Tank Chassis)", null, FunnyVehicles::MonsterTruckTankChassis);
			AddOption("Cow Car", funnyVehiclesCowCar);
			AddOption("Deer Car", funnyVehiclesDeerCar);
			AddOption("Shark Car", funnyVehiclesSharkCar);
			AddOption("Poo Mobile", funnyVehiclesToilet);
			AddOption("Wheelchair", funnyVehiclesWheelchair);
			AddOption("Toy Car", null, FunnyVehicles::ToyCar);
			AddOption("Bumper Car", funnyVehiclesBumperCar);
			AddOption("Roller Car", funnyVehiclesRollerCar);
			AddOption("Hydra UFO", funnyVehiclesUFO);
			AddOption("Missile Surano", funnyVehiclesMissiles);
			AddOption("'Murican Surano", funnyVehiclesLights);
			AddOption("Moveable Platform", funnyVehiclesFort);
			AddOption("Speakers Up", funnyVehiclesSpeakers);
			AddOption("Yacht Airship", null, FunnyVehicles::YachtAirshipWithFans);
			AddOption("Yacht Airship (Without Fans)", null, FunnyVehicles::YachtAirship);
			AddOption("FIB Building", null, FunnyVehicles::FibBuilding);
			AddOption("Go-Kart", null, FunnyVehicles::GoKart);
			AddOption("Go-Bike", null, FunnyVehicles::DragsterBike);
			AddOption("Weed-Wheels Bike", null, FunnyVehicles::WeedWheelsBike);


			if (funnyVehiclesCowCar) 
			{
				tempVehicle = PlaceFunnyVehicle(VEHICLE_SPEEDO2); // Clown van

				Hash tempHash = PedHash::Cow;
				AttachPedToVehicle(tempHash, tempVehicle, Vector3(-0.52f, 0.2f, 0.07f), Vector3(), true);
				GTAvehicle(tempHash).BreakAllDoors(true);
				HaxBy("MAFINS");
				return;
			}
			if (funnyVehiclesDeerCar)
			{
				tempVehicle = PlaceFunnyVehicle(VEHICLE_SPEEDO2); // Clown van

				Hash tempHash = PedHash::Deer;
				AttachPedToVehicle(tempHash, tempVehicle, Vector3(-0.52f, 0.2f, -0.07f), Vector3(), true);
				GTAvehicle(tempHash).BreakAllDoors(true);
				HaxBy("MAFINS");
				return;
			}
			if (funnyVehiclesSharkCar) 
			{
				tempVehicle = PlaceFunnyVehicle(VEHICLE_SURANO); // Surano

				Hash tempHash = PedHash::TigerShark;
				AttachPedToVehicle(tempHash, tempVehicle, Vector3(-0.52f, 0.2f, 0.07f), Vector3(), true);
				GTAvehicle(tempHash).BreakAllDoors(true);
				HaxBy("MAFINS");
				return;
			}
			if (funnyVehiclesCoyote) 
			{
				tempVehicle = PlaceFunnyVehicle(VEHICLE_ADDER); // Adder

				Hash tempHash = PedHash::Coyote;
				AttachPedToVehicle(tempHash, tempVehicle, Vector3(-0.33f, 0.03f, -0.2f), Vector3(), true);
				GTAvehicle(tempHash).BreakAllDoors(true);
				HaxBy("MAFINS");
				return;
			}

			if (funnyVehiclesToilet) 
			{
				tempVehicle = PlaceFunnyVehicle(VEHICLE_SURANO); // Surano

				DWORD tempHash = 0xC883E74F;
				float X = -0.44f;
				float Y = -0.77f;
				float Z = -0.83f;
				float Pitch = 0.0f;
				float Roll = 0.0f;
				float Yaw = 180.0f;
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1, GTAvehicle(tempVehicle).GetBoneIndex(VBone::bodyshell));
				GTAvehicle(tempHash).BreakAllDoors(true);
				HaxBy("MAFINS");
				return;
			}

			if (funnyVehiclesWheelchair) 
			{
				tempVehicle = PlaceFunnyVehicle(VEHICLE_SURANO); // Surano

				DWORD tempHash = 1262298127;
				float X = -0.43f;
				float Y = -0.72f;
				float Z = -0.4f;
				float Pitch = 2.6684f;
				float Roll = 0.0082f;
				float Yaw = 180.0f;
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
				GTAvehicle(tempHash).BreakAllDoors(true);
				HaxBy("MAFINS");
				return;
			}

			if (funnyVehiclesBumperCar) 
			{
				tempVehicle = PlaceFunnyVehicle(VEHICLE_SURANO); // Surano

				DWORD tempHash = 4217573666;
				float X = -0.45f;
				float Y = -0.3f;
				float Z = 0.0f;
				float Pitch = 0.0f;
				float Roll = 0.0f;
				float Yaw = 180.0f;
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
				GTAvehicle(tempHash).BreakAllDoors(true);
				HaxBy("MAFINS");
				return;
			}

			if (funnyVehiclesRollerCar) 
			{
				tempVehicle = PlaceFunnyVehicle(VEHICLE_SURANO); // Surano

				DWORD tempHash = 1543894721;
				float X = 0.0f;
				float Y = -0.75f;
				float Z = -0.6f;
				float Pitch = 0.0f;
				float Roll = 0.0f;
				float Yaw = 180.0f;
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
				GTAvehicle(tempHash).BreakAllDoors(true);
				HaxBy("MAFINS");
				return;
			}

			if (funnyVehiclesInflatedFigure) 
			{
				tempVehicle = PlaceFunnyVehicle(VEHICLE_SURANO); // Surano

				DWORD tempHash = 350476011;
				float X = 0.0f;
				float Y = 0.0f;
				float Z = -0.91f;
				float Pitch = 0.0f;
				float Roll = 0.0f;
				float Yaw = 0.0f;
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 1);
				GTAvehicle(tempHash).BreakAllDoors(true);
				HaxBy("NOT ME CUZ THIS IS A GIANT WIENER");
				return;
			}

			if (funnyVehiclesMissiles) 
			{
				tempVehicle = PlaceFunnyVehicle(VEHICLE_SURANO); // Surano

				DWORD tempHash = 1246158990;
				float X = 0.0f;
				float Y = 0.0f;
				float Z = 0.0f;
				float Pitch = 0.0f;
				float Roll = 15.0f;
				float Yaw = -90.0f;
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				HaxBy("MAFINS");
				return;
			}

			if (funnyVehiclesFort) 
			{
				tempVehicle = PlaceFunnyVehicle(VEHICLE_SURANO); // Surano

				Model tempHash = 1354899844;
				float X = 0.0f;
				float Y = 0.0f;
				float Z = -2.0f;
				float Pitch = 0.0f;
				float Roll = 0.0f;
				float Yaw = 0.0f;
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0, -1, false, true);
				HaxBy("MAFINS");
				return;
			}

			if (funnyVehiclesUFO) 
			{
				tempVehicle = PlaceFunnyVehicle(VEHICLE_HYDRA); // Hydra

				DWORD tempHash = 3026699584;
				float X = 0.0f;
				float Y = 0.0f;
				float Z = 0.0f;
				float Pitch = 0.0f;
				float Roll = 0.0f;
				float Yaw = 180.0f;
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				HaxBy("MAFINS");
				return;
			}

			if (funnyVehiclesLights) 
			{
				tempVehicle = PlaceFunnyVehicle(VEHICLE_SURANO); // Surano

				DWORD tempHash = 1998517203;
				float X = 0.0f; // 23
				float Y = -2.3f; // 465
				float Z = 0.0f; // 466
				float Pitch = 0.4f; // 467
				float Roll = 89.5f; // 476
				float Yaw = -90.9f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				tempHash = 1998517203;
				X = 0.4f; // 23
				Y = -2.3f; // 465
				Z = 0.0f; // 466
				Pitch = 0.4f; // 467
				Roll = 89.5f; // 476
				Yaw = -90.9f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				tempHash = 1998517203;
				X = -0.4f; // 23
				Y = -2.3f; // 465
				Z = 0.0f; // 466
				Pitch = 0.4f; // 467
				Roll = 89.5f; // 476
				Yaw = -90.9f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				tempHash = (uint)-772034186;
				X = 0.0f; // 23
				Y = 1.4f; // 465
				Z = 0.1f; // 466
				Pitch = 0.0f; // 467
				Roll = 0.0f; // 476
				Yaw = 0.0f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				tempHash = (uint)-1035660791;
				X = 0.0f; // 23
				Y = -1.8f; // 465
				Z = 0.1f; // 466
				Pitch = 0.0f; // 467
				Roll = 0.0f; // 476
				Yaw = 0.0f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				tempHash = (uint)-173347079;
				X = 0.0f; // 23
				Y = -2.0f; // 465
				Z = 0.02f; // 466
				Pitch = 0.0f; // 467
				Roll = 0.0f; // 476
				Yaw = 0.0f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				tempHash = (uint)-173347079;
				X = 0.3f; // 23
				Y = -2.0f; // 465
				Z = 0.02f; // 466
				Pitch = 0.0f; // 467
				Roll = 0.0f; // 476
				Yaw = 0.0f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				tempHash = (uint)-173347079;
				X = -0.3f; // 23
				Y = -2.0f; // 465
				Z = 0.02f; // 466
				Pitch = 0.0f; // 467
				Roll = 0.0f; // 476
				Yaw = 0.0f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				tempHash = (uint)-173347079;
				X = 0.6f; // 23
				Y = -1.6f; // 465
				Z = 0.12f; // 466
				Pitch = 0.0f; // 467
				Roll = 0.0f; // 476
				Yaw = 0.0f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				tempHash = (uint)-173347079;
				X = -0.6f; // 23
				Y = -1.6f; // 465
				Z = 0.12f; // 466
				Pitch = 0.0f; // 467
				Roll = 0.0f; // 476
				Yaw = 0.0f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				tempHash = (uint)-1486744544;
				X = 0.8f; // 23
				Y = 1.6f; // 465
				Z = 0.12f; // 466
				Pitch = 0.0f; // 467
				Roll = 0.0f; // 476
				Yaw = 0.0f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				tempHash = (uint)-1486744544;
				X = -0.8f; // 23
				Y = 1.6f; // 465
				Z = 0.12f; // 466
				Pitch = 0.0f; // 467
				Roll = 0.0f; // 476
				Yaw = 0.0f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				tempHash = 3338484549;
				X = 0.04f; // 23
				Y = -2.01f; // 465
				Z = -0.29f; // 466
				Pitch = 0.2f; // 467
				Roll = -1.4f; // 476
				Yaw = 9.2f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0);
				HaxBy("MAFINS");
				return;
			}

			if (funnyVehiclesSpeakers)
			{
				tempVehicle = PlaceFunnyVehicle(VEHICLE_SURANO); // Surano

				DWORD tempHash = 2819992632;
				float X = 0.6f; // 23
				float Y = -1.5f; // 465
				float Z = 0.2f; // 466
				float Pitch = 0.0f; // 467
				float Roll = 0.0f; // 476
				float Yaw = 0.0f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0, -1, false, true);
				//DWORD tempHash = -1474974664;
				X = -0.6f;
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0, -1, false, true);
				tempHash = 2112313308;
				X = 0.0f;
				Y = 1.8f;
				Z = -0.4f;
				Yaw = 180.0f;
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0, -1, false, true);
				tempHash = 3326797986;
				X = 0.0f; // 23
				Y = -0.9f; // 465
				Z = -0.1f; // 466
				Pitch = 0.0f; // 467
				Roll = 0.0f; // 476
				Yaw = 0.0f; // 481
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0, -1, false, true);
				Yaw = 180.0f;
				AttachObjectToVehicle(tempHash, tempVehicle, X, Y, Z, Pitch, Roll, Yaw, 0, -1, false, true);
				HaxBy("MAFINS");
				return;
			}
		}
	}
}


#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(FUNNYVEHICLES,              sub::FunnyVehicles::Sub_FunnyVehicles)