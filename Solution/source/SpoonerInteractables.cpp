#include "SpoonerInteractables.h"
#include "Menu/Menu.h"
#include "Scripting/Game.h"
#include "Scripting/Model.h"
#include "Submenus/Spooner/EntityManagement.h"
#include "Scripting/GTAped.h"
#include "Scripting/GTAentity.h"
#include "Submenus/Spooner/Submenus.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <Scripting/CustomHelpText.h>

namespace sub::Spooner::Interactables
{
		// ========================================================================
		//  1. MODEL HASHES
		// ========================================================================
	static const Hash CashStack = GET_HASH_KEY("h4_prop_h4_cash_stack_01a");
	static const Hash WeedStack = GET_HASH_KEY("h4_prop_h4_weed_stack_01a");
	static const Hash GoldStack = GET_HASH_KEY("h4_prop_h4_gold_stack_01a");
	static const Hash CokeStack = GET_HASH_KEY("h4_prop_h4_coke_stack_01a");
	static const Hash BookStack = GET_HASH_KEY("m24_2_prop_m42_book_stack_01a");
	static const Hash ServerStack = GET_HASH_KEY("m24_2_prop_m42_server_stack_01a");
	static const Hash HarddriveStack = GET_HASH_KEY("m24_2_prop_m42_harddrive_stack_01a");
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

	static const Hash SmallContainer = GET_HASH_KEY("m25_1_prop_m51_containersmall_01b");
	static const Hash ChainLock = GET_HASH_KEY("h4_prop_h4_chain_lock_01a");
	static const Hash BoltCutters = GET_HASH_KEY("h4_prop_h4_bolt_cutter_01a");

	static const Hash ElectricBox1 = GET_HASH_KEY("h4_prop_h4_elecbox_01a");
	static const Hash ElectricBox2 = GET_HASH_KEY("m24_2_prop_m42_elecbox_01a");

	static const Hash Lever1 = GET_HASH_KEY("m23_1_prop_m31_lever_box_01a");
	static const Hash Lever2 = GET_HASH_KEY("h4_prop_h4_lever_box_01a");
	static const Hash Lever3 = GET_HASH_KEY("m23_2_prop_m32_lever_box_01a");

	static const Hash DisplayCase1 = GET_HASH_KEY("h4_prop_h4_glass_disp_01a");
	static const Hash DisplayCase2 = GET_HASH_KEY("h4_prop_h4_glass_disp_01b");
	static const Hash DisplayCase3 = GET_HASH_KEY("h4_prop_h4_glass_cut_01a");
	static const Hash PlasmaCutter = GET_HASH_KEY("h4_prop_h4_cutter_01a");

	static const Hash PericoTarget1A = GET_HASH_KEY("h4_prop_h4_diamond_01a");
	static const Hash PericoTarget1B = GET_HASH_KEY("h4_prop_h4_diamond_disp_01a");
	static const Hash PericoTarget2A = GET_HASH_KEY("h4_prop_h4_necklace_01a");
	static const Hash PericoTarget2B = GET_HASH_KEY("h4_prop_h4_neck_disp_01a");
	static const Hash PericoTarget3 = GET_HASH_KEY("h4_prop_h4_t_bottle_01a");
	static const Hash PericoTarget4 = GET_HASH_KEY("h4_prop_h4_t_bottle_02a");
	static const Hash PericoTarget5 = GET_HASH_KEY("h4_prop_h4_t_bottle_02b");
	static const Hash PericoTarget6 = GET_HASH_KEY("h4_prop_h4_art_pant_01a");

	static const Hash Laptop = GET_HASH_KEY("prop_laptop_01a");
	static const Hash Guitar = GET_HASH_KEY("prop_acc_guitar_01");
	static const Hash GuitarStand = GET_HASH_KEY("sf_prop_sf_acc_stand_01a");

	static const Hash FuseBox = GET_HASH_KEY("reh_prop_reh_b_computer_04a");
	static const Hash FuseBoxOpen = GET_HASH_KEY("reh_prop_reh_b_computer_04b");
	static const Hash FuseFresh = GET_HASH_KEY("reh_prop_reh_fuse_01a");
	static const Hash FuseBlown = GET_HASH_KEY("reh_prop_reh_fuse_01b");

	static const Hash PowerBox = GET_HASH_KEY("xm_prop_x17_powerbox_01");

	static const Hash ClipboardSchedule = GET_HASH_KEY("tr_prop_tr_clipboard_sh_01a");
	static const Hash ClipboardSchedule2 = GET_HASH_KEY("tr_prop_tr_clipboard_sh_01a");

	static const Hash ServerCleanStack = GET_HASH_KEY("tr_prop_tr_corp_servercln_01a");
	static const Hash ServerDrive = GET_HASH_KEY("m24_2_prop_m42_serverdrive_01a");

	static const Hash PaintingCase = GET_HASH_KEY("ch_prop_ch_sec_cabinet_02a");
	static const Hash PaintingDiamondCasino01a = GET_HASH_KEY("ch_prop_vault_painting_01a");
	static const Hash PaintingDiamondCasino01b = GET_HASH_KEY("ch_prop_vault_painting_01b");
	static const Hash PaintingDiamondCasino01c = GET_HASH_KEY("ch_prop_vault_painting_01c");
	static const Hash PaintingDiamondCasino01d = GET_HASH_KEY("ch_prop_vault_painting_01d");
	static const Hash PaintingDiamondCasino01e = GET_HASH_KEY("ch_prop_vault_painting_01e");
	static const Hash PaintingDiamondCasino01f = GET_HASH_KEY("ch_prop_vault_painting_01f");
	static const Hash PaintingDiamondCasino01g = GET_HASH_KEY("ch_prop_vault_painting_01g");
	static const Hash PaintingDiamondCasino01h = GET_HASH_KEY("ch_prop_vault_painting_01h");
	static const Hash PaintingDiamondCasino01i = GET_HASH_KEY("ch_prop_vault_painting_01i");
	static const Hash PaintingCayoPerico01a = GET_HASH_KEY("h4_prop_h4_painting_01a");
	static const Hash PaintingCayoPerico01b = GET_HASH_KEY("h4_prop_h4_painting_01b");
	static const Hash PaintingCayoPerico01c = GET_HASH_KEY("h4_prop_h4_painting_01c");
	static const Hash PaintingCayoPerico01d = GET_HASH_KEY("h4_prop_h4_painting_01d");
	static const Hash PaintingCayoPerico01e = GET_HASH_KEY("h4_prop_h4_painting_01e");
	static const Hash PaintingCayoPerico01f = GET_HASH_KEY("h4_prop_h4_painting_01f");
	static const Hash PaintingCayoPerico01g = GET_HASH_KEY("h4_prop_h4_painting_01g");
	static const Hash PaintingCayoPerico01h = GET_HASH_KEY("h4_prop_h4_painting_01h");
	static const Hash PaintingBottomDollar01a = GET_HASH_KEY("m24_1_prop_m41_painting_01a");
	static const Hash PaintingBottomDollar01b = GET_HASH_KEY("m24_1_prop_m41_painting_01b");
	static const Hash PaintingBottomDollar01c = GET_HASH_KEY("m24_1_prop_m41_painting_01c");
	static const Hash PaintingBottomDollar01d = GET_HASH_KEY("m24_1_prop_m41_painting_01d");

	static const Hash TrolleyCash = GET_HASH_KEY("hei_prop_hei_cash_trolly_01");
	static const Hash TrolleyCashDiamondCasino = GET_HASH_KEY("ch_prop_ch_cash_trolly_01b");
	static const Hash TrolleyDiamond = GET_HASH_KEY("ch_prop_diamond_trolly_01b");
	static const Hash TrolleyGold = GET_HASH_KEY("ch_prop_gold_trolly_01c");
	static const Hash TrolleyGoldDiamondCasino = GET_HASH_KEY("ch_prop_gold_trolly_01b");

	static const Hash JewelCabinet = GET_HASH_KEY("p_new_j_counter_02");
	static const Hash JewelCabGlass = GET_HASH_KEY("p_counter_02_glass");
	static const Hash JewelCabStart1 = GET_HASH_KEY("des_jewel_cab_start");
	static const Hash JewelCabStart2 = GET_HASH_KEY("des_jewel_cab2_start");
	static const Hash JewelCabStart3 = GET_HASH_KEY("des_jewel_cab3_start");
	static const Hash JewelCabStart4 = GET_HASH_KEY("des_jewel_cab4_start");
	static const Hash JewelCabEnd1 = GET_HASH_KEY("des_jewel_cab_end");
	static const Hash JewelCabEnd2 = GET_HASH_KEY("des_jewel_cab2_end");
	static const Hash JewelCabEnd3 = GET_HASH_KEY("des_jewel_cab3_end");
	static const Hash JewelCabEnd4 = GET_HASH_KEY("des_jewel_cab4_end");
	static const Hash JewelCabRoot1 = GET_HASH_KEY("des_jewel_cab_root");
	static const Hash JewelCabRootB1 = GET_HASH_KEY("des_jewel_cab_root2");
	static const Hash JewelCabRoot2 = GET_HASH_KEY("des_jewel_cab2_root");
	static const Hash JewelCabRootB2 = GET_HASH_KEY("des_jewel_cab2_rootb");
	static const Hash JewelCabRoot3 = GET_HASH_KEY("des_jewel_cab3_root");
	static const Hash JewelCabRootB3 = GET_HASH_KEY("des_jewel_cab3_rootb");
	static const Hash JewelCabRoot4 = GET_HASH_KEY("des_jewel_cab4_root");
	static const Hash JewelCabRootB4 = GET_HASH_KEY("des_jewel_cab4_rootb");

	static const Hash VendingMachineEcola = GET_HASH_KEY("prop_vend_soda_01");
	static const Hash VendingMachineSprunk = GET_HASH_KEY("prop_vend_soda_02");
	static const Hash VendingMachineJunk = GET_HASH_KEY("sf_prop_sf_vend_drink_01a");
	static const Hash SodaCanEcola = GET_HASH_KEY("prop_ecola_can");
	static const Hash SodaCanSprunk = GET_HASH_KEY("prop_ld_can_01b");
	static const Hash SodaCanJunk = GET_HASH_KEY("sf_prop_sf_can_01a");

	// ========================================================================
	//  2. WORN-BAG MATCHING (slot 9 = Story Mode TASK, slot 5 = Online HAND)
	// ========================================================================
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

	static const std::vector<BagDrawablePair> vBagDrawablePairs =
	{
		{ 9, 1,  2 },
		{ 9, 12, 11 },
		{ 5, 41, 40 },
		{ 5, 45, 44 },
		{ 5, 81, 82 },
		{ 5, 86, 85 },
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

	// ========================================================================
	//  3. SHARED HELPERS
	// ========================================================================
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

	static Vector3 GetSceneOrigin(const InteractableInstance& inst)
	{
		const Vector3& o = inst.Def->SceneOriginOffset;
		const float headingRad = -inst.AnchorRot.z * 3.14159f / 180.0f;
		const Vector3 forward = Vector3(sinf(headingRad), cosf(headingRad), 0.0f);
		const Vector3 right = Vector3(cosf(headingRad), -sinf(headingRad), 0.0f);

		return inst.AnchorPos + (right * o.x) + (forward * o.y) + Vector3(0.0f, 0.0f, o.z);
	}

	static const char* ParticipantDict(const InteractableInstance& inst, const SceneParticipant& p)
	{
		return p.AnimDict.empty() ? inst.Def->AnimDict.c_str() : p.AnimDict.c_str();
	}

	static void RequireAnimDict(InteractableInstance& inst, const std::string& dict)
	{
		if (dict.empty() || std::find(inst.ExtraAnimDicts.begin(), inst.ExtraAnimDicts.end(), dict) != inst.ExtraAnimDicts.end())
			return;

		inst.ExtraAnimDicts.push_back(dict);
		STREAMING::REQUEST_ANIM_DICT(dict.c_str());
	}

	static std::vector<std::string> CollectAnimDicts(const InteractableInstance& inst)
	{
		std::vector<std::string> dicts;
		auto add = [&dicts](const std::string& d)
			{
				if (!d.empty() && std::find(dicts.begin(), dicts.end(), d) == dicts.end())
					dicts.push_back(d);
			};

		add(inst.Def->AnimDict);
		for (auto& st : inst.Def->Stages)
			for (auto& p : st.Participants)
				add(p.AnimDict);
		for (auto& d : inst.ExtraAnimDicts)
			add(d);
		return dicts;
	}

	static bool LoadAnimDicts(const std::vector<std::string>& dicts)
	{
		for (auto& d : dicts)
			STREAMING::REQUEST_ANIM_DICT(d.c_str());

		for (int waitCount = 0; waitCount < 500; waitCount++)
		{
			bool allLoaded = true;
			for (auto& d : dicts)
			{
				if (!STREAMING::HAS_ANIM_DICT_LOADED(d.c_str()))
				{
					allLoaded = false;
					break;
				}
			}
			if (allLoaded)
				return true;
			WAIT(0);
		}
		return false;
	}

	static Hash PickExtraModel(const InteractableInstance& inst, size_t index)
	{
		if (index < inst.ExtraModelOverride.size() && inst.ExtraModelOverride[index] != 0)
			return inst.ExtraModelOverride[index];

		const ExtraPropDef& def = inst.Def->ExtraProps[index];
		if (!def.PropModelVariants.empty())
			return def.PropModelVariants[rand() % (int)def.PropModelVariants.size()];
		return def.PropModel;
	}

	static void ApplyExtraDynamic(SpoonerEntity& extra, const ExtraPropDef& def)
	{
		if (!def.Dynamic || !extra.handle.Exists())
			return;

		extra.handle.SetDynamic(true);
		extra.handle.FreezePosition(false);
		extra.dynamic = true;
	}

	static void SpawnContainerPersistentProps(InteractableInstance& inst)
	{
		inst.ExtraEnts.resize(inst.Def->ExtraProps.size());

		for (size_t i = 0; i < inst.Def->ExtraProps.size(); i++)
		{
			const auto& extraDef = inst.Def->ExtraProps[i];
			if (!extraDef.SpawnAtPlacement)
				continue;

			SpoonerEntity extraEnt = EntityManagement::AddProp(PickExtraModel(inst, i), extraDef.Label);
			if (extraEnt.handle.Exists())
			{
				extraEnt.handle.SetIsCollisionEnabled(false);
				if (extraDef.StartInvisible)
					extraEnt.handle.SetVisible(false);

				EntityManagement::AttachEntity(extraEnt, inst.Ent.handle, extraDef.PrimaryAttachBone, extraDef.AttachOffset, extraDef.AttachRotation);
				EntityManagement::RemoveEntityFromDb(extraEnt);
				ApplyExtraDynamic(extraEnt, extraDef);
			}
			inst.ExtraEnts[i] = extraEnt;
		}
	}

	static int CreateScene(const InteractableInstance& inst, const InteractableStage& stage, bool holdLastFrame, bool applySceneOffset = true)
	{
		const Vector3 origin = applySceneOffset ? GetSceneOrigin(inst) : inst.AnchorPos;
		const float yaw = inst.AnchorRot.z + (applySceneOffset ? inst.Def->SceneHeadingOffset : 0.0f);

		int scene = CREATE_SYNCHRONIZED_SCENE(origin.x, origin.y, origin.z,
			inst.AnchorRot.x, inst.AnchorRot.y, yaw, 2);
		if (scene == -1)
			return -1;

		SET_SYNCHRONIZED_SCENE_HOLD_LAST_FRAME(scene, holdLastFrame);
		SET_SYNCHRONIZED_SCENE_RATE(scene, stage.SceneRate);

		if (stage.LoopUntilInteract)
			SET_SYNCHRONIZED_SCENE_LOOPED(scene, true);

		return scene;
	}

	static void PlayPropParticipant(InteractableInstance& inst, const SceneParticipant& p, int syncedScene, const InteractableStage& stage)
	{
		if (p.Target == AnimTarget::ExtraProp && (size_t)p.ExtraPropIndex >= inst.ExtraEnts.size())
			return;

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
			p.ClipName.c_str(), ParticipantDict(inst, p),
			stage.BlendIn, stage.BlendOut, stage.PropFlags, stage.PlaybackRate
		);
	}

	static void PlayWeaponParticipant(InteractableInstance& inst, const SceneParticipant& p, int syncedScene, const InteractableStage& stage)
	{
		GTAentity weapon = GET_CURRENT_PED_WEAPON_ENTITY_INDEX(PLAYER_PED_ID(), 0);
		if (!weapon.Exists())
			return;

		ENTITY::PLAY_SYNCHRONIZED_ENTITY_ANIM(
			weapon.Handle(), syncedScene,
			p.ClipName.c_str(), ParticipantDict(inst, p),
			stage.BlendIn, stage.BlendOut, stage.PropFlags, stage.PlaybackRate
		);
	}

	static int PlaySideScene(InteractableInstance& inst, const std::vector<SceneParticipant>& participants, bool applySceneOffset = false)
	{
		const InteractableStage& stage = inst.Def->Stages[inst.CurrentStage];

		const int scene = CreateScene(inst, stage, true, applySceneOffset);
		if (scene == -1)
			return -1;

		for (auto& p : participants)
			PlayPropParticipant(inst, p, scene, stage);

		return scene;
	}

	// ========================================================================
	//  4A. DISPLAY CASE HOOKS
	//      Extra slots: 0 Bag, 1 Cutter, 2 Display Case (Opened), 3 Glass Piece, 4 Treasure (Display), 5 Treasure (Item)
	//      Stages:      0 enter, 1 cutting_loop, 2-4 overheat 1-3, 5 success
	// ========================================================================
	struct TreasureOption { Hash Outer; Hash Inner; };

	static const std::vector<TreasureOption> vTreasureOptions =
	{
		{ PericoTarget1B, PericoTarget1A },
		{ PericoTarget2B, PericoTarget2A },
		{ PericoTarget3,  0 },
		{ PericoTarget4,  0 },
		{ PericoTarget5,  0 },
		{ PericoTarget6,  0 },
	};

	static void SpawnDisplayCasePersistentProps(InteractableInstance& inst)
	{
		inst.ExtraEnts.resize(inst.Def->ExtraProps.size());
		inst.ExtraRuntime.resize(inst.Def->ExtraProps.size());

		{
			const auto& def2 = inst.Def->ExtraProps[2];
			SpoonerEntity e = EntityManagement::AddProp(def2.PropModel, def2.Label);
			if (e.handle.Exists())
			{
				e.handle.SetIsCollisionEnabled(false);
				e.handle.SetVisible(false);
				EntityManagement::AttachEntity(e, inst.Ent.handle, 0, def2.AttachOffset, def2.AttachRotation);
				EntityManagement::RemoveEntityFromDb(e);
			}
			inst.ExtraEnts[2] = e;
		}

		int pick = rand() % (int)vTreasureOptions.size();
		const auto& option = vTreasureOptions[pick];
		Vector3 offset(0.0f, 0.0f, 1.0300f);

		SpoonerEntity outerEnt = EntityManagement::AddProp(option.Outer, "Treasure (Display)");
		if (outerEnt.handle.Exists())
		{
			outerEnt.handle.SetIsCollisionEnabled(false);
			EntityManagement::AttachEntity(outerEnt, inst.Ent.handle, 0, offset, Vector3());
			EntityManagement::RemoveEntityFromDb(outerEnt);
		}
		inst.ExtraEnts[4] = outerEnt;

		if (option.Inner != 0 && outerEnt.handle.Exists())
		{
			SpoonerEntity innerEnt = EntityManagement::AddProp(option.Inner, "Treasure (Item)");
			if (innerEnt.handle.Exists())
			{
				innerEnt.handle.SetIsCollisionEnabled(false);
				EntityManagement::AttachEntity(innerEnt, outerEnt.handle, 0, Vector3(), Vector3());
				EntityManagement::RemoveEntityFromDb(innerEnt);
			}
			inst.ExtraEnts[5] = innerEnt;
		}
	}

	static void OnStageStart_DisplayCase(InteractableInstance& inst, size_t stageIndex)
	{
		if (stageIndex != 5) return;

		inst.Ent.handle.SetAlpha(0);

		if (inst.ExtraEnts[2].handle.Exists())
		{
			inst.ExtraEnts[2].handle.SetAlpha(255);
			inst.ExtraEnts[2].handle.SetVisible(true);
		}

		const auto& def3 = inst.Def->ExtraProps[3];
		SpoonerEntity e3 = EntityManagement::AddProp(def3.PropModel, def3.Label);
		if (e3.handle.Exists())
		{
			e3.handle.SetIsCollisionEnabled(false);
			GTAped myPed = PLAYER_PED_ID();
			int wristBone = myPed.GetBoneIndex(Bone::RightWrist);
			EntityManagement::AttachEntity(e3, myPed, wristBone, def3.AttachOffset, def3.AttachRotation);
			EntityManagement::RemoveEntityFromDb(e3);
		}
		inst.ExtraEnts[3] = e3;
	}

	static void TryPickupDisplayCaseTreasure(InteractableInstance& inst)
	{
		if (inst.bTreasurePickedUp)
			return;

		bool isDuo = inst.ExtraEnts.size() > 5 && inst.ExtraEnts[5].handle.Exists();
		int itemIndex = isDuo ? 5 : 4;

		if ((size_t)itemIndex >= inst.ExtraEnts.size() || !inst.ExtraEnts[itemIndex].handle.Exists())
			return;

		GTAped myPed = PLAYER_PED_ID();
		int wristBone = myPed.GetBoneIndex(Bone::LeftWrist);

		Vector3 wristPos = myPed.GetBoneCoord(wristBone);
		Vector3 treasurePos = inst.ExtraEnts[itemIndex].handle.GetPosition();

		float distance = GET_DISTANCE_BETWEEN_COORDS(
			wristPos.x, wristPos.y, wristPos.z,
			treasurePos.x, treasurePos.y, treasurePos.z,
			true
		);

		if (distance > 0.86f)
			return;

		SpoonerEntity& treasure = inst.ExtraEnts[itemIndex];

		EntityManagement::DetachEntity(treasure);
		treasure.handle.FreezePosition(false);
		EntityManagement::AttachEntity(treasure, myPed, wristBone, Vector3(), Vector3());
		EntityManagement::RemoveEntityFromDb(treasure);

		inst.bTreasurePickedUp = true;
	}

	static void OnStageTick_DisplayCase(InteractableInstance& inst, size_t stageIndex)
	{
		if (stageIndex == 5)
			TryPickupDisplayCaseTreasure(inst);
	}

	static size_t GetNextStage_DisplayCase(InteractableInstance& inst, size_t currentStage)
	{
		if (currentStage == 0)
			return 1;

		if (currentStage == 1)
		{
			if (inst.OverheatCount < 3)
			{
				int roll = rand() % 100;
				if (roll < 20)
				{
					size_t overheatStage = 2 + inst.OverheatCount;
					inst.OverheatCount++;
					return overheatStage;
				}
			}

			return 5;
		}

		if (currentStage >= 2 && currentStage <= 4)
			return 1;

		return (size_t)inst.Def->Stages.size();
	}

	static void CustomCleanup_DisplayCase(InteractableInstance& inst)
	{
		if (inst.ExtraEnts.size() > 5 && inst.ExtraEnts[5].handle.Exists())
		{
			EntityManagement::DeleteEntity(inst.ExtraEnts[5]);
		}
		else if (inst.ExtraEnts.size() > 4 && inst.ExtraEnts[4].handle.Exists())
		{
			EntityManagement::DeleteEntity(inst.ExtraEnts[4]);
		}
	}

	// ========================================================================
	//  4B. GUITAR HOOKS
	// ========================================================================
	static void OnStageStart_Guitar(InteractableInstance& inst, size_t stageIndex)
	{
		if (stageIndex != 0) return;

		inst.Ent.handle.SetAlpha(0);
		inst.Ent.handle.SetIsCollisionEnabled(false);

		const auto& heldDef = inst.Def->ExtraProps[0];
		SpoonerEntity held = EntityManagement::AddProp(heldDef.PropModel, heldDef.Label);
		if (held.handle.Exists())
		{
			held.handle.SetIsCollisionEnabled(false);
			GTAped myPed = PLAYER_PED_ID();
			int bone = ResolveAttachBoneIndex(myPed, heldDef.BoneOverride, myPed.GetBoneIndex(Bone::SKEL_ROOT));
			EntityManagement::AttachEntity(held, myPed, bone, heldDef.AttachOffset, heldDef.AttachRotation);
			EntityManagement::RemoveEntityFromDb(held);
		}
		inst.ExtraEnts[0] = held;
	}

	static void OnStageStart_Clipboard(InteractableInstance& inst, size_t stageIndex)
	{
		if (stageIndex != 0)
			return;

		inst.Ent.handle.SetAlpha(0);
		inst.Ent.handle.SetIsCollisionEnabled(false);

		const auto& clipboardDef = inst.Def->ExtraProps[0];

		SpoonerEntity clipboard = EntityManagement::AddProp(
			clipboardDef.PropModel,
			clipboardDef.Label
		);

		if (clipboard.handle.Exists())
		{
			clipboard.handle.SetIsCollisionEnabled(false);

			GTAped myPed = PLAYER_PED_ID();
			int wristBone = myPed.GetBoneIndex(Bone::LeftWrist);

			EntityManagement::AttachEntity(
				clipboard,
				myPed,
				wristBone,
				clipboardDef.AttachOffset,
				clipboardDef.AttachRotation
			);

			EntityManagement::RemoveEntityFromDb(clipboard);
		}

		inst.ExtraEnts[0] = clipboard;
	}

	// ========================================================================
//  4C. FIX BREAKER
//      Extra prop indices: 0-3 = blown fuse in slot 1-4, 4-7 = fresh fuse in slot 1-4.
//      Stage indices:      0 = open the box, 1-4 = replace the fuse in that slot, 5 = success.
//      Flow: the first interaction plays stage 0 and ends. The box is then left alone in its held scene
//      (same as the Angle Grinder container) until the next interaction, which resumes at ResumeStage
//      and runs through to success.
// ========================================================================
	static const char* const BreakerAnimDict = "anim@scripted@ulp_missions@fuse@male@";
	static const size_t StageOpen = 0;
	static const size_t StageSuccess = 5;
	static const int BreakerStageTimeoutMs = 20000;
	static const DWORD BreakerSettleMs = 1500;

	static int BlownExtraIndex(int slot) { return slot - 1; }
	static int FreshExtraIndex(int slot) { return 4 + (slot - 1); }

	struct FusePose { int ExtraIndex; std::string Clip; };

	static std::string FuseReplaceClip(int step, bool broken, int slot)
	{
		if (step == 1 && broken && slot == 4)
			return "replace_fuse_01_broken_fuse_4";

		return "replace_fuse_0" + std::to_string(step) + (broken ? "_broken_fuse_" : "_fresh_fuse_") + std::to_string(slot);
	}

	static std::vector<FusePose> GetBreakerFusePoses(const InteractableInstance& inst, size_t stage)
	{
		std::vector<FusePose> poses;
		const int step = (int)stage;

		for (int slot = 1; slot <= 4; slot++)
		{
			const bool blown = (inst.FuseBlownMask & (1 << (slot - 1))) != 0;
			const std::string s = std::to_string(slot);

			if (stage == StageOpen)
			{
				if (blown) poses.push_back({ BlownExtraIndex(slot), "enter_broken_fuse_" + s });
				else       poses.push_back({ FreshExtraIndex(slot), "success_fresh_fuse_" + s });
			}
			else if (stage == StageSuccess)
			{
				poses.push_back({ FreshExtraIndex(slot), "success_fresh_fuse_" + s });
				if (blown)
					poses.push_back({ BlownExtraIndex(slot), "" });
			}
			else if (slot < step)
			{
				poses.push_back({ FreshExtraIndex(slot), FuseReplaceClip(step, false, slot) });
				if (blown)
					poses.push_back({ BlownExtraIndex(slot), "" });
			}
			else if (slot == step)
			{
				poses.push_back({ BlownExtraIndex(slot), FuseReplaceClip(step, true,  slot) });
				poses.push_back({ FreshExtraIndex(slot), FuseReplaceClip(step, false, slot) });
			}
			else if (blown)
			{
				if (step == 2 && slot == 4)
					poses.push_back({ BlownExtraIndex(slot), "" });
				else
					poses.push_back({ BlownExtraIndex(slot), FuseReplaceClip(step, true, slot) });
			}
			else
			{
				poses.push_back({ FreshExtraIndex(slot), "success_fresh_fuse_" + s });
			}
		}
		return poses;
	}

	static void ApplyBreakerFuseVisibility(InteractableInstance& inst, const std::vector<FusePose>& poses)
	{
		for (size_t i = 0; i < inst.ExtraEnts.size(); i++)
		{
			if (!inst.ExtraEnts[i].handle.Exists())
				continue;

			bool wanted = std::any_of(poses.begin(), poses.end(),
				[i](const FusePose& p) { return p.ExtraIndex == (int)i; });
			inst.ExtraEnts[i].handle.SetVisible(wanted);
		}
	}

	static size_t NextBlownStage(int blownMask, int afterSlot)
	{
		for (int slot = afterSlot + 1; slot <= 4; slot++)
		{
			if (blownMask & (1 << (slot - 1)))
				return (size_t)slot;
		}
		return StageSuccess;
	}

	static std::vector<ExtraPropDef> MakeBreakerFuseExtras()
	{
		std::vector<ExtraPropDef> extras;
		for (int i = 0; i < 8; i++)
		{
			const bool blown = i < 4;
			const std::string label = std::string(blown ? "Blown Fuse " : "Fresh Fuse ") + std::to_string((i % 4) + 1);
			extras.push_back(ExtraProp(label.c_str(), blown ? FuseBlown : FuseFresh)
				.AttachToPrimaryProp()
				.StartsHidden()
				.SpawnsAtPlacement()
				.KeepWhenDone()
				.Build());
		}
		return extras;
	}

	static std::vector<InteractableStage> MakeBreakerStages()
	{
		auto makeStage = [](const char* playerClip, const char* boxClip)
			{
				return Stage(BreakerStageTimeoutMs)
					.PlayerClip(playerClip)
					.PrimaryPropClip(boxClip)
					.PauseOnEnd()
					.Build();
			};

		std::vector<InteractableStage> stages;
		stages.push_back(makeStage("enter", "enter_fusebox"));
		stages.push_back(makeStage("replace_fuse_01", "replace_fuse_01_fusebox"));
		stages.push_back(makeStage("replace_fuse_02", "replace_fuse_02_fusebox"));
		stages.push_back(makeStage("replace_fuse_03", "replace_fuse_03_fusebox"));
		stages.push_back(makeStage("replace_fuse_04", "replace_fuse_04_fusebox"));
		stages.push_back(makeStage("success", "success_fusebox"));
		return stages;
	}

	static void ResolveParticipants_Breaker(InteractableInstance& inst, size_t stageIndex, std::vector<SceneParticipant>& participants)
	{
		for (auto& pose : GetBreakerFusePoses(inst, stageIndex))
		{
			if (pose.Clip.empty())
				continue;
			participants.push_back({ AnimTarget::ExtraProp, pose.Clip, pose.ExtraIndex });
		}
	}

	static void OnStageStart_Breaker(InteractableInstance& inst, size_t stageIndex)
	{
		ApplyBreakerFuseVisibility(inst, GetBreakerFusePoses(inst, stageIndex));
	}

	static size_t GetNextStage_Breaker(InteractableInstance& inst, size_t currentStage)
	{
		if (currentStage == StageOpen)
		{
			inst.bAnchorLocked = true;
			inst.ResumeStage = NextBlownStage(inst.FuseBlownMask, 0);
			return inst.Def->Stages.size();
		}

		if (currentStage >= StageSuccess)
			return inst.Def->Stages.size();

		return NextBlownStage(inst.FuseBlownMask, (int)currentStage);
	}

	static void CustomCleanup_Breaker(InteractableInstance& inst)
	{
		if (inst.CurrentStage == StageSuccess)
			inst.bCleanupDone = true;
	}

	static bool PinBreakerOpen(InteractableInstance& inst)
	{
		const InteractableStage& stage = inst.Def->Stages[StageOpen];
		const char* dict = inst.Def->AnimDict.c_str();

		STREAMING::REQUEST_ANIM_DICT(dict);
		int waitCount = 0;
		while (!STREAMING::HAS_ANIM_DICT_LOADED(dict) && waitCount++ < 500)
			WAIT(0);

		if (!STREAMING::HAS_ANIM_DICT_LOADED(dict))
			return false;

		int scene = CreateScene(inst, stage, true);
		if (scene == -1)
			return false;

		std::vector<FusePose> poses = GetBreakerFusePoses(inst, StageOpen);
		ApplyBreakerFuseVisibility(inst, poses);

		PlayPropParticipant(inst, { AnimTarget::PrimaryProp, "enter_fusebox" }, scene, stage);
		for (auto& pose : poses)
		{
			if (pose.Clip.empty())
				continue;
			PlayPropParticipant(inst, { AnimTarget::ExtraProp, pose.Clip, pose.ExtraIndex }, scene, stage);
		}

		inst.Ent.handle.SetIsCollisionEnabled(true);
		inst.bAnchorLocked = true;
		return true;
	}

	static void OnIdleTick_Breaker(InteractableInstance& inst)
	{
		if (inst.ResumeStage == 0 || inst.bAnchorLocked)
			return;

		const Vector3 pos = inst.Ent.handle.GetPosition();
		const Vector3 rot = inst.Ent.handle.GetRotation();
		const bool moved =
			fabsf(pos.x - inst.AnchorPos.x) > 0.005f || fabsf(pos.y - inst.AnchorPos.y) > 0.005f || fabsf(pos.z - inst.AnchorPos.z) > 0.005f ||
			fabsf(rot.x - inst.AnchorRot.x) > 0.1f || fabsf(rot.y - inst.AnchorRot.y) > 0.1f || fabsf(rot.z - inst.AnchorRot.z) > 0.1f;

		const DWORD now = GET_GAME_TIMER();
		if (moved || inst.BreakerSettleStart == 0)
		{
			inst.BreakerSettleStart = now;
			return;
		}

		if (now - inst.BreakerSettleStart < BreakerSettleMs)
			return;

		if (!PinBreakerOpen(inst))
			inst.BreakerSettleStart = now;
	}

	static void SpawnBreaker(InteractableInstance& inst, int blownMask, bool preOpened)
	{
		SpawnContainerPersistentProps(inst);
		inst.FuseBlownMask = blownMask;
		inst.ResumeStage = preOpened ? NextBlownStage(blownMask, 0) : 0;
	}

	static void SpawnBreaker_PreOpened(InteractableInstance& inst) { SpawnBreaker(inst, 0xF, true); }
	static void SpawnBreaker_AllBlown(InteractableInstance& inst) { SpawnBreaker(inst, 0xF, false); }

	// ========================================================================
//  4D. JEWEL CABINET HOOKS
//      Extra slots: 0 Extra Glass, 1 Start, 2 End, 3 Root, 4 Root B
//      Flow: Start is deleted at the smash; Root + Root B appear and play their clips; End takes over when finished.
// ========================================================================
	struct JewelCabVariant
	{
		Hash Start, End, Root, RootB;
		const char* Dict;
		const char* RootClip;
		const char* RootBClip;
	};

	static const std::vector<JewelCabVariant> vJewelCabVariants =
	{
		{ JewelCabStart1, JewelCabEnd1, JewelCabRoot1, JewelCabRootB1, "des_jewel_cab",  "des_jewel_cab_root",  "des_jewel_cab_root2" },
		{ JewelCabStart2, JewelCabEnd2, JewelCabRoot2, JewelCabRootB2, "des_jewel_cab2", "des_jewel_cab2_root", "des_jewel_cab2_rootb" },
		{ JewelCabStart3, JewelCabEnd3, JewelCabRoot3, JewelCabRootB3, "des_jewel_cab3", "des_jewel_cab3_root", "des_jewel_cab3_rootb" },
	};

	static const size_t JewelSlotStart = 1;
	static const size_t JewelSlotEnd = 2;
	static const size_t JewelSlotRoot = 3;
	static const size_t JewelSlotRootB = 4;
	static const float JewelSmashPhase = 0.1f;

	static const int JewelAttachBone = -1;

	static void AttachToPrimaryExactly(InteractableInstance& inst, SpoonerEntity& extra)
	{
		EntityManagement::AttachEntity(extra, inst.Ent.handle, JewelAttachBone, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
		EntityManagement::RemoveEntityFromDb(extra);
	}

	static void SpawnJewelCabinetGroup(InteractableInstance& inst)
	{
		if (inst.Def->ExtraProps.size() <= JewelSlotRootB)
			return;

		inst.VariantIndex = rand() % (int)vJewelCabVariants.size();
		const JewelCabVariant& v = vJewelCabVariants[inst.VariantIndex];

		RequireAnimDict(inst, v.Dict);

		const Hash models[] = { inst.Def->ExtraProps[0].PropModel, v.Start, v.End, v.Root, v.RootB };

		inst.ExtraEnts.resize(inst.Def->ExtraProps.size());
		inst.ExtraRuntime.resize(inst.Def->ExtraProps.size());

		for (size_t i = 0; i <= JewelSlotRootB; i++)
		{
			const auto& extraDef = inst.Def->ExtraProps[i];

			SpoonerEntity e = EntityManagement::AddProp(models[i], extraDef.Label);
			if (e.handle.Exists())
			{
				e.handle.SetIsCollisionEnabled(false);
				if (extraDef.StartInvisible)
					e.handle.SetVisible(false);
			}
			inst.ExtraEnts[i] = e;
		}

		for (size_t i = 0; i <= JewelSlotRootB; i++)
		{
			if (inst.ExtraEnts[i].handle.Exists())
				AttachToPrimaryExactly(inst, inst.ExtraEnts[i]);
		}
	}

	static void OnStageTick_JewelCabinet(InteractableInstance& inst, size_t stageIndex)
	{
		if (stageIndex != 0 || inst.SyncedScene == -1 || inst.VariantIndex < 0 || inst.ExtraEnts.size() <= JewelSlotRootB)
			return;

		SpoonerEntity& startProp = inst.ExtraEnts[JewelSlotStart];
		if (!startProp.handle.Exists())
			return;

		if (GET_SYNCHRONIZED_SCENE_PHASE(inst.SyncedScene) < JewelSmashPhase)
			return;

		const JewelCabVariant& v = vJewelCabVariants[inst.VariantIndex];

		EntityManagement::DeleteEntity(startProp);

		for (size_t slot : { JewelSlotRoot, JewelSlotRootB })
		{
			SpoonerEntity& e = inst.ExtraEnts[slot];
			if (e.handle.Exists())
			{
				e.handle.SetVisible(true);
				e.handle.SetAlpha(255);
			}
		}

		PlaySideScene(inst, {
			{ AnimTarget::ExtraProp, v.RootClip,  (int)JewelSlotRoot,  v.Dict },
			{ AnimTarget::ExtraProp, v.RootBClip, (int)JewelSlotRootB, v.Dict } });
	}

	static void CustomCleanup_JewelCabinet(InteractableInstance& inst)
	{
		if (inst.ExtraEnts.size() > JewelSlotEnd && inst.ExtraEnts[JewelSlotEnd].handle.Exists())
		{
			inst.ExtraEnts[JewelSlotEnd].handle.SetVisible(true);
			inst.ExtraEnts[JewelSlotEnd].handle.SetAlpha(255);
		}
	}

	// ========================================================================
	//  5. DEFINITIONS
	//     Adding a new grab stack is one line in vInteractables, e.g.
	//         MakeGrabStack("Grab Diamonds", DiamondStack, GrabGoldDict, "grab_gold"),
	// ========================================================================
	static const char* const GrabCashDict = "anim@scripted@player@mission@tun_table_grab@cash@";
	static const char* const GrabGoldDict = "anim@scripted@player@mission@tun_table_grab@gold@";

	static Interactable MakeGrabStack(const char* label, Hash stackModel, const char* animDict, const char* stackClip)
	{
		return Interactable(label, stackModel)
			.AnimDict(animDict)
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddStage(Stage(6000)
				.ExtraPropClip(0, "enter_bag")
				.PlayerClip("enter")
				.Facial("enter_facial"))
			.AddStage(Stage(30000)
				.PrimaryPropClip(stackClip)
				.ExtraPropClip(0, "grab_bag")
				.PlayerClip("grab")
				.Facial("grab_facial"))
			.AddStage(Stage(6000)
				.ExtraPropClip(0, "exit_bag")
				.PlayerClip("exit")
				.Facial("exit_facial"))
			.DeletePropsWhenDone()
			.StandOffset(0.0f, -0.85f, 0.10f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.UnarmedDuringScene();
	}

	static Interactable MakeGrabClipboard(const char* label, Hash model)
	{
		return Interactable("Grab Clipboard", ClipboardSchedule)
			.AnimDict("anim@heists@prison_heiststation@")
			.AddExtraProp(ExtraProp("Grabbed Clipboard", ClipboardSchedule2)
				.AttachToPlayer(AttachBone::LeftWrist)
				.SpawnedByHook())
			.AddStage(Stage(30000)
				.PrimaryPropClip("pickup_bus_schedule_schedule")
				.PlayerClip("pickup_bus_schedule"))
			.DeletePropsWhenDone()
			.StandOffset(0.0f, 0.0f, 0.10f)
			.InteractRange(1.0f)
			.SlideToMark(8.0f, -1.0f)
			.PrimaryPropIgnoresSceneOrigin()
			.UnarmedDuringScene()
			.KeepPlayerWhereTheyStand()
			.PlayerUsesTaskPlayAnim()
			.OnStageStart(&OnStageStart_Clipboard);
	}

	static Interactable MakePullHardDrive1()
	{
		return Interactable("Pull Hard Drive (With Bag)", ServerCleanStack)
			.AnimDict("anim@scripted@player@mission@tun_bunk_ig1_hdd_server@male@")
			.AddExtraProp(ExtraProp("Server Drive", ServerDrive)
				.AttachToPrimaryProp(Vector3(0.0f, -0.2700f, 0.9000f))
				.SpawnsAtPlacement()
				.PinnedUntilActivated()
				.DeleteWhenDone())
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddStage(Stage(30000)
				.PlayerClip("action")
				.ExtraPropClip(0, "action_drive")
				.ExtraPropClip(1, "action_bag")
				.Facial("action_facial")
				.Blend(16.0f, -16.0f))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, -1.0f, 1.0f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.SceneOrigin(0.0f, -0.3f, 0.7f)
			.PrimaryPropIgnoresSceneOrigin()
			.PlayerAlsoPlaysEntityAnim()
			.UnarmedDuringScene()
			.OnSpawn(&SpawnContainerPersistentProps);
			// Big thanks to HKH for the help on this one!
	}

	static Interactable MakePullHardDrive2()
	{
		return Interactable("Pull Hard Drive", ServerCleanStack)
			.AnimDict("anim@scripted@player@fix_club_ig1_hdd_remove@male@")
			.AddExtraProp(ExtraProp("Server Drive", ServerDrive)
				.AttachToPrimaryProp(Vector3(0.0f, -0.2700f, 0.9000f))
				.SpawnsAtPlacement()
				.PinnedUntilActivated()
				.DeleteWhenDone())
			.AddStage(Stage(30000)
				.PlayerClip("action")
				.PrimaryPropClip("action_server")
				.ExtraPropClip(0, "action_drive")
				.Facial("action_facial")
				.Blend(16.0f, -16.0f))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, -1.0f, 1.0f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.SceneOrigin(0.0f, -0.3f, 0.8f)
			.PrimaryPropIgnoresSceneOrigin()
			.PlayerAlsoPlaysEntityAnim()
			.UnarmedDuringScene()
			.OnSpawn(&SpawnContainerPersistentProps);
	}

	static Interactable MakeOpenContainer(const char* label, Hash closedModel, Hash openedModel, const char* animDict)
	{
		return Interactable(label, closedModel)
			.AddExtraProp(ExtraProp("Opened Container", openedModel)
				.AttachToPrimaryProp()
				.StartsHidden()
				.KeepWhenDone())
			.AnimDict(animDict)
			.AddStage(Stage(6000)
				.PlayerClip("action")
				.PrimaryPropClip("action_container")
				.Facial("action_facial"))
			.KeepPropsWhenDone()
			.AddEndState(EndState::Primary().Alpha(0).Collision(CollisionState::Off))
			.AddEndState(EndState::Extra(0).Alpha(255).Collision(CollisionState::On).Visibility(VisibilityState::Shown))
			.StandOffset(0.0f, -3.90f, 1.25f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.UnarmedDuringScene();
	}

	// Extra slots: 0 Bag, 1 Grinder, 2 Lock, 3 Collision proxy.
	static Interactable MakeAngleGrinderContainer()
	{
		return Interactable("Open Container (Angle Grinder)", TrContainer01a)
			.RandomPrimaryModel({ TrContainer01a, TrContainer01b, TrContainer01c, TrContainer01d, TrContainer01e,
								  TrContainer01f, TrContainer01g, TrContainer01h, TrContainer01i })
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddExtraProp(ExtraProp("Angle Grinder", TrGrinder01a).AttachToPlayer(AttachBone::LeftWrist))
			.AddExtraProp(ExtraProp("Lock", TrLock01a)
				.AttachToPrimaryProp(Vector3(0.0f, -1.8130f, 1.2000f))
				.SpawnsAtPlacement()
				.KeepWhenDone()
				.PinnedUntilActivated())
			.AddExtraProp(ExtraProp("Container Collision", TrContCollision)
				.AttachToPrimaryProp()
				.StartsHidden()
				.SpawnsAtPlacement()
				.KeepWhenDone())
			.AnimDict("anim@scripted@player@mission@tunf_train_ig1_container_p1@male@")
			.AddStage(Stage(30000)
				.PlayerClip("action")
				.PrimaryPropClip("action_container")
				.ExtraPropClip(0, "action_bag")
				.ExtraPropClip(1, "action_angle_grinder")
				.ExtraPropClip(2, "action_lock"))
			.KeepPropsWhenDone()
			.AddEndState(EndState::Primary().Alpha(255).Collision(CollisionState::Off).Visibility(VisibilityState::Shown))
			.AddEndState(EndState::Extra(3).Alpha(0).Collision(CollisionState::On).Visibility(VisibilityState::Hidden))
			.StandOffset(0.0f, -2.35f, 1.0f)
			.InteractRange(0.50f)
			.OnSpawn(&SpawnContainerPersistentProps)
			.SlideToMark(8.0f, -1.0f)
			.UnarmedDuringScene();
	}

	static Interactable MakeElectricBox(const char* label, Hash model)
	{
		return Interactable(label, model)
			.AnimDict("anim@scripted@heist@ig9_control_tower@male@")
			.AddStage(Stage(6000).PlayerClip("enter").PrimaryPropClip("enter_electric_box"))
			.AddStage(Stage(30000).PlayerClip("loop").PrimaryPropClip("loop_electric_box").Facial("loop_facial"))
			.AddStage(Stage(6000).PlayerClip("exit").PrimaryPropClip("exit_electric_box"))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, -1.0f, 1.25f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.UnarmedDuringScene();
	}

	static Interactable MakeLever(const char* label, Hash model, float standZ)
	{
		return Interactable(label, model)
			.AnimDict("anim@scripted@heist@ig2_hand_lever@male@")
			.AddStage(Stage(6000).PlayerClip("enter").PrimaryPropClip("enter_lever").Facial("enter_male_facial"))
			.AddStage(Stage(6000).PlayerClip("exit").PrimaryPropClip("exit_lever").Facial("exit_male_facial"))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, -0.65f, standZ)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.UnarmedDuringScene();
	}

	static Interactable MakePowerBox(const char* label, Hash model)
	{
		return Interactable(label, model)
			.AnimDict("anim@gangops@hanger@fuse_box@")
			.AddStage(Stage(6000).PlayerClip("switch_on").PrimaryPropClip("switch_on_box"))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, -0.85f, 0.0f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.UnarmedDuringScene();
	}

	static Stage MakeOverheatStage(int n)
	{
		const std::string base = "overheat_react_0" + std::to_string(n);
		return Stage(6000)
			.PlayerClip(base.c_str())
			.PrimaryPropClip((base + "_glass_display").c_str())
			.ExtraPropClip(1, (base + "_cutter").c_str())
			.ExtraPropClip(0, (base + "_bag").c_str())
			.Facial((base + "_facial").c_str());
	}

	static const char* const StealPaintingDict = "anim_heist@hs3f@ig11_steal_painting@male@";
	struct StealPaintingLayout
	{
		const char* PrimarySuffix;
		int BagSlot;
		int PaintingSlot;
	};

	static const StealPaintingLayout StealPaintingOnWall = { "ch_prop_vault_painting_01a", 0, -1 };
	static const StealPaintingLayout StealPaintingInCase = { "ch_prop_ch_sec_cabinet_02a", 0, 1 };

	static Stage MakeStealPaintingStage(const std::string& version, const char* action,
		const StealPaintingLayout& layout = StealPaintingOnWall)
	{
		const std::string base = version + "_" + action;

		Stage stage = Stage(30000)
			.PlayerClip(base.c_str())
			.PrimaryPropClip((base + "_" + layout.PrimarySuffix).c_str())
			.ExtraPropClip(layout.BagSlot, (base + "_hei_p_m_bag_var22_arm_s").c_str())
			.PlayerWeaponClip((base + "_w_me_switchblade").c_str())
			.PauseOnEnd();

		if (layout.PaintingSlot >= 0)
			stage.ExtraPropClip(layout.PaintingSlot, (base + "_ch_prop_vault_painting_01a").c_str());

		return stage;
	}

	// 0 Bag, 1 Cutter, 2 Display Case (Opened), 3 Glass Piece, 4 Treasure (Display), 5 Treasure (Item).
	static Interactable MakeDisplayCase()
	{
		return Interactable("Cut Open Display Case", DisplayCase1)
			.AnimDict("anim@scripted@heist@ig16_glass_cut@male@")
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddExtraProp(ExtraProp("Plasma Cutter", PlasmaCutter).AttachToPlayer(AttachBone::RightWrist))
			.AddExtraProp(ExtraProp("Display Case (Opened)", DisplayCase2)
				.AttachToPrimaryProp().StartsHidden().KeepWhenDone().PinnedUntilActivated().SpawnedByHook())
			.AddExtraProp(ExtraProp("Glass Piece", DisplayCase3)
				.AttachToPlayer(AttachBone::RightWrist).SpawnedByHook())
			.AddExtraProp(ExtraProp("Treasure (Display)", PericoTarget1B)
				.AttachToPrimaryProp().KeepWhenDone().PinnedUntilActivated().SpawnedByHook())
			.AddExtraProp(ExtraProp("Treasure (Item)", PericoTarget1A)
				.AttachToPrimaryProp().KeepWhenDone().PinnedUntilActivated().SpawnedByHook())
			.AddStage(Stage(6000)
				.PlayerClip("enter")
				.PrimaryPropClip("enter_glass_display")
				.ExtraPropClip(1, "enter_cutter")
				.ExtraPropClip(0, "enter_bag")
				.Facial("enter_facial"))
			.AddStage(Stage(2000)
				.PlayerClip("cutting_loop")
				.PrimaryPropClip("cutting_loop_glass_display")
				.ExtraPropClip(1, "cutting_loop_cutter")
				.ExtraPropClip(0, "cutting_loop_bag")
				.Facial("cutting_loop_facial"))
			.AddStage(MakeOverheatStage(1))
			.AddStage(MakeOverheatStage(2))
			.AddStage(MakeOverheatStage(3))
			.AddStage(Stage(10000)
				.PlayerClip("success")
				.ExtraPropClip(1, "success_cutter")
				.ExtraPropClip(0, "success_bag")
				.ExtraPropClip(2, "success_glass_display_cut")
				.ExtraPropClip(3, "success_glass_piece")
				.Facial("success_facial"))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, -1.0f, 1.0f)
			.InteractRange(0.50f)
			.OnSpawn(&SpawnDisplayCasePersistentProps)
			.OnStageStart(&OnStageStart_DisplayCase)
			.PickNextStage(&GetNextStage_DisplayCase)
			.OnCleanup(&CustomCleanup_DisplayCase)
			.OnStageTick(&OnStageTick_DisplayCase)
			.SlideToMark(8.0f, -1.0f)
			.UnarmedDuringScene();
	}

	static Interactable MakeLaptop()
	{
		return Interactable("Use Laptop", Laptop)
			.AnimDict("anim@amb@warehouse@laptop@")
			.AddStage(Stage(6000).PlayerClip("enter"))
			.AddStage(Stage(NoTimeout).PlayerClip("idle_a").LoopsUntilInteract())
			.AddStage(Stage(6000).PlayerClip("exit"))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, -1.0f, 0.0f)
			.InteractRange(0.50f)
			.Repeatable()
			.SlideToMark(8.0f, -1.0f)
			.UnarmedDuringScene();
	}

	static Interactable MakeGuitar()
	{
		return Interactable("Play Guitar", GuitarStand)
			.AddExtraProp(ExtraProp("Held Guitar", Guitar)
				.AttachToPlayer(AttachBone::LeftWrist)
				.SpawnedByHook())
			.AnimDict("amb@world_human_musician@guitar@male@base")
			.AddStage(Stage(NoTimeout)
				.PlayerClip("base")
				.LoopsUntilInteract()
				.PlayerUsesTaskPlayAnim(PlayAnimFlag_Loop))
			.KeepPropsWhenDone()
			.AddEndState(EndState::Primary().Alpha(255).Collision(CollisionState::On).Visibility(VisibilityState::Shown))
			.StandOffset(0.0f, -0.65f, 0.70f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.UnarmedDuringScene()
			.OnStageStart(&OnStageStart_Guitar)
			.Repeatable()
			.KeepPlayerWhereTheyStand()
			.PlayerUsesTaskPlayAnim();
	}

	// Fix Breaker. Stage 0 is built like the Angle Grinder container's single stage: the box rides the player's
	// held scene and nothing touches it afterwards. The end state keeps the fusebox's collision ON.
	static Interactable MakeBreaker(const char* label, SpawnPersistentPropsFn spawnFn)
	{
		return Interactable(label, FuseBox)
			.AddExtraProps(MakeBreakerFuseExtras())
			.AnimDict(BreakerAnimDict)
			.AddStages(MakeBreakerStages())
			.KeepPropsWhenDone()
			.AddEndState(EndState::Primary().Alpha(255).Collision(CollisionState::On).Visibility(VisibilityState::Shown))
			.StandOffset(0.0f, -1.0f, 1.0f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.UnarmedDuringScene()
			.Repeatable()
			.OnSpawn(spawnFn)
			.OnStageStart(&OnStageStart_Breaker)
			.PickNextStage(&GetNextStage_Breaker)
			.OnCleanup(&CustomCleanup_Breaker)
			.ResolveParticipants(&ResolveParticipants_Breaker)
			.OnIdleTick(&OnIdleTick_Breaker);
	}

	static Interactable MakeStealPainting1()
	{
		const std::string version = "ver_01";

		return Interactable("Steal Painting (Diamond Casino) 1", PaintingDiamondCasino01a)
			.RandomPrimaryModel({ PaintingDiamondCasino01a, PaintingDiamondCasino01b, PaintingDiamondCasino01c,
								  PaintingDiamondCasino01d, PaintingDiamondCasino01e, PaintingDiamondCasino01f,
								  PaintingDiamondCasino01g, PaintingDiamondCasino01h, PaintingDiamondCasino01i })
			.AnimDict(StealPaintingDict)
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddStage(MakeStealPaintingStage(version, "top_left_enter"))
			.AddStage(MakeStealPaintingStage(version, "cutting_top_left_to_right"))
			.AddStage(MakeStealPaintingStage(version, "cutting_right_top_to_bottom"))
			.AddStage(MakeStealPaintingStage(version, "cutting_bottom_right_to_left"))
			.AddStage(MakeStealPaintingStage(version, "cutting_left_top_to_bottom"))
			.AddStage(MakeStealPaintingStage(version, "with_painting_exit"))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, -0.55f, 0.20f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.SceneOrigin(0.0f, 0.0f, -1.0f)
			.PlayerAlsoPlaysEntityAnim()
			.EquipDuringScene(WEAPON_SWITCHBLADE);
	}

	static Interactable MakeStealPainting2()
	{
		const std::string version = "ver_02";

		return Interactable("Steal Painting (Diamond Casino) 2", PaintingCase)
			.AnimDict(StealPaintingDict)
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddExtraProp(ExtraProp("Painting", PaintingDiamondCasino01a)
				.RandomExtraModel({ PaintingDiamondCasino01a, PaintingDiamondCasino01b, PaintingDiamondCasino01c,
									PaintingDiamondCasino01d, PaintingDiamondCasino01e, PaintingDiamondCasino01f,
									PaintingDiamondCasino01g, PaintingDiamondCasino01h, PaintingDiamondCasino01i })
				.AttachToPrimaryProp(Vector3(0.0f, 0.5700f, 1.1800f))
				.SpawnsAtPlacement()
				.KeepWhenDone())
			.AddStage(MakeStealPaintingStage(version, "top_left_enter", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "cutting_top_left_to_right", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "cutting_right_top_to_bottom", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "cutting_bottom_right_to_left", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "cutting_left_top_to_bottom", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "with_painting_exit", StealPaintingInCase))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, -0.55f, 1.20f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.EquipDuringScene(WEAPON_SWITCHBLADE)
			.OnSpawn(&SpawnContainerPersistentProps);
	}

	static Interactable MakeStealPainting3()
	{
		const std::string version = "ver_01";

		return Interactable("Steal Painting (Cayo Perico) 1", PaintingCayoPerico01a)
			.RandomPrimaryModel({ PaintingCayoPerico01a, PaintingCayoPerico01b, PaintingCayoPerico01c,
								  PaintingCayoPerico01d, PaintingCayoPerico01e, PaintingCayoPerico01f,
								  PaintingCayoPerico01g, PaintingCayoPerico01h })
			.AnimDict(StealPaintingDict)
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddStage(MakeStealPaintingStage(version, "top_left_enter"))
			.AddStage(MakeStealPaintingStage(version, "cutting_top_left_to_right"))
			.AddStage(MakeStealPaintingStage(version, "cutting_right_top_to_bottom"))
			.AddStage(MakeStealPaintingStage(version, "cutting_bottom_right_to_left"))
			.AddStage(MakeStealPaintingStage(version, "cutting_left_top_to_bottom"))
			.AddStage(MakeStealPaintingStage(version, "with_painting_exit"))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, -0.55f, 0.20f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.SceneOrigin(0.0f, 0.0f, -1.0f)
			.PlayerAlsoPlaysEntityAnim()
			.EquipDuringScene(WEAPON_SWITCHBLADE);
	}

	static Interactable MakeStealPainting4()
	{
		const std::string version = "ver_02";

		return Interactable("Steal Painting (Cayo Perico) 2", PaintingCase)
			.AnimDict(StealPaintingDict)
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddExtraProp(ExtraProp("Painting", PaintingCayoPerico01a)
				.RandomExtraModel({ PaintingCayoPerico01a, PaintingCayoPerico01b, PaintingCayoPerico01c,
								  PaintingCayoPerico01d, PaintingCayoPerico01e, PaintingCayoPerico01f,
								  PaintingCayoPerico01g, PaintingCayoPerico01h })
				.AttachToPrimaryProp(Vector3(0.0f, 0.5700f, 1.1800f))
				.SpawnsAtPlacement()
				.KeepWhenDone())
			.AddStage(MakeStealPaintingStage(version, "top_left_enter", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "cutting_top_left_to_right", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "cutting_right_top_to_bottom", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "cutting_bottom_right_to_left", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "cutting_left_top_to_bottom", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "with_painting_exit", StealPaintingInCase))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, -0.55f, 1.20f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.EquipDuringScene(WEAPON_SWITCHBLADE)
			.OnSpawn(&SpawnContainerPersistentProps);
	}

	static Interactable MakeStealPainting5()
	{
		const std::string version = "ver_01";

		return Interactable("Steal Painting (Bottom Dollar Bounties) 1", PaintingBottomDollar01a)
			.RandomPrimaryModel({ PaintingBottomDollar01a, PaintingBottomDollar01b, PaintingBottomDollar01c,
								  PaintingBottomDollar01d })
			.AnimDict(StealPaintingDict)
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddStage(MakeStealPaintingStage(version, "top_left_enter"))
			.AddStage(MakeStealPaintingStage(version, "cutting_top_left_to_right"))
			.AddStage(MakeStealPaintingStage(version, "cutting_right_top_to_bottom"))
			.AddStage(MakeStealPaintingStage(version, "cutting_bottom_right_to_left"))
			.AddStage(MakeStealPaintingStage(version, "cutting_left_top_to_bottom"))
			.AddStage(MakeStealPaintingStage(version, "with_painting_exit"))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, -0.55f, 0.20f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.SceneOrigin(0.0f, 0.0f, -1.0f)
			.PlayerAlsoPlaysEntityAnim()
			.EquipDuringScene(WEAPON_SWITCHBLADE);
	}

	static Interactable MakeStealPainting6()
	{
		const std::string version = "ver_02";

		return Interactable("Steal Painting (Bottom Dollar Bounties) 2", PaintingCase)
			.AnimDict(StealPaintingDict)
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddExtraProp(ExtraProp("Painting", PaintingBottomDollar01a)
				.RandomExtraModel({ PaintingBottomDollar01a, PaintingBottomDollar01b, PaintingBottomDollar01c,
								  PaintingBottomDollar01d })
				.AttachToPrimaryProp(Vector3(0.0f, 0.5700f, 1.1800f))
				.SpawnsAtPlacement()
				.KeepWhenDone())
			.AddStage(MakeStealPaintingStage(version, "top_left_enter", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "cutting_top_left_to_right", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "cutting_right_top_to_bottom", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "cutting_bottom_right_to_left", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "cutting_left_top_to_bottom", StealPaintingInCase))
			.AddStage(MakeStealPaintingStage(version, "with_painting_exit", StealPaintingInCase))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, -0.55f, 1.20f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.EquipDuringScene(WEAPON_SWITCHBLADE)
			.OnSpawn(&SpawnContainerPersistentProps);
	}

	static Interactable MakeGrabCashFromTrolley1()
	{
		return Interactable("Grab Cash From Trolley 1", TrolleyCash)
			.AnimDict("anim@heists@ornate_bank@grab_cash")
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddStage(Stage(6000)
				.ExtraPropClip(0, "bag_intro")
				.PlayerClip("intro"))
			.AddStage(Stage(30000)
				.PrimaryPropClip("cart_cash_dissapear")
				.ExtraPropClip(0, "bag_grab")
				.PlayerClip("grab"))
			.AddStage(Stage(6000)
				.ExtraPropClip(0, "bag_exit")
				.PlayerClip("exit"))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, 0.85f, 0.65f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f);
	}

	static Interactable MakeGrabCashFromTrolley2()
	{
		return Interactable("Grab Cash From Trolley 2", TrolleyCashDiamondCasino)
			.AnimDict("anim@heists@ornate_bank@grab_cash")
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddStage(Stage(6000)
				.ExtraPropClip(0, "bag_intro")
				.PlayerClip("intro"))
			.AddStage(Stage(30000)
				.PrimaryPropClip("cart_cash_dissapear")
				.ExtraPropClip(0, "bag_grab")
				.PlayerClip("grab"))
			.AddStage(Stage(6000)
				.ExtraPropClip(0, "bag_exit")
				.PlayerClip("exit"))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, 0.85f, 0.65f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f);
	}

	static Interactable MakeGrabDiamondFromTrolley1()
	{
		return Interactable("Grab Diamonds From Trolley", TrolleyDiamond)
			.AnimDict("anim@heists@ornate_bank@grab_cash")
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddStage(Stage(6000)
				.ExtraPropClip(0, "bag_intro")
				.PlayerClip("intro"))
			.AddStage(Stage(60000)
				.PrimaryPropClip("cart_cash_dissapear")
				.ExtraPropClip(0, "bag_grab")
				.PlayerClip("grab"))
			.AddStage(Stage(6000)
				.ExtraPropClip(0, "bag_exit")
				.PlayerClip("exit"))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, 0.85f, 0.65f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f);
	}

	static Interactable MakeGrabGoldFromTrolley1()
	{
		return Interactable("Grab Gold From Trolley", TrolleyGold)
			.AnimDict("anim@heists@ornate_bank@grab_cash")
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddStage(Stage(6000)
				.ExtraPropClip(0, "bag_intro")
				.PlayerClip("intro"))
			.AddStage(Stage(60000)
				.PrimaryPropClip("cart_cash_dissapear")
				.ExtraPropClip(0, "bag_grab")
				.PlayerClip("grab"))
			.AddStage(Stage(6000)
				.ExtraPropClip(0, "bag_exit")
				.PlayerClip("exit"))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, 0.85f, 0.65f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f);
	}

	static Interactable MakeGrabGoldFromTrolley2()
	{
		return Interactable("Grab Gold From Trolley", TrolleyGoldDiamondCasino)
			.AnimDict("anim@heists@ornate_bank@grab_cash")
			.AddExtraProp(ExtraProp("Duffel Bag", BagFallback).MatchWornBag().AttachToPlayer())
			.AddStage(Stage(6000)
				.ExtraPropClip(0, "bag_intro")
				.PlayerClip("intro"))
			.AddStage(Stage(60000)
				.PrimaryPropClip("cart_cash_dissapear")
				.ExtraPropClip(0, "bag_grab")
				.PlayerClip("grab"))
			.AddStage(Stage(6000)
				.ExtraPropClip(0, "bag_exit")
				.PlayerClip("exit"))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, 0.85f, 0.65f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f);
	}

	static Interactable MakeSmashOpenJewelCabinet()
	{
		return Interactable("Smash Open Jewel Cabinet", JewelCabinet)
			.AnimDict("anim@scripted@kortz@ig10_meth_smash_glass@male@")
			.AddExtraProp(ExtraProp("Extra Glass", JewelCabGlass).SpawnedByHook().KeepWhenDone())
			.AddExtraProp(ExtraProp("Glass", JewelCabStart1).SpawnedByHook())
			.AddExtraProp(ExtraProp("Smashed Glass", JewelCabEnd1).StartsHidden().SpawnedByHook().KeepWhenDone())
			.AddExtraProp(ExtraProp("Root", JewelCabRoot1).StartsHidden().SpawnedByHook())
			.AddExtraProp(ExtraProp("Root B", JewelCabRootB1).StartsHidden().SpawnedByHook())
			.AddStage(Stage(30000)
				.PlayerClip("smash_case_a")
				.Facial("smash_case_a_facial"))
			.KeepPropsWhenDone()
			.StandOffset(0.0f, -0.80f, 0.55f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.SceneOrigin(0.0f, 0.0f, -0.6f)
			.OnSpawn(&SpawnJewelCabinetGroup)
			.OnStageTick(&OnStageTick_JewelCabinet)
			.OnCleanup(&CustomCleanup_JewelCabinet);
	}

	static Interactable MakeVendingMachine1()
	{
		return Interactable("Drink Ecola Can", VendingMachineEcola)
			.AnimDict("mini@sprunk")
			.AddStage(Stage(60000)
				.PlayerClip("plyr_buy_drink_pt1"))
			.AddExtraProp(ExtraProp("Soda Can", SodaCanEcola)
				.AttachToPlayer(AttachBone::RightWrist))
			.AddEndState(EndState::Extra(0).Collision(CollisionState::On))
			.AddStage(Stage(60000)
				.PlayerClip("plyr_buy_drink_pt2"))
			.AddStage(Stage(60000)
				.PlayerClip("plyr_buy_drink_pt3")
				.DetachExtraProp(0))
			.StandOffset(0.0f, -0.85f, 0.15f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.SceneOrigin(0.0f, -1.0f, 0.01f)
			.KeepPropsWhenDone()
			.Repeatable();
	}

	static Interactable MakeVendingMachine2()
	{
		return Interactable("Drink Sprunk Can", VendingMachineSprunk)
			.AnimDict("mini@sprunk")
			.AddStage(Stage(60000)
				.PlayerClip("plyr_buy_drink_pt1"))
			.AddExtraProp(ExtraProp("Soda Can", SodaCanSprunk)
				.AttachToPlayer(AttachBone::RightWrist))
			.AddStage(Stage(60000)
				.PlayerClip("plyr_buy_drink_pt2"))
			.AddStage(Stage(60000)
				.PlayerClip("plyr_buy_drink_pt3")
				.DetachExtraProp(0))
			.StandOffset(0.0f, -0.85f, 0.15f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.SceneOrigin(0.0f, -1.0f, 0.01f)
			.KeepPropsWhenDone()
			.Repeatable();
	}

	static Interactable MakeVendingMachine3()
	{
		return Interactable("Drink Junk Can", VendingMachineJunk)
			.AnimDict("mini@sprunk")
			.AddStage(Stage(60000)
				.PlayerClip("plyr_buy_drink_pt1"))
			.AddExtraProp(ExtraProp("Soda Can", SodaCanJunk)
				.AttachToPlayer(AttachBone::RightWrist))
			.AddStage(Stage(60000)
				.PlayerClip("plyr_buy_drink_pt2"))
			.AddStage(Stage(60000)
				.PlayerClip("plyr_buy_drink_pt3")
				.DetachExtraProp(0))
			.AddEndState(EndState::Extra(0).Collision(CollisionState::On))
			.StandOffset(0.0f, -0.85f, 1.0f)
			.InteractRange(0.50f)
			.SlideToMark(8.0f, -1.0f)
			.SceneOrigin(0.0f, -1.0f, 1.0f)
			.KeepPropsWhenDone()
			.Repeatable();
	}

	std::vector<InteractableDefinition> vInteractables =
	{
		MakeGrabStack("Grab Cash",       CashStack,      GrabCashDict, "grab_cash"),
		MakeGrabStack("Grab Weed",       WeedStack,      GrabCashDict, "grab_cash"),
		MakeGrabStack("Grab Gold",       GoldStack,      GrabGoldDict, "grab_gold"),
		MakeGrabStack("Grab Cocaine",    CokeStack,      GrabCashDict, "grab_cash"),
		MakeGrabStack("Grab Books",      BookStack,      GrabGoldDict, "grab_gold"),
		MakeGrabStack("Grab Servers",    ServerStack,    GrabGoldDict, "grab_gold"),
		MakeGrabStack("Grab Hard Drives", HarddriveStack, GrabGoldDict, "grab_gold"),

		MakeOpenContainer("Open Container",          ContainerClosed,     ContainerOpened,     "anim@scripted@freemode@ig1_open_container@no_tools@male@"),
		MakeAngleGrinderContainer(),
		MakeOpenContainer("Open Container (Keypad)", Container_MW_Closed, Container_MW_Opened, "anim@scripted@freemode@ig1_open_container@male@"),

		MakeElectricBox("Use Electric Box 1", ElectricBox1),
		MakeElectricBox("Use Electric Box 2", ElectricBox2),

		MakeLever("Flip Lever 1", Lever1,  0.0f),
		MakeLever("Flip Lever 2", Lever2,  0.0f),
		MakeLever("Flip Lever 3", Lever3, -0.15f),

		MakePowerBox("Flip Lever 4", PowerBox),

		MakeDisplayCase(),
		MakeLaptop(),
		MakeGuitar(),

		MakeBreaker("Fix Breaker (Pre-Opened)", &SpawnBreaker_PreOpened),
		MakeBreaker("Fix Breaker",  &SpawnBreaker_AllBlown),

		MakeGrabClipboard("Grab Clipboard", ClipboardSchedule),

		MakePullHardDrive1(),
		MakePullHardDrive2(),

		MakeStealPainting1(),
		MakeStealPainting2(),
		MakeStealPainting3(),
		MakeStealPainting4(),
		MakeStealPainting5(),
		MakeStealPainting6(),

		MakeGrabDiamondFromTrolley1(),
		MakeGrabCashFromTrolley1(),
		MakeGrabCashFromTrolley2(),
		MakeGrabGoldFromTrolley1(),
		MakeGrabGoldFromTrolley2(),

		MakeSmashOpenJewelCabinet(),

		MakeVendingMachine1(),
		MakeVendingMachine2(),
		MakeVendingMachine3(),
	};

	std::vector<InteractableInstance> vActiveInteractables;

	// ========================================================================
	//  6. ENGINE
	// ========================================================================
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

	static bool IsStageFinished(const InteractableInstance& inst, const InteractableStage& stage)
	{
		DWORD elapsed = GET_GAME_TIMER() - inst.ActivationTime;
		bool timedOut = stage.MaxDurationMs > 0 && elapsed >= (DWORD)stage.MaxDurationMs;

		if (inst.Def->PlayerAnim == PlayerAnimMethod::TaskPlayAnim)
		{
			if (timedOut)
				return true;
			if (elapsed < 500)
				return false;

			for (auto& p : stage.Participants)
			{
				if (p.Target == AnimTarget::Player)
					return !IS_ENTITY_PLAYING_ANIM(PLAYER_PED_ID(), ParticipantDict(inst, p), p.ClipName.c_str(), 3);
			}
			return true;
		}

		bool sceneFinished = (inst.SyncedScene == -1) || GET_SYNCHRONIZED_SCENE_PHASE(inst.SyncedScene) >= 0.99f;
		return sceneFinished || timedOut;
	}

	static void StartStage(InteractableInstance& inst, size_t stageIndex)
	{
		if (inst.Def->OnStageStart)
			inst.Def->OnStageStart(inst, stageIndex);

		const InteractableStage& stage = inst.Def->Stages[stageIndex];

		for (int extraIndex : stage.DetachExtraProps)
		{
			if (extraIndex < 0 ||
				static_cast<size_t>(extraIndex) >= inst.ExtraEnts.size())
				continue;

			SpoonerEntity& extraEnt = inst.ExtraEnts[extraIndex];

			if (!extraEnt.handle.Exists())
				continue;

			ENTITY::DETACH_ENTITY(extraEnt.handle.Handle(), true, true);

			if (EntityManagement::GetEntityIndexInDb(extraEnt.handle) < 0)
			{
				EntityManagement::AddEntityToDb(extraEnt, true);
			}
		}

		std::vector<SceneParticipant> participants = stage.Participants;
		if (inst.Def->ResolveParticipants)
			inst.Def->ResolveParticipants(inst, stageIndex, participants);

		GTAped myPed = PLAYER_PED_ID();

		if (inst.Def->RepositionPlayer && inst.Def->Approach == ApproachMethod::Teleport)
		{
			Vector3 standPos = GetInteractPosition(inst);
			myPed.SetPosition(standPos);
			myPed.SetHeading(inst.AnchorRot.z + inst.Def->PlayerHeadingOffset);
		}

		const bool playerUsesTaskPlayAnim = inst.Def->PlayerAnim == PlayerAnimMethod::TaskPlayAnim;
		const bool stageHasProps = std::any_of(participants.begin(), participants.end(),
			[](const SceneParticipant& p) { return p.Target != AnimTarget::Player; });
		const bool needsScene = !playerUsesTaskPlayAnim || stageHasProps;

		const bool propsGetOwnScene = stageHasProps && !playerUsesTaskPlayAnim && !stage.PauseOnEnd;

		inst.ActivationTime = GET_GAME_TIMER();
		inst.SyncedScene = -1;

		int playerScene = -1;
		int propScene = -1;
		int primaryScene = -1;

		if (needsScene)
		{
			const bool firstSceneHolds = playerUsesTaskPlayAnim ? stage.PauseOnEnd : true;
			playerScene = CreateScene(inst, stage, firstSceneHolds);
			propScene = playerScene;

			if (playerScene != -1 && propsGetOwnScene)
				propScene = CreateScene(inst, stage, false);

			primaryScene = propScene;
			if (playerScene != -1 && inst.Def->PrimaryPropKeepsOwnOrigin)
				primaryScene = CreateScene(inst, stage, stage.PauseOnEnd, false);

			inst.SyncedScene = playerScene;

			if (playerScene == -1 || propScene == -1 || primaryScene == -1)
			{
				Game::Print::PrintBottomCentre("~r~Error:~s~ Failed to create synced scene.");
				return;
			}
		}

		if (stage.LoopUntilInteract)
		{
			Game::CustomHelpText::ShowTimedText(oss_ << "Press ~INPUT_CONTEXT~ again to stop.", 4000);
			SET_AUDIO_FLAG("IsDirectorModeActive", true);
		}

		if (!stage.FacialClip.empty())
		{
			PED::PLAY_FACIAL_ANIM(myPed.Handle(), stage.FacialClip.c_str(), inst.Def->AnimDict.c_str());
		}

		for (auto& p : participants)
		{
			if (p.Target == AnimTarget::Player)
			{
				myPed.SetCanRagdoll(false);
				SET_PED_RAGDOLL_ON_COLLISION(myPed.Handle(), false);
				inst.bRagdollDisabled = true;

				if (playerUsesTaskPlayAnim)
				{
					TASK_PLAY_ANIM(
						myPed.Handle(),
						inst.Def->AnimDict.c_str(), p.ClipName.c_str(),
						stage.BlendIn, stage.BlendOut,
						stage.PlayAnim.DurationMs, stage.PlayAnim.Flag, stage.PlayAnim.PlaybackRate,
						stage.PlayAnim.LockPos, stage.PlayAnim.LockPos, stage.PlayAnim.LockPos
					);
				}
				else
				{
					TASK::TASK_SYNCHRONIZED_SCENE(
						myPed.Handle(), playerScene,
						inst.Def->AnimDict.c_str(), p.ClipName.c_str(),
						stage.BlendIn, stage.BlendOut,
						stage.Flags, stage.RagdollBlockingFlags, stage.MoverBlendDelta, stage.IkFlags
					);

					if (inst.Def->PlayerAlsoPlaysEntityAnim)
					{
						ENTITY::PLAY_SYNCHRONIZED_ENTITY_ANIM(
							myPed.Handle(), playerScene,
							p.ClipName.c_str(), inst.Def->AnimDict.c_str(),
							stage.BlendIn, stage.BlendOut, stage.PropFlags, stage.PlaybackRate
						);
					}
				}
			}
			else if (p.Target == AnimTarget::PlayerWeapon)
			{
				PlayWeaponParticipant(inst, p, propScene, stage);
			}
			else
			{
				PlayPropParticipant(inst, p, (p.Target == AnimTarget::PrimaryProp) ? primaryScene : propScene, stage);
			}
		}
	}

	static const char* FirstPlayerClip(const InteractableDefinition& def)
	{
		for (auto& st : def.Stages)
			for (auto& p : st.Participants)
				if (p.Target == AnimTarget::Player)
					return p.ClipName.c_str();
		return nullptr;
	}

	static void GetApproachTarget(const InteractableInstance& inst, Vector3& outPos, float& outHeading)
	{
		outPos = GetInteractPosition(inst);
		outHeading = inst.AnchorRot.z + inst.Def->PlayerHeadingOffset;

		if (inst.Def->Approach == ApproachMethod::SlideToClipStart)
		{
			const char* clip = FirstPlayerClip(*inst.Def);
			if (clip)
			{
				const char* dict = inst.Def->AnimDict.c_str();
				const Vector3 o = GetSceneOrigin(inst);
				const Vector3& r = inst.AnchorRot;
				const float yaw = r.z + inst.Def->SceneHeadingOffset;

				outPos = PED::GET_ANIM_INITIAL_OFFSET_POSITION(dict, clip, o.x, o.y, o.z, r.x, r.y, yaw, 0.0f, 2);
				outHeading = PED::GET_ANIM_INITIAL_OFFSET_ROTATION(dict, clip, o.x, o.y, o.z, r.x, r.y, yaw, 0.0f, 2).z;
			}
		}

		outPos.z += inst.Def->ApproachZOffset;
	}

	static void StartApproach(InteractableInstance& inst)
	{
		GTAped myPed = PLAYER_PED_ID();

		Vector3 mark;
		float heading;
		GetApproachTarget(inst, mark, heading);

		ScrHandle seq = 0;
		OPEN_SEQUENCE_TASK(&seq);
		TASK::TASK_PED_SLIDE_TO_COORD(0, mark.x, mark.y, mark.z, heading, inst.Def->ApproachSpeed);
		CLOSE_SEQUENCE_TASK(seq);
		TASK_PERFORM_SEQUENCE(myPed.Handle(), seq);
		CLEAR_SEQUENCE_TASK(&seq);

		inst.bActivated = true;
		inst.Phase = InteractPhase::Approaching;
		inst.ApproachStartTime = GET_GAME_TIMER();
	}

	static void BeginScenePlayback(InteractableInstance& inst)
	{
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
			if (extraDef.SpawnAtPlacement || extraDef.ManualSpawn)
				continue;

			Hash modelToSpawn = extraDef.MatchWornComponent
				? ResolveWornBagModel(wornBag, extraDef.PropModel)
				: PickExtraModel(inst, i);

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

				int attachBone = (extraDef.AttachTarget == ExtraPropAttachTarget::PrimaryProp)
					? extraDef.PrimaryAttachBone
					: ResolveAttachBoneIndex(myPed, extraDef.BoneOverride, rootBone);

				EntityManagement::AttachEntity(extraEnt, attachTarget, attachBone, extraDef.AttachOffset, extraDef.AttachRotation);
				EntityManagement::RemoveEntityFromDb(extraEnt);
				ApplyExtraDynamic(extraEnt, extraDef);
			}
			inst.ExtraEnts[i] = extraEnt;
		}

		size_t startStage = (inst.ResumeStage < inst.Def->Stages.size()) ? inst.ResumeStage : 0;
		inst.CurrentStage = startStage;
		StartStage(inst, startStage);
		inst.bActivated = true;
		inst.Phase = InteractPhase::Playing;
	}

	static void ActivateInteractable(InteractableInstance& inst)
	{
		bool primaryAnimates = false;
		for (auto& st : inst.Def->Stages)
			for (auto& p : st.Participants)
				if (p.Target == AnimTarget::PrimaryProp) { primaryAnimates = true; break; }

		if (primaryAnimates)
			inst.Ent.handle.FreezePosition(false);

		if (!LoadAnimDicts(CollectAnimDicts(inst)))
		{
			Game::Print::PrintBottomCentre("~r~Error:~s~ Anim dict failed to load.");
			return;
		}

		GTAped myPed = PLAYER_PED_ID();

		const Hash sceneWeapon = (inst.Def->SceneWeapon != 0)
			? inst.Def->SceneWeapon
			: (inst.Def->UnarmedDuringScene ? (Hash)WEAPON_UNARMED : 0);

		if (sceneWeapon != 0 && !inst.bWeaponHolstered)
		{
			Hash current = 0;
			GET_CURRENT_PED_WEAPON(myPed.Handle(), &current, true);
			inst.PreviousWeapon = current;

			if (sceneWeapon != WEAPON_UNARMED)
				GIVE_WEAPON_TO_PED(myPed.Handle(), sceneWeapon, 1, true, false);

			SET_CURRENT_PED_WEAPON(myPed.Handle(), sceneWeapon, true);
			inst.bWeaponHolstered = true;

			if (inst.Def->SceneWeapon != 0)
			{
				int weaponWait = 0;
				while (!GTAentity(GET_CURRENT_PED_WEAPON_ENTITY_INDEX(myPed.Handle(), 0)).Exists() && weaponWait++ < 60)
					WAIT(0);
			}
		}

		if (inst.Def->Approach != ApproachMethod::Teleport)
		{
			StartApproach(inst);
			return;
		}

		BeginScenePlayback(inst);
	}

	void Tick()
	{
		if (vActiveInteractables.empty())
			return;

		GTAped myPed = PLAYER_PED_ID();
		Vector3 myPos = myPed.GetPosition();
		bool bAnyInstanceDead = false;

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
				bAnyInstanceDead = true;
				continue;
			}

			if (inst.bCleanupDone)
				continue;

			if (inst.bActivated)
			{
				if (inst.Phase == InteractPhase::Approaching)
				{
					const DWORD elapsed = GET_GAME_TIMER() - inst.ApproachStartTime;
					const bool arrived = elapsed > 100 && GET_SEQUENCE_PROGRESS(myPed.Handle()) == -1;

					if (arrived || elapsed > (DWORD)inst.Def->ApproachTimeoutMs)
					{
						if (!arrived)
						{
							Vector3 mark;
							float heading;
							GetApproachTarget(inst, mark, heading);
							mark.z -= inst.Def->ApproachZOffset;
							CLEAR_PED_TASKS_IMMEDIATELY(myPed.Handle());
							myPed.SetPosition(mark);
							myPed.SetHeading(heading);
						}
						BeginScenePlayback(inst);
					}
					continue;
				}

				if (inst.Def->OnStageTick)
					inst.Def->OnStageTick(inst, inst.CurrentStage);

				const InteractableStage& stage = inst.Def->Stages[inst.CurrentStage];

				bool stageShouldEnd;
				if (stage.LoopUntilInteract)
				{
					bool pastGracePeriod = (GET_GAME_TIMER() - inst.ActivationTime) > 500;
					stageShouldEnd = pastGracePeriod && PAD::IS_CONTROL_JUST_PRESSED(0, INPUT_CONTEXT);
					if (stageShouldEnd)
						SET_AUDIO_FLAG("IsDirectorModeActive", false);
				}
				else
				{
					stageShouldEnd = IsStageFinished(inst, stage);
				}

				if (stageShouldEnd)
				{
					size_t nextStage = inst.Def->GetNextStage
						? inst.Def->GetNextStage(inst, inst.CurrentStage)
						: inst.CurrentStage + 1;

					if (nextStage < inst.Def->Stages.size())
					{
						inst.CurrentStage = nextStage;
						StartStage(inst, nextStage);
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

						if (inst.bWeaponHolstered)
						{
							SET_CURRENT_PED_WEAPON(myPed.Handle(), inst.PreviousWeapon, true);
							inst.bWeaponHolstered = false;
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

						if (inst.Def->CustomCleanup)
							inst.Def->CustomCleanup(inst);

						inst.Phase = InteractPhase::Idle;

						if (inst.Def->Repeatable)
						{
							inst.bActivated = false;
							inst.CurrentStage = 0;
						}
						else
						{
							inst.bCleanupDone = true;
						}
					}
				}

				continue;
			}

			if (inst.Def->OnIdleTick)
				inst.Def->OnIdleTick(inst);

			if (!inst.bAnchorLocked)
			{
				inst.AnchorPos = inst.Ent.handle.GetPosition();
				inst.AnchorRot = inst.Ent.handle.GetRotation();
			}

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

			const Vector3 entPos = GetInteractPosition(inst);
			const float range = inst.Def->InteractRange;
			const float dx = myPos.x - entPos.x;
			const float dy = myPos.y - entPos.y;
			const float dz = myPos.z - entPos.z;

			if ((dx * dx + dy * dy + dz * dz) <= range * range)
			{
				Game::CustomHelpText::ShowTimedText("Press ~INPUT_CONTEXT~ to interact.", 4000);

				if (PAD::IS_CONTROL_JUST_PRESSED(0, INPUT_CONTEXT))
				{
					ActivateInteractable(inst);
				}
			}
			else
			{
				inst.bHasPlayedInteractSound = false;
			}
		}

		if (bAnyInstanceDead)
		{
			vActiveInteractables.erase(
				std::remove_if(vActiveInteractables.begin(), vActiveInteractables.end(),
					[](const InteractableInstance& i) { return !i.Ent.handle.Exists(); }),
				vActiveInteractables.end()
			);
		}
	}
}