/*
* Menyoo PC - Grand Theft Auto V single-player trainer mod
* Copyright (C) 2019  MAFINS
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/
#include "Credits.h"
#include "MainMenu.h"

#include "..\Util\ExePath.h"

#include <string>
#include <vector>
#include <windows.h>
#include <shellapi.h>
#include <pugixml/src/pugixml.hpp>

#pragma comment(lib, "shell32.lib")

namespace sub
{
    struct CreditsTier
    {
        std::string name;
        std::vector<std::string> members;
    };

    static std::vector<CreditsTier> g_CreditsTiers;
    static bool g_CreditsLoaded = false;

    static void LoadCredits()
    {
        addlog(ige::LogType::LOG_DEBUG, "Loading credits from XML...");
        g_CreditsTiers.clear();

        pugi::xml_document doc;
        std::string path = GetPathffA(Pathff::Main, true) + "Supporters.xml";

        addlog(ige::LogType::LOG_TRACE, "Credits path: " + path);

        pugi::xml_parse_result result = doc.load_file(path.c_str());
        if (result.status != pugi::status_ok)
        {
            addlog(ige::LogType::LOG_ERROR, "Credits XML load failed: " + std::string(result.description()));
            return;
        }

        if (doc.load_file(path.c_str()).status != pugi::status_ok)
            return;

        pugi::xml_node root = doc.child("Credits");
        if (!root)
            return;

        for (pugi::xml_node tierNode : root.children("Tier"))
        {
            CreditsTier tier;
            tier.name = tierNode.attribute("name").as_string();
            addlog(ige::LogType::LOG_TRACE, "Processing credits tier: " + tier.name);
            for (pugi::xml_node memberNode : tierNode.children("Member"))
            {
                std::string member = memberNode.text().as_string();
                addlog(ige::LogType::LOG_TRACE, "Processing credits member: " + member);
                if (!member.empty())
                    tier.members.push_back(member);
            }

            if (!tier.members.empty())
                g_CreditsTiers.push_back(tier);
        }

        g_CreditsLoaded = true;
    }

    void CreditsMenu()
    {
        AddTitle("Credits");

        if (!g_CreditsLoaded)
            LoadCredits();

        bool openPatreon = false;
        AddOption("Support Menyoo on Patreon", openPatreon);
        if (openPatreon)
        {
            ShellExecuteA(
                nullptr,
                "open",
                "https://www.patreon.com/cw/ItsJustCurtis",
                nullptr,
                nullptr,
                SW_SHOWNORMAL
            );
        }

        if (g_CreditsTiers.empty())
        {
            bool dummy = false;
            AddOption("No credits found - Check Supporters.xml from your download", null);
            return;
        }

        for (const auto& tier : g_CreditsTiers)
        {
            AddBreak(tier.name);
            for (const auto& member : tier.members)
            {
                bool dummy = false;
                AddOption(member, null);
            }
        }
    }
}

#include "..\Menu\submenu_switch.h"
#include "..\Menu\submenu_enum.h"
REGISTER_SUBMENU(CREDITSSUB, sub::CreditsMenu)