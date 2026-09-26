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
	static const std::vector<KnownBagVariant> vKnownBagVariants =
	{
		{ 5, 40, -1, GET_HASH_KEY("p_ld_heist_bag_s_pro2_s") },
		{ 5, 41, -1, GET_HASH_KEY("p_ld_heist_bag_s_pro2_s") },

		{ 5, 81, 0,  GET_HASH_KEY("ch_p_m_bag_var02_arm_s") },
		{ 5, 81, 2,  GET_HASH_KEY("xm3_p_xm3_m_bag_var22_arm_s") },
		{ 5, 81, 4,  GET_HASH_KEY("m23_2_p_m23_m_bag_var22_arm_s_g") },
		{ 5, 81, 8,  GET_HASH_KEY("ch_p_m_bag_var04_arm_s") },
		{ 5, 81, 9,  GET_HASH_KEY("ch_p_m_bag_var01_arm_s") },
		{ 5, 81, 10, GET_HASH_KEY("ch_p_m_bag_var08_arm_s") },
		{ 5, 81, 11, GET_HASH_KEY("ch_p_m_bag_var09_arm_s") },
		{ 5, 81, 12, GET_HASH_KEY("ch_p_m_bag_var06_arm_s") },
		{ 5, 81, 13, GET_HASH_KEY("ch_p_m_bag_var05_arm_s") },
		{ 5, 81, 14, GET_HASH_KEY("ch_p_m_bag_var10_arm_s") },
		{ 5, 81, 15, GET_HASH_KEY("ch_p_m_bag_var07_arm_s") },

		{ 5, 82, 0,  GET_HASH_KEY("ch_p_m_bag_var02_arm_s") },
		{ 5, 82, 2,  GET_HASH_KEY("xm3_p_xm3_m_bag_var22_arm_s") },
		{ 5, 82, 4,  GET_HASH_KEY("m23_2_p_m23_m_bag_var22_arm_s_g") },
		{ 5, 82, 8,  GET_HASH_KEY("ch_p_m_bag_var04_arm_s") },
		{ 5, 82, 9,  GET_HASH_KEY("ch_p_m_bag_var01_arm_s") },
		{ 5, 82, 10, GET_HASH_KEY("ch_p_m_bag_var08_arm_s") },
		{ 5, 82, 11, GET_HASH_KEY("ch_p_m_bag_var09_arm_s") },
		{ 5, 82, 12, GET_HASH_KEY("ch_p_m_bag_var06_arm_s") },
		{ 5, 82, 13, GET_HASH_KEY("ch_p_m_bag_var05_arm_s") },
		{ 5, 82, 14, GET_HASH_KEY("ch_p_m_bag_var10_arm_s") },
		{ 5, 82, 15, GET_HASH_KEY("ch_p_m_bag_var07_arm_s") },
	};

	struct WornBagInfo { bool Found = false; int Slot = -1; int DrawableId = -1; int TextureId = -1; };

	static WornBagInfo DetectWornBag(GTAped& myPed)
	{
		for (int slot : { 9, 5 })
		{
			int drawableId = PED::GET_PED_DRAWABLE_VARIATION(myPed.Handle(), slot);
			if (drawableId <= 0) continue;
			int textureId = PED::GET_PED_TEXTURE_VARIATION(myPed.Handle(), slot);
			return { true, slot, drawableId, textureId };
		}
		return {};
	}

	static Hash ResolveWornBagModel(const WornBagInfo& worn, Hash fallbackModel)
	{
		if (!worn.Found) return fallbackModel;
		for (auto& variant : vKnownBagVariants)
		{
			if (variant.Slot == worn.Slot && variant.DrawableId == worn.DrawableId &&
				(variant.TextureId == -1 || variant.TextureId == worn.TextureId))
				return variant.PropModel;
		}
		return fallbackModel;
	}

	static const std::vector<BagDrawablePair> vBagDrawablePairs =
	{
		{ 9, 1,  2 },
		{ 9, 12, 11 },
		{ 5, 41, 40 },
		{ 5, 45, 44 },
		{ 5, 81, 82 },
		{ 5, 86, 85 },
	};

	Vector3 GetInteractPosition(const InteractableInstance& inst)
	{
		float headingRad = -inst.AnchorRot.z * 3.14159f / 180.0f;
		Vector3 forward = Vector3(sinf(headingRad), cosf(headingRad), 0.0f);
		Vector3 right = Vector3(cosf(headingRad), -sinf(headingRad), 0.0f);

		return inst.AnchorPos
			+ (right * inst.Def->PlayerStandOffset.x)
			+ (forward * inst.Def->PlayerStandOffset.y)
			+ Vector3(0.0f, 0.0f, inst.Def->PlayerStandOffset.z);
	}

	std::vector<InteractableDefinition> vInteractables =
	{
		{
			"Grab Cash",
			GET_HASH_KEY("h4_prop_h4_cash_stack_01a"),
			{
				{ "Duffel Bag", GET_HASH_KEY("h4_p_h4_m_bag_var22_arm_s"), true }
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
			},
			BagOpenState::NoChange,
			true,{},
			Vector3(0.0f, -0.85f, 0.10f), 0.0f,
			0.50f
		},
		{
			"Open Container",
			GET_HASH_KEY("m23_2_prop_m32_container_01a"),
			{
				{
					"Opened Container",
					GET_HASH_KEY("m23_2_prop_m32_container_01b"),
					false,
					ExtraPropAttachTarget::PrimaryProp,
					true
				}
			},
			"anim@scripted@freemode@ig1_open_container@no_tools@male@",
			{
				{
					{
						{ AnimTarget::Player,      "action" },
						{ AnimTarget::PrimaryProp, "action_container" }
					},
					6000, 1.0f, "action_facial",
					1000.0f, -8.0f, 0, 0, 0.0f, 0, 0, 1000.0f
				}
			},
			BagOpenState::NoChange,
			false,
			{
				{ AnimTarget::PrimaryProp, 0, 0,   false, false },
				{ AnimTarget::ExtraProp,   0, 255, true,  true }
			},
			Vector3(0.0f, -3.90f, 1.25f), 0.0f,
			0.50f
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

		// Player placement: local offset (right, forward, up) rotated by the anchor's heading.
		float headingRad = -inst.AnchorRot.z * 3.14159f / 180.0f;
		Vector3 forward = Vector3(sinf(headingRad), cosf(headingRad), 0.0f);
		Vector3 right = Vector3(cosf(headingRad), -sinf(headingRad), 0.0f); // unverified handedness - tune in-game if left/right is flipped

		Vector3 standPos = GetInteractPosition(inst);
		myPed.SetPosition(standPos);
		myPed.SetHeading(inst.AnchorRot.z + inst.Def->PlayerHeadingOffset);

		myPed.SetPosition(standPos);
		myPed.SetHeading(inst.AnchorRot.z + inst.Def->PlayerHeadingOffset);

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
			else
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

		if (!STREAMING::HAS_ANIM_DICT_LOADED(inst.Def->AnimDict.c_str()))
		{
			Game::Print::PrintBottomCentre("~r~Error:~s~ Anim dict failed to load.");
			return; // abort cleanly - never place the player or start a broken task
		}

		GTAped myPed = PLAYER_PED_ID();
		int rootBone = myPed.GetBoneIndex(Bone::SKEL_ROOT);

		bool needsBagDetection = std::any_of(inst.Def->ExtraProps.begin(), inst.Def->ExtraProps.end(),
			[](const ExtraPropDef& d) { return d.MatchWornComponent; });

		WornBagInfo wornBag;
		if (needsBagDetection)
		{
			wornBag = DetectWornBag(myPed);
			inst.WornBagSlot = wornBag.Slot;
			inst.WornBagDrawable = wornBag.DrawableId;
			inst.WornBagTexture = wornBag.TextureId;

			if (wornBag.Found)
			{
				PED::SET_PED_COMPONENT_VARIATION(myPed.Handle(), wornBag.Slot, 0, 0, 0);
			}
		}

		inst.ExtraEnts.clear();
		for (auto& extraDef : inst.Def->ExtraProps)
		{
			Hash modelToSpawn = extraDef.MatchWornComponent
				? ResolveWornBagModel(wornBag, extraDef.PropModel)
				: extraDef.PropModel;

			SpoonerEntity extraEnt = EntityManagement::AddProp(modelToSpawn, extraDef.Label);
			if (extraEnt.handle.Exists())
			{
				extraEnt.handle.SetIsCollisionEnabled(false);

				if (extraDef.StartInvisible)
				{
					extraEnt.handle.SetVisible(false);
				}

				GTAentity attachTarget = (extraDef.AttachTarget == ExtraPropAttachTarget::PrimaryProp)
					? inst.Ent.handle
					: (GTAentity)myPed;
				int attachBone = (extraDef.AttachTarget == ExtraPropAttachTarget::PrimaryProp) ? 0 : rootBone;

				EntityManagement::AttachEntity(extraEnt, attachTarget, attachBone, Vector3(), Vector3());

				// Keep it out of the visible entity database - our own ExtraEnts
				// tracking is the sole owner of its lifecycle from here on.
				EntityManagement::RemoveEntityFromDb(extraEnt);
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
			{
				for (auto& extraEnt : inst.ExtraEnts)
				{
					if (extraEnt.handle.Exists())
						EntityManagement::DeleteEntity(extraEnt);
				}
				inst.ExtraEnts.clear();
				continue;
			}

			if (inst.bCleanupDone)
				continue; // already finished - stop touching this instance forever

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

						if (inst.WornBagSlot != -1)
						{
							int targetDrawable = inst.WornBagDrawable;

							if (inst.Def->PlayerBagStateAfter != BagOpenState::NoChange)
							{
								for (auto& pair : vBagDrawablePairs)
								{
									if (pair.Slot == inst.WornBagSlot &&
										(pair.ClosedDrawableId == inst.WornBagDrawable || pair.OpenedDrawableId == inst.WornBagDrawable))
									{
										targetDrawable = (inst.Def->PlayerBagStateAfter == BagOpenState::Opened)
											? pair.OpenedDrawableId
											: pair.ClosedDrawableId;
										break;
									}
								}
							}

							PED::SET_PED_COMPONENT_VARIATION(myPed.Handle(), inst.WornBagSlot, targetDrawable, inst.WornBagTexture, 0);
						}

						if (inst.Def->DeletePropsOnComplete)
						{
							EntityManagement::DeleteEntity(inst.Ent);
							for (auto& extraEnt : inst.ExtraEnts)
							{
								if (extraEnt.handle.Exists())
									EntityManagement::DeleteEntity(extraEnt);
							}
							inst.ExtraEnts.clear();
						}
						else
						{
							for (auto& finalState : inst.Def->FinalPropStates)
							{
								SpoonerEntity& targetEnt = (finalState.Target == AnimTarget::PrimaryProp)
									? inst.Ent
									: inst.ExtraEnts[finalState.ExtraPropIndex];

								if (targetEnt.handle.Exists())
								{
									targetEnt.handle.SetAlpha(finalState.Alpha);
									targetEnt.handle.SetIsCollisionEnabled(finalState.CollisionEnabled);
									targetEnt.handle.SetVisible(finalState.Visible);
								}
							}
						}

						inst.bCleanupDone = true; // NEW - stops this branch from ever running again for this instance
					}
				}

				continue;
			}

			Vector3 entPos = GetInteractPosition(inst);
			float dist = GET_DISTANCE_BETWEEN_COORDS(myPos.x, myPos.y, myPos.z, entPos.x, entPos.y, entPos.z, true);

			if (dist <= inst.Def->InteractRange)
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
				[](const InteractableInstance& inst) { return !inst.Ent.handle.Exists() || inst.bCleanupDone; }),
			vActiveInteractables.end()
		);
	}
}