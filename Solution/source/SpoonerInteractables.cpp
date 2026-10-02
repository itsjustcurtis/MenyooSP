#include "SpoonerInteractables.h"
#include "Menu/Menu.h"
#include "Scripting/Game.h"
#include "Scripting/Model.h"
#include "Submenus/Spooner/EntityManagement.h"
#include "Scripting/GTAped.h"
#include "Scripting/GTAentity.h"
#include "Submenus/Spooner/Submenus.h"
#include <algorithm>
#include <cstdlib>

namespace sub::Spooner::Interactables
{
	static const Hash CashStack = GET_HASH_KEY("h4_prop_h4_cash_stack_01a");
	static const Hash WeedStack = GET_HASH_KEY("h4_prop_h4_weed_stack_01a");
	static const Hash GoldStack = GET_HASH_KEY("h4_prop_h4_gold_stack_01a");
	static const Hash CokeStack = GET_HASH_KEY("h4_prop_h4_coke_stack_01a");
	static const Hash BagFallback = GET_HASH_KEY("h4_p_h4_m_bag_var22_arm_s");

	static const Hash ContainerClosed = GET_HASH_KEY("m23_2_prop_m32_container_01a");
	static const Hash ContainerOpened = GET_HASH_KEY("m23_2_prop_m32_container_01b");

	static const Hash Container_MW_Closed = GET_HASH_KEY("m23_1_prop_m31_container_01a");
	static const Hash Container_MW_Opened = GET_HASH_KEY("m23_1_prop_m31_container_01b");

	static const Hash BagPro2 = GET_HASH_KEY("p_ld_heist_bag_s_pro2_s");
	static const Hash BagVar01 = GET_HASH_KEY("ch_p_m_bag_var01_arm_s");
	static const Hash BagVar02 = GET_HASH_KEY("ch_p_m_bag_var02_arm_s");
	static const Hash BagVar04 = GET_HASH_KEY("ch_p_m_bag_var04_arm_s");
	static const Hash BagVar05 = GET_HASH_KEY("ch_p_m_bag_var05_arm_s");
	static const Hash BagVar06 = GET_HASH_KEY("ch_p_m_bag_var06_arm_s");
	static const Hash BagVar07 = GET_HASH_KEY("ch_p_m_bag_var07_arm_s");
	static const Hash BagVar08 = GET_HASH_KEY("ch_p_m_bag_var08_arm_s");
	static const Hash BagVar09 = GET_HASH_KEY("ch_p_m_bag_var09_arm_s");
	static const Hash BagVar10 = GET_HASH_KEY("ch_p_m_bag_var10_arm_s");
	static const Hash BagVar22 = GET_HASH_KEY("xm3_p_xm3_m_bag_var22_arm_s");
	static const Hash BagVar22_Green = GET_HASH_KEY("m23_2_p_m32_m_bag_var22_arm_s_g");

	static const Hash TrContainer01a = GET_HASH_KEY("tr_prop_tr_container_01a");
	static const Hash TrContainer01b = GET_HASH_KEY("tr_prop_tr_container_01b");
	static const Hash TrContainer01c = GET_HASH_KEY("tr_prop_tr_container_01c");
	static const Hash TrContainer01d = GET_HASH_KEY("tr_prop_tr_container_01d");
	static const Hash TrContainer01e = GET_HASH_KEY("tr_prop_tr_container_01e");
	static const Hash TrContainer01f = GET_HASH_KEY("tr_prop_tr_container_01f");
	static const Hash TrContainer01g = GET_HASH_KEY("tr_prop_tr_container_01g");
	static const Hash TrContainer01h = GET_HASH_KEY("tr_prop_tr_container_01h");
	static const Hash TrContainer01i = GET_HASH_KEY("tr_prop_tr_container_01i");
	static const Hash TrGrinder01a = GET_HASH_KEY("tr_prop_tr_grinder_01a");
	static const Hash TrLock01a = GET_HASH_KEY("tr_prop_tr_lock_01a");
	static const Hash TrContCollision = GET_HASH_KEY("tr_prop_tr_cont_coll_01a");

	static const std::vector<KnownBagVariant> vKnownBagVariants =
	{
		{ 5, 40, -1, BagPro2 },
		{ 5, 41, -1, BagPro2 },

		{ 5, 81, 0,  BagVar02 },
		{ 5, 81, 2,  BagVar22 },
		{ 5, 81, 4,  BagVar22_Green },
		{ 5, 81, 8,  BagVar04 },
		{ 5, 81, 9,  BagVar01 },
		{ 5, 81, 10, BagVar08 },
		{ 5, 81, 11, BagVar09 },
		{ 5, 81, 12, BagVar06 },
		{ 5, 81, 13, BagVar05 },
		{ 5, 81, 14, BagVar10 },
		{ 5, 81, 15, BagVar07 },

		{ 5, 82, 0,  BagVar02 },
		{ 5, 82, 2,  BagVar22 },
		{ 5, 82, 4,  BagVar22_Green },
		{ 5, 82, 8,  BagVar04 },
		{ 5, 82, 9,  BagVar01 },
		{ 5, 82, 10, BagVar08 },
		{ 5, 82, 11, BagVar09 },
		{ 5, 82, 12, BagVar06 },
		{ 5, 82, 13, BagVar05 },
		{ 5, 82, 14, BagVar10 },
		{ 5, 82, 15, BagVar07 },
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

	static void SpawnContainerPersistentProps(InteractableInstance& inst)
	{
		inst.ExtraEnts.resize(inst.Def->ExtraProps.size());

		for (size_t i = 0; i < inst.Def->ExtraProps.size(); i++)
		{
			const auto& extraDef = inst.Def->ExtraProps[i];
			if (!extraDef.SpawnAtPlacement)
				continue;

			SpoonerEntity extraEnt = EntityManagement::AddProp(extraDef.PropModel, extraDef.Label);
			if (extraEnt.handle.Exists())
			{
				extraEnt.handle.SetIsCollisionEnabled(false);
				if (extraDef.StartInvisible)
					extraEnt.handle.SetVisible(false);

				EntityManagement::AttachEntity(extraEnt, inst.Ent.handle, 0, extraDef.AttachOffset, extraDef.AttachRotation);
				EntityManagement::RemoveEntityFromDb(extraEnt);
			}
			inst.ExtraEnts[i] = extraEnt;
		}
	}

	std::vector<InteractableDefinition> vInteractables =
	{
		{
			"Grab Cash",
			CashStack,
			{},
			{
				{ "Duffel Bag", BagFallback, true }
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
			"Grab Weed",
			WeedStack,
			{},
			{
				{ "Duffel Bag", BagFallback, true }
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
			"Grab Gold",
			GoldStack,
			{},
			{
				{ "Duffel Bag", BagFallback, true }
			},
			"anim@scripted@player@mission@tun_table_grab@gold@",
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
						{ AnimTarget::PrimaryProp, "grab_gold" },
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
			"Grab Cocaine",
			CokeStack,
			{},
			{
				{ "Duffel Bag", BagFallback, true }
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
			ContainerClosed,
			{},
			{
				{
					"Opened Container",
					ContainerOpened,
					false,
					ExtraPropAttachTarget::PrimaryProp,
					true,
					false,              // SpawnAtPlacement
					AttachBone::Default,
					false               // DeleteOnComplete - false: must persist via FinalPropStates, not be deleted
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
{
	"Open Container (Angle Grinder)",
	TrContainer01a,
	{
		TrContainer01a, TrContainer01b, TrContainer01c, TrContainer01d, TrContainer01e,
		TrContainer01f, TrContainer01g, TrContainer01h, TrContainer01i
	},
	{
		{ "Duffel Bag", BagFallback, true, ExtraPropAttachTarget::Player, false, false, AttachBone::Default, true },

		{ "Angle Grinder", TrGrinder01a, false, ExtraPropAttachTarget::Player, false, false, AttachBone::LeftHand, true },

		{ "Lock", TrLock01a, false, ExtraPropAttachTarget::PrimaryProp, false, true, AttachBone::Default, false, true,
			Vector3(0.0f, -1.8130f, 1.2000f), Vector3() },

		{ "Container Collision", TrContCollision, false, ExtraPropAttachTarget::PrimaryProp, true, true, AttachBone::Default, false }
	},
	"anim@scripted@player@mission@tunf_train_ig1_container_p1@male@",
	{
		{
			{
				{ AnimTarget::Player,      "action" },
				{ AnimTarget::PrimaryProp, "action_container" },
				{ AnimTarget::ExtraProp,   "action_bag",           0 },
				{ AnimTarget::ExtraProp,   "action_angle_grinder", 1 },
				{ AnimTarget::ExtraProp,   "action_lock",          2 }
			},
			30000, 1.0f, "",
			1000.0f, -8.0f, 0, 0, 0.0f, 0, 0, 1000.0f
		}
	},
	BagOpenState::NoChange,
	false,
	{
		{ AnimTarget::PrimaryProp, 0, 255, false, true },
		{ AnimTarget::ExtraProp,   3, 0,   true,  false }
	},
	Vector3(0.0f, -2.35f, 1.0f), 0.0f,
	0.50f,
	&SpawnContainerPersistentProps
},
{
			"Open Container (Keypad)",
			Container_MW_Closed,
			{},
			{
				{
					"Opened Container",
					Container_MW_Opened,
					false,
					ExtraPropAttachTarget::PrimaryProp,
					true,
					false,
					AttachBone::Default,
					false
				}
			},
			"anim@scripted@freemode@ig1_open_container@male@",
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
		Hash modelToSpawn = def.PropModel;
		if (!def.PropModelVariants.empty())
		{
			int idx = rand() % (int)def.PropModelVariants.size();
			modelToSpawn = def.PropModelVariants[idx];
		}

		SpoonerEntity ent = EntityManagement::AddProp(modelToSpawn, def.Label);
		if (!ent.handle.Exists())
			return SpoonerEntity();

		InteractableInstance inst;
		inst.Ent = ent;
		inst.Def = &def;
		inst.AnchorPos = ent.handle.GetPosition();
		inst.AnchorRot = ent.handle.GetRotation();
		inst.ExtraRuntime.resize(def.ExtraProps.size());

		if (def.SpawnPersistentProps != nullptr)
		{
			def.SpawnPersistentProps(inst);
		}

		vActiveInteractables.push_back(inst);
		return ent;
	}

	static void StartStage(InteractableInstance& inst, size_t stageIndex)
	{
		const InteractableStage& stage = inst.Def->Stages[stageIndex];

		GTAped myPed = PLAYER_PED_ID();

		Vector3 standPos = GetInteractPosition(inst);
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
			return;
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

		if (inst.ExtraEnts.size() != inst.Def->ExtraProps.size())
		{
			inst.ExtraEnts.resize(inst.Def->ExtraProps.size());
			inst.ExtraRuntime.resize(inst.Def->ExtraProps.size());
		}

		for (size_t i = 0; i < inst.Def->ExtraProps.size(); i++)
		{
			const auto& extraDef = inst.Def->ExtraProps[i];
			if (extraDef.SpawnAtPlacement)
				continue; // already spawned in SpawnInteractable

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

				int defaultBone = (extraDef.AttachTarget == ExtraPropAttachTarget::PrimaryProp) ? 0 : rootBone;
				int attachBone = defaultBone;
				if (extraDef.AttachTarget == ExtraPropAttachTarget::Player)
				{
					if (extraDef.BoneOverride == AttachBone::LeftHand)
						attachBone = myPed.GetBoneIndex(Bone::LeftHand);
					else if (extraDef.BoneOverride == AttachBone::RightHand)
						attachBone = myPed.GetBoneIndex(Bone::RightHand);
				}

				EntityManagement::AttachEntity(extraEnt, attachTarget, attachBone, extraDef.AttachOffset, extraDef.AttachRotation);
				EntityManagement::RemoveEntityFromDb(extraEnt);
			}
			inst.ExtraEnts[i] = extraEnt;
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

							if (EntityManagement::GetEntityIndexInDb(inst.Ent.handle) < 0)
							{
								EntityManagement::AddEntityToDb(inst.Ent, true);
							}

							for (size_t i = 0; i < inst.Def->ExtraProps.size(); i++)
							{
								if (inst.Def->ExtraProps[i].DeleteOnComplete &&
									i < inst.ExtraEnts.size() && inst.ExtraEnts[i].handle.Exists())
								{
									EntityManagement::DeleteEntity(inst.ExtraEnts[i]);
								}
							}
						}

						inst.bCleanupDone = true;
					}
				}

				continue;
			}

			inst.AnchorPos = inst.Ent.handle.GetPosition();
			inst.AnchorRot = inst.Ent.handle.GetRotation();

			for (size_t i = 0; i < inst.Def->ExtraProps.size() && i < inst.ExtraEnts.size(); i++)
			{
				if (!inst.Def->ExtraProps[i].FreezeAtSpawn)
					continue;

				SpoonerEntity& extraEnt = inst.ExtraEnts[i];
				if (!extraEnt.handle.Exists())
					continue;

				ExtraPropRuntime& runtime = inst.ExtraRuntime[i];
				if (!runtime.Captured)
				{
					runtime.FrozenPos = extraEnt.handle.GetPosition();
					runtime.FrozenRot = extraEnt.handle.GetRotation();
					runtime.Captured = true;
				}
				else
				{
					extraEnt.handle.SetPosition(runtime.FrozenPos);
					extraEnt.handle.SetRotation(runtime.FrozenRot);
				}
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

		vActiveInteractables.erase(
			std::remove_if(vActiveInteractables.begin(), vActiveInteractables.end(),
				[](const InteractableInstance& inst) { return !inst.Ent.handle.Exists(); }),
			vActiveInteractables.end()
		);
	}
}
}