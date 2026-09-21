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

	struct SceneParticipant
	{
		AnimTarget Target;
		std::string ClipName;
		int ExtraPropIndex = 0; // only used when Target == ExtraProp
	};

	struct ExtraPropDef
	{
		std::string Label;
		Hash PropModel;
	};

	struct InteractableStage
	{
		std::vector<SceneParticipant> Participants;
		int MaxDurationMs = 6000;
		float SceneRate = 1.0f;
		std::string FacialClip;

		float BlendIn = 1000.0f;   // p4 for props, speed for player - 1000.0f is the documented "instant" convention
		float BlendOut = -8.0f;
		int Flags = 0;
		int RagdollBlockingFlags = 0;
		float MoverBlendDelta = 0.0f;
		int IkFlags = 0;
		Any PropFlags = 0;
		float PlaybackRate = 1000.0f; // p7 - the missing half of the convention, was stuck at 1.0f this whole session
	};

	struct InteractableDefinition
	{
		std::string Label;
		Hash PropModel;
		std::vector<ExtraPropDef> ExtraProps;
		std::string AnimDict;
		std::vector<InteractableStage> Stages;
	};

	struct InteractableInstance
	{
		SpoonerEntity Ent;
		std::vector<SpoonerEntity> ExtraEnts;
		const InteractableDefinition* Def = nullptr;
		bool bActivated = false;
		int SyncedScene = -1;
		bool bRagdollDisabled = false;
		DWORD ActivationTime = 0;
		size_t CurrentStage = 0;
		Vector3 AnchorPos;
		Vector3 AnchorRot;
	};

	extern std::vector<InteractableDefinition> vInteractables;
	extern std::vector<InteractableInstance> vActiveInteractables;

	constexpr float InteractRange = 2.5f;

	SpoonerEntity SpawnInteractable(const InteractableDefinition& def);
	void Tick();
}