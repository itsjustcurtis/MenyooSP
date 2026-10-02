#pragma once
#include <string>
#include <vector>
#include "Natives/natives.h"
#include "Natives/natives2.h"
#include "Util/GTAmath.h"
#include "Submenus/Spooner/SpoonerEntity.h"

namespace sub::Spooner::Interactables
{
	enum class AnimTarget { Player, PrimaryProp, ExtraProp };
	enum class BagOpenState { NoChange, Opened, Closed };
	enum class ExtraPropAttachTarget { Player, PrimaryProp };
	enum class AttachBone { Default, LeftHand, RightHand };

	struct InteractableInstance; // forward declaration - needed by the function-pointer typedef below

	using SpawnPersistentPropsFn = void(*)(InteractableInstance&);

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
		bool SpawnAtPlacement = false; // marker only - actual spawning now happens via SpawnPersistentPropsFn
		AttachBone BoneOverride = AttachBone::Default;
		bool DeleteOnComplete = true;
		bool FreezeAtSpawn = false;
		Vector3 AttachOffset = Vector3();
		Vector3 AttachRotation = Vector3();
	};

	struct PropEndState
	{
		AnimTarget Target;
		int ExtraPropIndex = 0;
		int Alpha = 255;
		bool CollisionEnabled = true;
		bool Visible = true;
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
	};

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

		SpawnPersistentPropsFn SpawnPersistentProps = nullptr; // NEW - set only by interactables with placement-time props
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

	extern std::vector<InteractableDefinition> vInteractables;
	extern std::vector<InteractableInstance> vActiveInteractables;

	SpoonerEntity SpawnInteractable(const InteractableDefinition& def);
	Vector3 GetInteractPosition(const InteractableInstance& inst);
	void Tick();
}