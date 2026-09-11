#include "SpoonerInteractables.h"
#include "..\..\Menu\Menu.h"
#include "Scripting/Game.h"
#include "..\..\Scripting\Model.h"
#include "Submenus/Spooner/EntityManagement.h"
#include "Scripting/GTAped.h"
#include <algorithm>

namespace sub::Spooner::Interactables
{
	std::vector<InteractableDefinition> vInteractables =
	{
		{
			"Grab Cash",
			GET_HASH_KEY("h4_prop_h4_cash_stack_01a"),
			"anim@scripted@player@mission@tun_table_grab@cash@",
			{

			{
				{ { AnimTarget::Player, "enter", 1000.0f, -8.0f, 0, 0, 0.0f, 0 } },
				6000,
				1.0f
			},

		{
			{
				{ AnimTarget::Player, "grab",      1000.0f, -8.0f, 0, 0, 0.0f, 0 },
				{ AnimTarget::Prop,   "grab_cash",  1000.0f, -8.0f, 0, 0, 0.0f, 0, 0, 1.0f }
			},
			6000,
			1.0f
		},

		{
			{ { AnimTarget::Player, "exit", 1000.0f, -8.0f, 0, 0, 0.0f, 0 } },
			6000,
			1.0f
		}
	}
},
	};

	std::vector<InteractableInstance> vActiveInteractables;

	SpoonerEntity SpawnInteractable(const InteractableDefinition& def)
	{
		SpoonerEntity ent = EntityManagement::AddProp(def.PropModel, def.Label);
		if (!ent.Handle.Exists())
			return SpoonerEntity();

		InteractableInstance inst;
		inst.Ent = ent;
		inst.Def = &def;
		vActiveInteractables.push_back(inst);

		return ent;
	}

	static void StartStage(InteractableInstance& inst, size_t stageIndex)
	{
		const InteractableStage& stage = inst.Def->Stages[stageIndex];

		Vector3 pos = inst.Ent.Handle.Position_get();
		Vector3 rot = inst.Ent.Handle.Rotation_get();

		int syncedScene = CREATE_SYNCHRONIZED_SCENE(pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, 2);
		inst.SyncedScene = syncedScene;
		inst.ActivationTime = GET_GAME_TIMER();

		if (syncedScene == -1)
		{
			Game::Print::PrintBottomCentre("~r~Error:~s~ Failed to create synced scene.");
			return;
		}

		SET_SYNCHRONIZED_SCENE_HOLD_LAST_FRAME(syncedScene, true);
		SET_SYNCHRONIZED_SCENE_RATE(syncedScene, stage.SceneRate);

		for (auto& p : stage.Participants)
		{
			if (p.Target == AnimTarget::Player)
			{
				GTAped myPed = PLAYER_PED_ID();
				myPed.CanRagdoll_set(false);
				SET_PED_RAGDOLL_ON_COLLISION(myPed.Handle(), false);
				inst.bRagdollDisabled = true;

				TASK::TASK_SYNCHRONIZED_SCENE(
					myPed.Handle(), syncedScene,
					inst.Def->AnimDict.c_str(), p.ClipName.c_str(),
					p.BlendIn, p.BlendOut,
					p.Flags, p.RagdollBlockingFlags, p.MoverBlendDelta, p.IkFlags
				);
			}
			else // Prop
			{
				ENTITY::PLAY_SYNCHRONIZED_ENTITY_ANIM(
					inst.Ent.Handle.Handle(), syncedScene,
					p.ClipName.c_str(), inst.Def->AnimDict.c_str(),
					p.BlendIn, p.BlendOut, p.PropFlags, p.PlaybackRate
				);
			}
		}
	}

	static void ActivateInteractable(InteractableInstance& inst)
	{
		STREAMING::REQUEST_ANIM_DICT(inst.Def->AnimDict.c_str());
		int waitCount = 0;
		while (!STREAMING::HAS_ANIM_DICT_LOADED(inst.Def->AnimDict.c_str()) && waitCount++ < 500)
			WAIT(0);

		inst.CurrentStage = 0;
		StartStage(inst, 0);
		inst.bActivated = true;
	}

	void Tick()
	{
		if (vActiveInteractables.empty())
			return;

		GTAped myPed = PLAYER_PED_ID();
		Vector3 myPos = myPed.Position_get();

		for (auto& inst : vActiveInteractables)
		{
			if (!inst.Ent.Handle.Exists())
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
							myPed.CanRagdoll_set(true);
							SET_PED_RAGDOLL_ON_COLLISION(myPed.Handle(), true);
							inst.bRagdollDisabled = false;
						}

						EntityManagement::DeleteEntity(inst.Ent);
					}
				}

				continue;
			}

			Vector3 entPos = inst.Ent.Handle.Position_get();
			float dist = GET_DISTANCE_BETWEEN_COORDS(myPos.x, myPos.y, myPos.z, entPos.x, entPos.y, entPos.z, true);

			if (dist <= InteractRange)
			{
				Game::Print::setupdraw(GTAfont::Arial, Vector2(0, 0.4f), false, true, false, RGBA(255, 255, 255, 190));
				Game::Print::drawstring("Press ~b~[E]~s~ to interact.", NULL, 0.1f);

				if (PAD::IS_CONTROL_JUST_PRESSED(0, INPUT_CONTEXT))
				{
					ActivateInteractable(inst);
				}
			}
		}

		vActiveInteractables.erase(
			std::remove_if(vActiveInteractables.begin(), vActiveInteractables.end(),
				[](const InteractableInstance& inst) { return !inst.Ent.Handle.Exists(); }),
			vActiveInteractables.end()
		);
	}
}