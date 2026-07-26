/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

This file contains code for handling collapsible menu categories in the menu system. 
It was designed to be as easy to use as possible and can be easily integrated into any menu. It also takes inspiration from how ImGui handles context menus and collapsible headers.

* Main functions that you should be using are:


- AddCategory(label): Adds a category header to the menu. Clicking the header (ENTER) will expand or collapse the category.
	Should be used like
		if (AddCategory(label))
		{
			// Add options for this category here}
		}


- AddCategoryOption(text, pressed): Adds an option under a category. If the user presses left/right while this option is selected, it will jump to the previous/next category.
	YOU DO NOT NEED TO NECESARILY USE THIS FUNCTION for category members. 
	You can use AddOption(), AddTexter, AddNumber or whatever - but if you want to have the left/right jump to next category behavior, use this function or create a custom one (take a look at AddAnimOption).


- JumpToAdjacentCategory(currentCategory): Jumps to the previous/next category header. You can use this in your own custom option functions if you want to have the left/right jump behavior.
*/

#include "MenuCategory.h"
#include "Menu.h"
#include "..\Util\keyboard.h"
#include "..\Natives\natives2.h"

#include <vector>
#include <map>

// Per-frame state
static std::vector<int> s_headerPositions;
static std::map<std::string, size_t> s_categoryIndexMap;
static std::string s_currentCategory;

// Persistent expanded state, keyed on "currentsub:label"
static std::map<std::string, bool> s_expandedState;
static std::map<std::string, bool> s_expandedBackup;

static std::string MakeKey(const std::string& label)
{
	return std::to_string(Menu::currentsub) + ":" + label;
}

namespace MenuCategory
{
	void ResetCategoryState()
	{
		s_headerPositions.clear();
		s_categoryIndexMap.clear();
		s_currentCategory.clear();
	}

	bool AddCategory(const std::string& label, bool defaultExpanded)
	{
		s_currentCategory = label;

		std::string key = MakeKey(label);
		if (s_expandedState.find(key) == s_expandedState.end())
			s_expandedState[key] = defaultExpanded;
		bool& expanded = s_expandedState[key];

		size_t idx = s_headerPositions.size();
		s_categoryIndexMap[label] = idx;

		bool catPressed = false;
		AddTickol(label, expanded, catPressed, catPressed, TICKOL::ARROWRIGHT, TICKOL::ARROWRIGHT, false, 270.0f, 90.0f);
		if (catPressed)
			expanded = !expanded;
		s_headerPositions.push_back(Menu::printingop);

		if (Menu::printingop == *Menu::currentopATM && s_headerPositions.size() > 1)
		{
			if (Menu::bitController)
			{
				if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_LEFT))
					*Menu::currentopATM = (idx > 0) ? s_headerPositions[idx - 1] : s_headerPositions.back();
				if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_RIGHT))
					*Menu::currentopATM = (idx < s_headerPositions.size() - 1) ? s_headerPositions[idx + 1] : s_headerPositions.front();
			}
			else
			{
				if (IsKeyJustUp(VirtualKey::Left))
					*Menu::currentopATM = (idx > 0) ? s_headerPositions[idx - 1] : s_headerPositions.back();
				if (IsKeyJustUp(VirtualKey::Right))
					*Menu::currentopATM = (idx < s_headerPositions.size() - 1) ? s_headerPositions[idx + 1] : s_headerPositions.front();
			}
		}

		return expanded;
	}

	void ExpandAll()
	{
		std::string prefix = std::to_string(Menu::currentsub) + ":";
		bool alreadyBackedUp = !s_expandedBackup.empty();
		for (auto& [key, val] : s_expandedState)
		{
			if (key.substr(0, prefix.size()) == prefix)
			{
				if (!alreadyBackedUp)
					s_expandedBackup[key] = val;
				val = true;
			}
		}
	}

	void RestoreExpandedState()
	{
		if (s_expandedBackup.empty())
			return;
		std::string prefix = std::to_string(Menu::currentsub) + ":";
		for (auto& [key, val] : s_expandedBackup)
		{
			if (key.substr(0, prefix.size()) == prefix)
				s_expandedState[key] = val;
		}
		s_expandedBackup.clear();
	}

	bool AddCategoryOption(const std::string& text, bool& pressed)
	{
		AddOption(text, pressed);
		JumpToAdjacentCategory(s_currentCategory);
		return pressed;
	}

	void JumpToAdjacentCategory(const std::string& currentCategory)
	{
		if (s_headerPositions.empty())
			return;

		auto it = s_categoryIndexMap.find(currentCategory);
		if (it == s_categoryIndexMap.end())
			return;

		size_t idx = it->second;
		if (Menu::bitController)
		{
			if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_LEFT))
				*Menu::currentopATM = (idx > 0) ? s_headerPositions[idx - 1] : s_headerPositions.back();
			if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_FRONTEND_RIGHT))
				*Menu::currentopATM = (idx < s_headerPositions.size() - 1) ? s_headerPositions[idx + 1] : s_headerPositions.front();
		}
		else
		{
			if (IsKeyJustUp(VirtualKey::Left))
				*Menu::currentopATM = (idx > 0) ? s_headerPositions[idx - 1] : s_headerPositions.back();
			if (IsKeyJustUp(VirtualKey::Right))
				*Menu::currentopATM = (idx < s_headerPositions.size() - 1) ? s_headerPositions[idx + 1] : s_headerPositions.front();
		}
	}

	bool HasMultipleCategories()
	{
		return s_headerPositions.size() > 1;
	}
}