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

Every submenu that has at least two categories will automatically show a "Navigate Categories" IB (G on keyboard, X on controller) that will open a submenu that allows you to jump to any category in the current menu.

* Basically the only thing that you need to use is "AddCategory"; below is explanation on how to use it:
   AddCategory(label, defaultExpanded) adds a category header to the menu. Clicking the header (ENTER) will expand or collapse the category.
	Should be used like
		if (AddCategory(label))
		{
			// Add options for this category here. You can use AddOption, AddTickol, etc. as normal.
		}
	defaultExpanded is optional and defaults to true. If you want a category to be collapsed by default, pass false.

This file also exposes a few other functions that can be used to get information about the current categories in the menu, such as GetCategoryLabels and GetCategoryPositions.
These can be used to implement custom category navigation if desired. Take a look at sub::CategoryNavigate for an example of how to use these functions.

*/

#include "submenu_switch.h"
#include "submenu_enum.h"
#include "MenuCategory.h"
#include "Menu.h"
#include "..\Util\keyboard.h"
#include "..\Natives\natives2.h"

#include <vector>
#include <map>

// Per-frame state
static std::vector<int> categoryHeaderPositions;
static std::vector<std::string> categoryHeaderLabels;

// Persistent navigation target, set by CategoryNavigate SUB
static int pendingCategoryIndex = -1;

// Frame tracking for auto-clearing per-frame state
static DWORD s_lastFrameTick = 0;
static bool categoryNavigationHintAdded = false;

// Persistent expanded state, keyed on "activeSubmenu:label"
static std::map<std::string, bool> s_expandedState;
static std::map<std::string, bool> s_expandedBackup;

static std::string MakeKey(const std::string& label)
{
	return std::to_string(Menu::activeSubmenu) + ":" + label;
}

namespace MenuCategory
{
	void ResetCategoryState()
	{
		categoryHeaderPositions.clear();
		categoryHeaderLabels.clear();

		if (pendingCategoryIndex != -1)
		{
			*Menu::activeOptionIndex = pendingCategoryIndex;
			pendingCategoryIndex = -1;
		}
	}

	bool AddCategory(const std::string& label, bool defaultExpanded)
	{
		DWORD now = GetTickCount();
		if (now != s_lastFrameTick)
		{
			categoryHeaderPositions.clear();
			categoryHeaderLabels.clear();
			categoryNavigationHintAdded = false;
			s_lastFrameTick = now;

			if (pendingCategoryIndex != -1)
			{
				*Menu::activeOptionIndex = pendingCategoryIndex;
				pendingCategoryIndex = -1;
			}
		}

		std::string key = MakeKey(label);
		if (s_expandedState.find(key) == s_expandedState.end())
			s_expandedState[key] = defaultExpanded;
		bool& expanded = s_expandedState[key];

		bool catPressed = false;
		AddTickol(label, expanded, catPressed, catPressed, TICKOL::ARROWRIGHT, TICKOL::ARROWRIGHT, false, 270.0f, 90.0f);
		if (catPressed)
			expanded = !expanded;
		categoryHeaderPositions.push_back(Menu::currentOptionCount);
		categoryHeaderLabels.push_back(label);

		if (categoryHeaderPositions.size() > 1)
		{
			if (!categoryNavigationHintAdded)
			{
				if (Menu::usingControllerInput)
					Menu::add_IB(INPUT_SPECIAL_ABILITY, "Navigate categories"); // XBOX "l3" (left stick click)
				else
					Menu::add_IB(VirtualKey::G, "Navigate categories");
				categoryNavigationHintAdded = true;
			}

			if (Menu::usingControllerInput)
			{
				if (IS_DISABLED_CONTROL_JUST_PRESSED(2, INPUT_SPECIAL_ABILITY)) // XBOX "l3" (left stick click)
					Menu::pendingSubmenu = SUB::CATEGORYNAVIGATOR;
			}
			else
			{
				if (IsKeyJustUp(VirtualKey::G))
					Menu::pendingSubmenu = SUB::CATEGORYNAVIGATOR;
			}
		}

		return expanded;
	}

	void ExpandAll()
	{
		std::string prefix = std::to_string(Menu::activeSubmenu) + ":";
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
		std::string prefix = std::to_string(Menu::activeSubmenu) + ":";
		for (auto& [key, val] : s_expandedBackup)
		{
			if (key.substr(0, prefix.size()) == prefix)
				s_expandedState[key] = val;
		}
		s_expandedBackup.clear();
	}

	const std::vector<std::string>& GetCategoryLabels()
	{
		return categoryHeaderLabels;
	}

	const std::vector<int>& GetCategoryPositions()
	{
		return categoryHeaderPositions;
	}

	void JumpToCategory(size_t index)
	{
		if (index < categoryHeaderPositions.size())
			pendingCategoryIndex = categoryHeaderPositions[index];
	}
}

namespace sub
{
	void CategoryNavigate()
	{
		auto& labels = MenuCategory::GetCategoryLabels();
		auto& positions = MenuCategory::GetCategoryPositions();

		if (labels.empty())
		{
			Menu::SetPreviousMenu();
			return;
		}

		AddTitle("Jump to Category");

		for (size_t i = 0; i < labels.size(); i++)
		{
			bool pressed = false;
			AddOption(labels[i], pressed);
			if (pressed)
			{
				MenuCategory::JumpToCategory(i);
				Menu::SetPreviousMenu();
				return;
			}
		}
	}
}
REGISTER_SUBMENU(CATEGORYNAVIGATOR, sub::CategoryNavigate)
