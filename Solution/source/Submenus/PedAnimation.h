/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#pragma once

#include <vector>
#include <string>
#include <map>

class GTAentity;

namespace sub
{
	namespace AnimationMenu
	{
		struct NamedAnimation { std::string caption; std::string animDict, animName; };
		extern const std::vector<AnimationMenu::NamedAnimation> presetPedAnims;
		extern std::map<std::string, std::vector<std::string>> allPedAnims;
		extern std::pair<const std::string, std::vector<std::string>>* selectedAnimDictPtr;

		void PopulateAllPedAnimsList();
		void AllPedAnimsMenu();
		void Sub_AllPedAnims_InDict();
	}

	void GetFavouriteAnimations(std::vector<std::pair<std::string, std::string>>& result);
	bool IsAnimationAFavourite(const std::string animDict, const std::string& animName);
	void AddAnimationToFavourites(const std::string animDict, const std::string& animName);
	void RemoveAnimationFromFavourites(const std::string animDict, const std::string& animName);
	void AnimationStopAnimationCallback();
	void PedAnimationMenu();
	void AnimationSub_Settings();
	void AnimationFavouritesMenu();
	void AnimationSub_Custom();
	void DeerAnimationMenu();
	void SharkAnimationMenu();
	void MissionRappelAnimationMenu();
	void GestureSitAnimationMenu();
	void SwatAnimationMenu();
	void GuardReactAnimationMenu();
	void RandomArrestAnimationMenu();

	namespace AnimationTaskScenarios
	{
		extern std::vector<std::string> vValues_TaskScenarios;
		struct NamedScenario { std::string name; std::string label; };
		extern std::vector<NamedScenario> vNamedScenarios;

		void AnimationTaskScenarios1();
		void AnimationTaskScenarios2();

	}

	std::string GetPedMovementClipSet(const GTAentity& ped);
	void SetPedMovementClipSet(GTAentity ped, const std::string& setName);
	std::string GetPedWeaponMovementClipSet(const GTAentity& ped);
	void SetPedWeaponMovementClipSet(GTAentity ped, const std::string& setName);
	void MovementGroupMenu();

	namespace FacialAnims
	{
		struct NamedFacialAnim { std::string caption; std::string animName; };
		extern const std::vector<NamedFacialAnim> vFacialAnims;

		void FacialMoodMenu();
	}

}