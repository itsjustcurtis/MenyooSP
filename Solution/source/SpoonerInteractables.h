#pragma once

#include <string>
#include <vector>
#include <initializer_list>
#include "Natives/natives.h"
#include "Natives/natives2.h"
#include "Util/GTAmath.h"
#include "Submenus/Spooner/SpoonerEntity.h"
#include "Scripting/enums.h"
#include "Scripting/GTAped.h"

namespace sub::Spooner::Interactables
{
	// ========================================================================
	//  1. ENUMS & NAMED CONSTANTS
	// ========================================================================
	enum class AnimTarget { Player, PrimaryProp, ExtraProp, PlayerWeapon };
	enum class BagOpenState { NoChange, Opened, Closed };
	enum class ExtraPropAttachTarget { Player, PrimaryProp };
	enum class AttachBone { Default, LeftWrist, RightWrist };
	enum class PlayerAnimMethod { SynchronizedScene, TaskPlayAnim };
	enum class CollisionState { Off, On };
	enum class VisibilityState { Hidden, Shown };
	enum class ApproachMethod { Teleport, SlideToMark, SlideToClipStart };
	enum class InteractPhase { Idle, Approaching, Playing };

	constexpr int NoTimeout = 0;
	constexpr int PlayAnimFlag_Loop = 1;

	// ========================================================================
	//  2. DATA
	// ========================================================================
	struct InteractableInstance;

	static int ResolveAttachBoneIndex(GTAped& ped, AttachBone bone, int defaultBone)
	{
		switch (bone)
		{
		case AttachBone::LeftWrist:  return ped.GetBoneIndex(Bone::LeftWrist);
		case AttachBone::RightWrist: return ped.GetBoneIndex(Bone::RightWrist);
		default:                     return defaultBone;
		}
	}

	struct SceneParticipant
	{
		AnimTarget Target;
		std::string ClipName;
		int ExtraPropIndex = 0;
	};

	struct ExtraPropDef
	{
		std::string Label;
		Hash PropModel;
		bool MatchWornComponent = false;
		ExtraPropAttachTarget AttachTarget = ExtraPropAttachTarget::Player;
		bool StartInvisible = false;
		bool SpawnAtPlacement = false;
		AttachBone BoneOverride = AttachBone::Default;
		bool DeleteOnComplete = true;
		bool FreezeAtSpawn = false;
		Vector3 AttachOffset = Vector3();
		Vector3 AttachRotation = Vector3();
		bool ManualSpawn = false;
		std::vector<Hash> PropModelVariants;
	};

	struct PropEndState
	{
		AnimTarget Target;
		int ExtraPropIndex = 0;
		int Alpha = 255;
		bool CollisionEnabled = true;
		bool Visible = true;
	};

	struct PlayAnimSettings
	{
		int Flag = 0;
		int DurationMs = -1;
		float PlaybackRate = 0.0f;
		bool LockPos = false;
	};

	struct InteractableStage
	{
		std::vector<SceneParticipant> Participants;
		int MaxDurationMs = 6000;
		float SceneRate = 1.0f;
		std::string FacialClip;

		float BlendIn = 1000.0f;
		float BlendOut = -8.0f;
		int Flags = 0;
		int RagdollBlockingFlags = 0;
		float MoverBlendDelta = 0.0f;
		int IkFlags = 0;
		Any PropFlags = 0;
		float PlaybackRate = 1000.0f;
		bool LoopUntilInteract = false;
		PlayAnimSettings PlayAnim;
		bool PauseOnEnd = true;
	};

	using SpawnPersistentPropsFn = void(*)(InteractableInstance&);
	using OnStageStartFn = void(*)(InteractableInstance&, size_t stageIndex);
	using GetNextStageFn = size_t(*)(InteractableInstance&, size_t currentStage);
	using CustomCleanupFn = void(*)(InteractableInstance&);
	using OnStageTickFn = void(*)(InteractableInstance&, size_t stageIndex);
	using ResolveParticipantsFn = void(*)(InteractableInstance&, size_t stageIndex, std::vector<SceneParticipant>& participants);
	using OnIdleTickFn = void(*)(InteractableInstance&);

	struct InteractableDefinition
	{
		std::string Label;
		Hash PropModel;
		std::vector<Hash> PropModelVariants;
		std::vector<ExtraPropDef> ExtraProps;
		std::string AnimDict;
		std::vector<InteractableStage> Stages;
		BagOpenState PlayerBagStateAfter = BagOpenState::NoChange;

		bool DeletePropsOnComplete = true;
		std::vector<PropEndState> FinalPropStates;

		Vector3 PlayerStandOffset = Vector3(0.0f, -1.0f, 0.0f);
		float PlayerHeadingOffset = 0.0f;
		float InteractRange = 0.50f;

		SpawnPersistentPropsFn SpawnPersistentProps = nullptr;
		OnStageStartFn OnStageStart = nullptr;
		GetNextStageFn GetNextStage = nullptr;
		CustomCleanupFn CustomCleanup = nullptr;
		OnStageTickFn OnStageTick = nullptr;
		bool Repeatable = false;
		bool RepositionPlayer = true;
		PlayerAnimMethod PlayerAnim = PlayerAnimMethod::SynchronizedScene;
		ResolveParticipantsFn ResolveParticipants = nullptr;
		OnIdleTickFn OnIdleTick = nullptr;

		Vector3 SceneOriginOffset = Vector3(0.0f, 0.0f, 0.0f);
		float SceneHeadingOffset = 0.0f;
		ApproachMethod Approach = ApproachMethod::Teleport;
		float ApproachSpeed = 8.0f;
		float ApproachZOffset = 0.0f;
		int ApproachTimeoutMs = 5000;
		bool PlayerAlsoPlaysEntityAnim = false;
		bool UnarmedDuringScene = false;
		bool PrimaryPropKeepsOwnOrigin = false;
		Hash SceneWeapon = 0;
	};

	struct ExtraPropRuntime
	{
		Vector3 FrozenPos;
		Vector3 FrozenRot;
		bool Captured = false;
	};

	struct InteractableInstance
	{
		SpoonerEntity Ent;
		std::vector<SpoonerEntity> ExtraEnts;
		std::vector<ExtraPropRuntime> ExtraRuntime;
		const InteractableDefinition* Def = nullptr;
		bool bActivated = false;
		bool bCleanupDone = false;
		int SyncedScene = -1;
		bool bRagdollDisabled = false;
		DWORD ActivationTime = 0;
		size_t CurrentStage = 0;
		Vector3 AnchorPos;
		Vector3 AnchorRot;
		int WornBagSlot = -1;
		int WornBagDrawable = -1;
		int WornBagTexture = -1;
		int OverheatCount = 0;
		bool bTreasurePickedUp = false;
		bool bHasPlayedInteractSound = false;
		size_t ResumeStage = 0;
		bool bAnchorLocked = false;
		int FuseBlownMask = 0;
		DWORD BreakerSettleStart = 0;

		InteractPhase Phase = InteractPhase::Idle;
		DWORD ApproachStartTime = 0;
		Hash PreviousWeapon = 0;
		bool bWeaponHolstered = false;
	};

	struct KnownBagVariant
	{
		int Slot;
		int DrawableId;
		int TextureId;
		Hash PropModel;
	};

	struct BagDrawablePair
	{
		int Slot;
		int ClosedDrawableId;
		int OpenedDrawableId;
	};

	// ========================================================================
	//  3. BUILDERS - labeled, chainable way to write definitions.
	//     Only write what differs from the defaults in section 2.
	// ========================================================================
	class ExtraProp
	{
	public:
		ExtraProp(const char* label, Hash model) { d.Label = label; d.PropModel = model; }

		ExtraProp& MatchWornBag() { d.MatchWornComponent = true; return *this; }
		ExtraProp& AttachToPlayer(AttachBone bone = AttachBone::Default)
		{
			d.AttachTarget = ExtraPropAttachTarget::Player;
			d.BoneOverride = bone;
			return *this;
		}
		ExtraProp& AttachToPrimaryProp(Vector3 offset = Vector3(), Vector3 rotation = Vector3())
		{
			d.AttachTarget = ExtraPropAttachTarget::PrimaryProp;
			d.AttachOffset = offset;
			d.AttachRotation = rotation;
			return *this;
		}
		ExtraProp& StartsHidden() { d.StartInvisible = true; return *this; }
		ExtraProp& SpawnsAtPlacement() { d.SpawnAtPlacement = true; return *this; }
		ExtraProp& SpawnedByHook() { d.ManualSpawn = true; return *this; }
		ExtraProp& PinnedUntilActivated() { d.FreezeAtSpawn = true; return *this; }
		ExtraProp& DeleteWhenDone() { d.DeleteOnComplete = true; return *this; }
		ExtraProp& KeepWhenDone() { d.DeleteOnComplete = false; return *this; }
		ExtraProp& RandomExtraModel(std::initializer_list<Hash> models) { d.PropModelVariants.assign(models); return *this; }

		ExtraPropDef Build() const { return d; }
	private:
		ExtraPropDef d;
	};

	class Stage
	{
	public:
		explicit Stage(int maxDurationMs) { s.MaxDurationMs = maxDurationMs; }

		Stage& PlayerClip(const char* clip) { s.Participants.push_back({ AnimTarget::Player, clip, 0 }); return *this; }
		Stage& PrimaryPropClip(const char* clip) { s.Participants.push_back({ AnimTarget::PrimaryProp, clip, 0 }); return *this; }
		Stage& ExtraPropClip(int extraPropIndex, const char* clip)
		{
			s.Participants.push_back({ AnimTarget::ExtraProp, clip, extraPropIndex });
			return *this;
		}
		Stage& PlayerWeaponClip(const char* clip) { s.Participants.push_back({ AnimTarget::PlayerWeapon, clip, 0 }); return *this; }
		Stage& Facial(const char* clip) { s.FacialClip = clip; return *this; }
		Stage& SceneRate(float rate) { s.SceneRate = rate; return *this; }
		Stage& Blend(float blendIn, float blendOut) { s.BlendIn = blendIn; s.BlendOut = blendOut; return *this; }
		Stage& LoopsUntilInteract() { s.LoopUntilInteract = true; return *this; }
		Stage& PauseOnEnd() { s.PauseOnEnd = true;  return *this; }
		Stage& ReleaseOnEnd() { s.PauseOnEnd = false; return *this; }
		Stage& PlayerUsesTaskPlayAnim(int flag, int durationMs = -1, float playbackRate = 0.0f)
		{
			s.PlayAnim.Flag = flag;
			s.PlayAnim.DurationMs = durationMs;
			s.PlayAnim.PlaybackRate = playbackRate;
			return *this;
		}

		InteractableStage Build() const { return s; }
	private:
		InteractableStage s;
	};

	class EndState
	{
	public:
		static EndState Primary() { return EndState(AnimTarget::PrimaryProp, 0); }
		static EndState Extra(int extraIndex) { return EndState(AnimTarget::ExtraProp, extraIndex); }

		EndState& Alpha(int alpha0to255) { e.Alpha = alpha0to255; return *this; }
		EndState& Collision(CollisionState c) { e.CollisionEnabled = (c == CollisionState::On); return *this; }
		EndState& Visibility(VisibilityState v) { e.Visible = (v == VisibilityState::Shown); return *this; }

		PropEndState Build() const { return e; }
	private:
		EndState(AnimTarget target, int extraIndex) { e.Target = target; e.ExtraPropIndex = extraIndex; }
		PropEndState e;
	};

	class Interactable
	{
	public:
		Interactable(const char* label, Hash primaryModel) { d.Label = label; d.PropModel = primaryModel; }

		Interactable& RandomPrimaryModel(std::initializer_list<Hash> models) { d.PropModelVariants.assign(models); return *this; }
		Interactable& AnimDict(const char* dict) { d.AnimDict = dict; return *this; }
		Interactable& AddExtraProp(const ExtraProp& extra) { d.ExtraProps.push_back(extra.Build()); return *this; }
		Interactable& AddExtraProps(const std::vector<ExtraPropDef>& extras)
		{
			d.ExtraProps.insert(d.ExtraProps.end(), extras.begin(), extras.end());
			return *this;
		}
		Interactable& AddStage(const Stage& stage) { d.Stages.push_back(stage.Build()); return *this; }
		Interactable& AddStages(const std::vector<InteractableStage>& stages)
		{
			d.Stages.insert(d.Stages.end(), stages.begin(), stages.end());
			return *this;
		}

		Interactable& DeletePropsWhenDone() { d.DeletePropsOnComplete = true; return *this; }
		Interactable& KeepPropsWhenDone() { d.DeletePropsOnComplete = false; return *this; }
		Interactable& AddEndState(const EndState& end) { d.FinalPropStates.push_back(end.Build()); return *this; }
		Interactable& PlayerBagAfter(BagOpenState state) { d.PlayerBagStateAfter = state; return *this; }

		Interactable& StandOffset(float x, float y, float z) { d.PlayerStandOffset = Vector3(x, y, z); return *this; }
		Interactable& StandHeading(float degrees) { d.PlayerHeadingOffset = degrees; return *this; }
		Interactable& InteractRange(float meters) { d.InteractRange = meters; return *this; }

		Interactable& Repeatable() { d.Repeatable = true; return *this; }
		Interactable& KeepPlayerWhereTheyStand() { d.RepositionPlayer = false; return *this; }
		Interactable& PlayerUsesTaskPlayAnim() { d.PlayerAnim = PlayerAnimMethod::TaskPlayAnim; return *this; }

		Interactable& OnSpawn(SpawnPersistentPropsFn fn) { d.SpawnPersistentProps = fn; return *this; }
		Interactable& OnStageStart(OnStageStartFn fn) { d.OnStageStart = fn; return *this; }
		Interactable& PickNextStage(GetNextStageFn fn) { d.GetNextStage = fn; return *this; }
		Interactable& OnCleanup(CustomCleanupFn fn) { d.CustomCleanup = fn; return *this; }
		Interactable& OnStageTick(OnStageTickFn fn) { d.OnStageTick = fn; return *this; }
		Interactable& OnIdleTick(OnIdleTickFn fn) { d.OnIdleTick = fn; return *this; }
		Interactable& ResolveParticipants(ResolveParticipantsFn fn) { d.ResolveParticipants = fn; return *this; }

		Interactable& SceneOrigin(float x, float y, float z) { d.SceneOriginOffset = Vector3(x, y, z); return *this; }
		Interactable& SceneHeading(float degrees) { d.SceneHeadingOffset = degrees; return *this; }
		Interactable& SlideToMark(float speed = 8.0f, float zOffset = 0.0f)
		{
			d.Approach = ApproachMethod::SlideToMark;
			d.ApproachSpeed = speed;
			d.ApproachZOffset = zOffset;
			return *this;
		}
		Interactable& SlideToClipStart(float speed = 8.0f, float zOffset = 0.0f)
		{
			d.Approach = ApproachMethod::SlideToClipStart;
			d.ApproachSpeed = speed;
			d.ApproachZOffset = zOffset;
			return *this;
		}
		Interactable& PlayerAlsoPlaysEntityAnim() { d.PlayerAlsoPlaysEntityAnim = true; return *this; }
		Interactable& UnarmedDuringScene() { d.UnarmedDuringScene = true; return *this; }
		Interactable& PrimaryPropIgnoresSceneOrigin() { d.PrimaryPropKeepsOwnOrigin = true; return *this; }

		Interactable& EquipDuringScene(Hash weapon) { d.SceneWeapon = weapon; return *this; }

		InteractableDefinition Build() const { return d; }
		operator InteractableDefinition() const { return d; }
	private:
		InteractableDefinition d;
	};

	// ========================================================================
	//  4. PUBLIC API (names the debug submenu relies on)
	// ========================================================================
	extern std::vector<InteractableDefinition> vInteractables;
	extern std::vector<InteractableInstance> vActiveInteractables;

	SpoonerEntity SpawnInteractable(const InteractableDefinition& def);
	Vector3 GetInteractPosition(const InteractableInstance& inst);
	void Tick();
}