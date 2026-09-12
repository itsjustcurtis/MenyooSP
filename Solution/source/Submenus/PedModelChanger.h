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

#include <string>

namespace GTAmodel 
{
	class Model;
}

namespace sub
{
	namespace PedFavourites
	{
		extern std::string xmlFavouritePeds;
		bool IsPedAFavourite(GTAmodel::Model model);
		bool AddPedToFavourites(GTAmodel::Model model, const std::string& customName);
		bool RemovePedFromFavourites(GTAmodel::Model model);
		void ShowInstructionalButton(GTAmodel::Model model);
		void PedFavouritesMenu();
	}

	void ChangeModel(GTAmodel::Model model);
	void AddModelChangerOption(const std::string& text, const GTAmodel::Model& model, int tickTrue = 1);
	void AddModelOption(const std::string& text, const GTAmodel::Model& model, bool *extra_option_code = nullptr, int tickTrue = 1);

	void ModelChangerMenu();
	void ModelChangerPlayer();
	void ModelChangerAnimal();
	void ModelChangerAmbientFemale();
	void ModelChangerAmbientMale();
	void ModelChangerCutscene();
	void ModelChangerGangFemale();
	void ModelChangerGangMale();
	void ModelChangerStory();
	void ModelChangerMultiplayer();
	void ModelChangerScenarioFemale();
	void ModelChangerScenarioMale();
	void ModelChangerStoryScenarioFemale();
	void ModelChangerStoryScenarioMale();
	void ModelChangerOthers();
}