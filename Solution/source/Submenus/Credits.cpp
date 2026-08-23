#include "Credits.h"
#include "MainMenu.h"

#include "..\Util\ExePath.h"

#include <string>
#include <vector>
#include <windows.h>
#include <winhttp.h>
#include <shellapi.h>
#include <fstream>
#include <json\single_include\nlohmann\json.hpp>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "winhttp.lib")

namespace sub
{
    struct CreditsTier
    {
        std::string name;
        std::vector<std::string> members;
    };

    static std::vector<CreditsTier> g_CreditsTiers;
    static bool g_CreditsLoaded = false;

    static const wchar_t* kApiHost = L"raw.githubusercontent.com";
    static const wchar_t* kApiPath = L"/itsjustcurtis/menyoo-API/main/data/supporters.json";

    // Pulls raw JSON from GitHub over HTTPS. Returns empty string on any failure.
    static std::string FetchSupportersJson()
    {
		addlog(ige::LogType::LOG_TRACE, "Fetching credits from API...");
        std::string result;

        HINTERNET hSession = WinHttpOpen(L"Menyoo/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession)
            return result;
		addlog(ige::LogType::LOG_TRACE, "Credits API session opened");

        HINTERNET hConnect = WinHttpConnect(hSession, kApiHost, INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (!hConnect)
        {
            WinHttpCloseHandle(hSession);
            return result;
        }
		addlog(ige::LogType::LOG_TRACE, "Credits API connection established");

        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", kApiPath,
            nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE);
        if (!hRequest)
        {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return result;
        }
		addlog(ige::LogType::LOG_TRACE, "Credits API request prepared");

        BOOL sent = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
        
		addlog(ige::LogType::LOG_DEBUG, "Credits API request sent");
        if (sent && WinHttpReceiveResponse(hRequest, nullptr))
        {
            DWORD statusCode = 0;
            DWORD statusSize = sizeof(statusCode);
            WinHttpQueryHeaders(hRequest,
                WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusSize, WINHTTP_NO_HEADER_INDEX);

            if (statusCode == 200)
            {
				addlog(ige::LogType::LOG_TRACE, "Credits API returned status 200 OK, reading data...");
                DWORD bytesAvailable = 0;
                while (WinHttpQueryDataAvailable(hRequest, &bytesAvailable) && bytesAvailable > 0)
                {
                    std::vector<char> buffer(bytesAvailable);
                    DWORD bytesRead = 0;
                    if (!WinHttpReadData(hRequest, buffer.data(), bytesAvailable, &bytesRead))
                        break;
                    result.append(buffer.data(), bytesRead);
                }
            }
            else
            {
                addlog(ige::LogType::LOG_ERROR, "Credits API returned status " + std::to_string(statusCode));
            }
        }
        else
        {
            addlog(ige::LogType::LOG_ERROR, "Credits API request failed: " + std::to_string(GetLastError()));
        }

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return result;
    }

    static std::string GetCacheFilePath()
    {
        return GetPathffA(Pathff::Main, true) + "Supporters.json";
    }

    static void ParseSupportersJson(const std::string& raw)
    {
        g_CreditsTiers.clear();

        nlohmann::json j = nlohmann::json::parse(raw, nullptr, false);
        if (j.is_discarded() || !j.contains("tiers"))
        {
            addlog(ige::LogType::LOG_ERROR, "Credits JSON parse failed");
            return;
        }

        for (const auto& tierNode : j["tiers"])
        {
            CreditsTier tier;
            tier.name = tierNode.value("name", "");

            if (tier.name == "Contributor")
            {
                tier.members.push_back("MAFINS");
                tier.members.push_back("ItsJustCurtis");
            }

            for (const auto& member : tierNode.value("members", std::vector<std::string>{}))
            {
                if (!member.empty())
                    tier.members.push_back(member);
            }

            if (!tier.members.empty())
                g_CreditsTiers.push_back(tier);
        }
    }

    static void LoadCredits()
    {
        addlog(ige::LogType::LOG_DEBUG, "Loading credits from API...");

        std::string raw = FetchSupportersJson();

        if (!raw.empty())
        {
            // Cache the fresh copy for offline fallback next time.
            std::ofstream cacheOut(GetCacheFilePath(), std::ios::binary | std::ios::trunc);
            if (cacheOut)
                cacheOut << raw;
        }
        else
        {
            addlog(ige::LogType::LOG_ERROR, "Credits API fetch failed, falling back to cache");
            std::ifstream cacheIn(GetCacheFilePath(), std::ios::binary);
            if (cacheIn)
            {
                raw.assign((std::istreambuf_iterator<char>(cacheIn)), std::istreambuf_iterator<char>());
            }
        }

        if (!raw.empty())
            ParseSupportersJson(raw);

        g_CreditsLoaded = true;
    }

    void CreditsMenu()
    {
        AddTitle("Credits");

        if (!g_CreditsLoaded)
            LoadCredits();

        bool openPatreon = false;
        AddOption("Support Menyoo on Patreon", openPatreon, nullFunc, -1, true);
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
            AddOption("No credits found - Check your connection", null);
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