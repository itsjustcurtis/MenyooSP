#include "SpoonerInteractables.h"
#include "Menu/Menu.h"
#include "Scripting/Game.h"
#include "Scripting/Model.h"
#include "Submenus/Spooner/EntityManagement.h"
#include "Scripting/GTAped.h"
#include "Scripting/GTAentity.h"
#include "Submenus/Spooner/Submenus.h"
#include <algorithm>

namespace sub::Spooner::Interactables
{
	std::vector<InteractableDefinition> vInteractables =
	{
		{
			"Grab Cash",
			GET_HASH_KEY("h4_prop_h4_cash_stack_01a"),
			{
				{ "Duffel Bag", GET_HASH_KEY("ch_p_m_bag_var01_arm_s") }
			},
			"anim@scripted@player@mission@tun_table_grab@cash@",
			{
				{
					{
						{ AnimTarget::ExtraProp, "enter_bag" },
						{ AnimTarget::Player,    "enter" }
					},
					6000, 1.0f, "enter_facial",
					1000.0f, -8.0f, 0, 0, 0.0f, 0, 0, 1000.0f
				},
				{
					{
						{ AnimTarget::PrimaryProp, "grab_cash" },
						{ AnimTarget::ExtraProp,   "grab_bag" },
						{ AnimTarget::Player,      "grab" }
					},
					30000, 1.0f, "grab_facial",
					1000.0f, -8.0f, 0, 0, 0.0f, 0, 0, 1000.0f
				},
				{
					{
						{ AnimTarget::ExtraProp, "exit_bag" },
						{ AnimTarget::Player,    "exit" }
					},
					6000, 1.0f, "exit_facial",
					1000.0f, -8.0f, 0, 0, 0.0f, 0, 0, 1000.0f
				}
			}
		},
	};

	std::vector<InteractableInstance> vActiveInteractables;

	SpoonerEntity SpawnInteractable(const InteractableDefinition& def)
	{
		SpoonerEntity ent = EntityManagement::AddProp(def.PropModel, def.Label);
		if (!ent.handle.Exists())
			return SpoonerEntity();

		InteractableInstance inst;
		inst.Ent = ent;
		inst.Def = &def;
		inst.AnchorPos = ent.handle.GetPosition();
		inst.AnchorRot = ent.handle.GetRotation();
		vActiveInteractables.push_back(inst);

		return ent;
	}

	static void StartStage(InteractableInstance& inst, size_t stageIndex)
	{
		const InteractableStage& stage = inst.Def->Stages[stageIndex];

		GTAped myPed = PLAYER_PED_ID();

		Vector3 forward = Vector3(sinf(-inst.AnchorRot.z * 3.14159f / 180.0f), cosf(-inst.AnchorRot.z * 3.14159f / 180.0f), 0.0f);
		Vector3 standPos = inst.AnchorPos - (forward * 1.0f);
		myPed.SetPosition(standPos);
		myPed.SetHeading(inst.AnchorRot.z);

		int syncedScene = CREATE_SYNCHRONIZED_SCENE(inst.AnchorPos.x, inst.AnchorPos.y, inst.AnchorPos.z, inst.AnchorRot.x, inst.AnchorRot.y, inst.AnchorRot.z, 2);
		inst.SyncedScene = syncedScene;
		inst.ActivationTime = GET_GAME_TIMER();

		if (syncedScene == -1)
		{
			Game::Print::PrintBottomCentre("~r~Error:~s~ Failed to create synced scene.");
			return;
		}

		SET_SYNCHRONIZED_SCENE_HOLD_LAST_FRAME(syncedScene, true);
		SET_SYNCHRONIZED_SCENE_RATE(syncedScene, stage.SceneRate);

		if (!stage.FacialClip.empty())
		{
			PED::PLAY_FACIAL_ANIM(myPed.Handle(), stage.FacialClip.c_str(), inst.Def->AnimDict.c_str());
		}

		for (auto& p : stage.Participants)
		{
			if (p.Target == AnimTarget::Player)
			{
				myPed.SetCanRagdoll(false);
				SET_PED_RAGDOLL_ON_COLLISION(myPed.Handle(), false);
				inst.bRagdollDisabled = true;

				TASK::TASK_SYNCHRONIZED_SCENE(
					myPed.Handle(), syncedScene,
					inst.Def->AnimDict.c_str(), p.ClipName.c_str(),
					stage.BlendIn, stage.BlendOut,
					stage.Flags, stage.RagdollBlockingFlags, stage.MoverBlendDelta, stage.IkFlags
				);
			}
			else // PrimaryProp or ExtraProp
			{
				SpoonerEntity& targetEnt = (p.Target == AnimTarget::PrimaryProp)
					? inst.Ent
					: inst.ExtraEnts[p.ExtraPropIndex];

				EntityManagement::DetachEntity(targetEnt);
				targetEnt.handle.FreezePosition(false);

				if (p.Target == AnimTarget::PrimaryProp)
				{
					targetEnt.handle.SetPosition(inst.AnchorPos);
					targetEnt.handle.SetRotation(inst.AnchorRot);
				}

				ENTITY::PLAY_SYNCHRONIZED_ENTITY_ANIM(
					targetEnt.handle.Handle(), syncedScene,
					p.ClipName.c_str(), inst.Def->AnimDict.c_str(),
					stage.BlendIn, stage.BlendOut, stage.PropFlags, stage.PlaybackRate
				);
			}
		}
	}

	static void ActivateInteractable(InteractableInstance& inst)
	{
		inst.Ent.handle.FreezePosition(false);

		STREAMING::REQUEST_ANIM_DICT(inst.Def->AnimDict.c_str());
		int waitCount = 0;
		while (!STREAMING::HAS_ANIM_DICT_LOADED(inst.Def->AnimDict.c_str()) && waitCount++ < 500)
			WAIT(0);

		GTAped myPed = PLAYER_PED_ID();
		int rootBone = myPed.GetBoneIndex(Bone::SKEL_ROOT);

		inst.ExtraEnts.clear();
		for (auto& extraDef : inst.Def->ExtraProps)
		{
			SpoonerEntity extraEnt = EntityManagement::AddProp(extraDef.PropModel, extraDef.Label);
			if (extraEnt.handle.Exists())
			{
				extraEnt.handle.SetIsCollisionEnabled(false);
				EntityManagement::AttachEntity(extraEnt, myPed, rootBone, Vector3(), Vector3());
			}
			inst.ExtraEnts.push_back(extraEnt);
		}

		inst.CurrentStage = 0;
		StartStage(inst, 0);
		inst.bActivated = true;
	}

	void Tick()
	{
		if (vActiveInteractables.empty())
			return;

		GTAped myPed = PLAYER_PED_ID();
		Vector3 myPos = myPed.GetPosition();

		for (auto& inst : vActiveInteractables)
		{
			if (!inst.Ent.handle.Exists())
				continue;

			if (inst.bActivated)
			{
				const InteractableStage& stage = inst.Def->Stages[inst.CurrentStage];
				bool sceneFinished = (inst.SyncedScene == -1) || GET_SYNCHRONIZED_SCENE_PHASE(inst.SyncedScene) >= 0.99f;
				bool timedOut = GET_GAME_TIMER() - inst.ActivationTime >= (DWORD)stage.MaxDurationMs;

				if (sceneFinished || timedOut)
				{
					if (inst.CurrentStage + 1 < inst.Def->Stages.size())
					{
						inst.CurrentStage++;
						StartStage(inst, inst.CurrentStage);
					}
					else
					{
						CLEAR_PED_TASKS_IMMEDIATELY(myPed.Handle());

						if (inst.bRagdollDisabled)
						{
							myPed.SetCanRagdoll(true);
							SET_PED_RAGDOLL_ON_COLLISION(myPed.Handle(), true);
							inst.bRagdollDisabled = false;
						}

						EntityManagement::DeleteEntity(inst.Ent);
						for (auto& extraEnt : inst.ExtraEnts)
						{
							if (extraEnt.handle.Exists())
								EntityManagement::DeleteEntity(extraEnt);
						}
						inst.ExtraEnts.clear();
					}
				}

				continue;
			}

			Vector3 entPos = inst.Ent.handle.GetPosition();
			float dist = GET_DISTANCE_BETWEEN_COORDS(myPos.x, myPos.y, myPos.z, entPos.x, entPos.y, entPos.z, true);

			if (dist <= InteractRange)
			{
				Game::Print::SetupDraw(GTAfont::Arial, Vector2(0, 0.4f), false, true, false, RGBA(255, 255, 255, 190));
				Game::Print::drawstring("Press ~b~[E]~s~ to interact.", NULL, 0.1f);

				if (PAD::IS_CONTROL_JUST_PRESSED(0, INPUT_CONTEXT))
				{
					ActivateInteractable(inst);
				}
			}
		}

		vActiveInteractables.erase(
			std::remove_if(vActiveInteractables.begin(), vActiveInteractables.end(),
				[](const InteractableInstance& inst) { return !inst.Ent.handle.Exists(); }),
			vActiveInteractables.end()
		);
	}
}