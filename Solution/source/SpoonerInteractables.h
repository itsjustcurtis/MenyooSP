#pragma once
#include <string>
#include <vector>
#include "..\..\Natives\natives.h"
#include "..\..\Natives\natives2.h"
#include "..\..\Util\GTAmath.h"
#include "Submenus/Spooner/SpoonerEntity.h"

namespace sub::Spooner::Interactables
{
	enum class AnimTarget { Prop, Player };

	struct SceneParticipant
	{
		AnimTarget Target;
		std::string ClipName;
		float BlendIn = 8.0f;
		float BlendOut = -8.0f;

		// Only used when Target == Player (TASK_SYNCHRONIZED_SCENE)
		int Flags = 0;
		int RagdollBlockingFlags = 0;
		float MoverBlendDelta = 0.0f;
		int IkFlags = 0;

		// Only used when Target == Prop (PLAY_SYNCHRONIZED_ENTITY_ANIM)
		Any PropFlags = 0;
		float PlaybackRate = 1.0f;
	};

	struct InteractableStage
	{
		std::vector<SceneParticipant> Participants;
		int MaxDurationMs = 6000;
		float SceneRate = 1.0f;
	};

	struct InteractableDefinition
	{
		std::string Label;
		Hash PropModel;
		std::string AnimDict;
		std::vector<InteractableStage> Stages;
	};

	struct InteractableInstance
	{
		SpoonerEntity Ent;
		const InteractableDefinition* Def = nullptr;
		bool bActivated = false;
		int SyncedScene = -1;
		bool bRagdollDisabled = false;
		DWORD ActivationTime = 0;
		size_t CurrentStage = 0;
	};

	extern std::vector<InteractableDefinition> vInteractables;
	extern std::vector<InteractableInstance> vActiveInteractables;

	constexpr float InteractRange = 2.5f;

	SpoonerEntity SpawnInteractable(const InteractableDefinition& def);
	void Tick();
}