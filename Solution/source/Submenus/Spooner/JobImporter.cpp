#include "JobImporter.h"
#include "Submenus.h"
#include <thread>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <atomic>
#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#include <json\single_include\nlohmann\json.hpp>
#include <pugixml/src/pugixml.hpp>

#include "..\..\Util\ExePath.h"
#include "..\..\Util\FileLogger.h"
#include "..\..\Menu\Menu.h"
#include "..\..\Scripting\Game.h"
#include "..\..\Scripting\DxHookIMG.h"
#include "..\..\Scripting\ModelNames.h"
#include "..\..\Scripting\Model.h"
#include "..\..\Util\GTAmath.h"
#include "..\..\Natives\natives2.h"
#include "..\..\Natives\types.h"
#include "FileManagement.h"

using json = nlohmann::json;

// ============================================================================
// JOB BROWSER
// ============================================================================
namespace sub::Spooner::JobBrowser
{
    std::vector<BrowserJobResult> results;
    int selectedResultIndex = -1;
    bool isSearching = false;
    std::string searchStatus = "";

    bool isDownloading = false;
    std::string downloadStatus = "";
    static std::atomic<bool> pendingSavedNotification{ false };

    enum class QueryState {
        Idle,
        Cancelling,
        Requesting,
        Polling,
        Success,
        Failed
    };
    QueryState queryState = QueryState::Idle;
    DWORD queryTimer = 0;
    static Any ugcQueryVar = 0;

    static int GetQueryType() //there is a couple more that could add later
    {
        if (Menu::activeSubmenu == (int)SUB::SPOONER_JOBBROWSER) return 0; // Bookmarked
        if (Menu::activeSubmenu == (int)SUB::SPOONER_JOBBROWSER_MYCONTENT) return 1; // My Content
        if (Menu::activeSubmenu == (int)SUB::SPOONER_JOBBROWSER_FRIENDCONTENT) return 2; // Friend Content
        if (Menu::activeSubmenu == (int)SUB::SPOONER_JOBBROWSER_MOSTRECENT) return 3; // Most Recent images don't seem to load with this one but maybe cause too many
        if (Menu::activeSubmenu == (int)SUB::SPOONER_JOBBROWSER_TOPRATED) return 4; // Top Rated
        if (Menu::activeSubmenu == (int)SUB::SPOONER_JOBBROWSER_CREWCONTENT) return 5; // Crew Content
        return 0;
    }

    static std::string GetQueryTitle()
    {
        int q = GetQueryType();
        switch (q)
        {
            case 0: return "Bookmarked Jobs";
            case 1: return "My Content";
            case 2: return "Friend Content";
            case 3: return "Most Recent";
            case 4: return "Top Rated";
            case 5: return "Crew Content";
        }
        return "Job Browser";
    }

    std::map<std::string, DxHookIMG::DxTexture> thumbCache;
    bool thumbDownloadStarted = false;

    static std::string GetThumbFolder()
    {
        std::string folder = GetPathffA(Pathff::Main, false) + "\\Job Thumbnails";
        return folder;
    }

    static void DrawPreviewImage(DxHookIMG::DxTexture& tex)
    {
        Vector2 res = { 0.1f, 0.0889f };
        FLOAT x_coord = 0.324f + menuPos.x;
        FLOAT y_coord = currentOptionY + 0.044f + menuPos.y;
        if (menuPos.x > 0.45f) x_coord = menuPos.x - 0.003f;
        DRAW_RECT(x_coord, y_coord, res.x + 0.003f, res.y + 0.003f, 0, 0, 0, 212, false);
        tex.Draw(0, Vector2(x_coord, y_coord), Vector2(res.x, res.y / 2 + 0.005f), 0.0f, RGBA::AllWhite());
    }

    static std::string HttpGet(const std::string& url, bool& success) {
        success = false;
        std::string response;
        HINTERNET hInternet = InternetOpenA("MenyooSP", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (hInternet) {
            DWORD timeout = 4000;
            InternetSetOptionA(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
            InternetSetOptionA(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
            
            HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, 
                INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
            if (hConnect) {
                DWORD statusCode = 0;
                DWORD statusCodeSize = sizeof(statusCode);
                if (HttpQueryInfoA(hConnect, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusCodeSize, NULL)) {
                    if (statusCode == 200) {
                        char buffer[8192];
                        DWORD bytesRead = 0;
                        while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
                            response.append(buffer, bytesRead);
                        }
                        success = true;
                    }
                }
                InternetCloseHandle(hConnect);
            }
            InternetCloseHandle(hInternet);
        }
        return response;
    }

    static std::string SanitizeFilename(std::string name) {
        std::string invalidChars = "\\/*?:\"<>|";
        name.erase(std::remove_if(name.begin(), name.end(), [&](char c) {
            return invalidChars.find(c) != std::string::npos;
        }), name.end());
        
        std::string punctuation = " -+.,;=";
        for (char& c : name) {
            if (punctuation.find(c) != std::string::npos) {
                c = '_';
            }
        }
        
        std::string clean;
        bool lastWasUnderscore = false;
        for (char c : name) {
            if (c == '_') {
                if (!lastWasUnderscore) {
                    clean += c;
                    lastWasUnderscore = true;
                }
            } else {
                clean += c;
                lastWasUnderscore = false;
            }
        }
        name = clean;
        
        if (name.length() > 100) {
            name = name.substr(0, 100);
        }
        
        while (!name.empty() && name.front() == '_') {
            name.erase(0, 1);
        }
        while (!name.empty() && name.back() == '_') {
            name.pop_back();
        }
        
        if (name.empty()) {
            name = "unnamed_job";
        }
        return name;
    }

    static void WriteFileBinary(const std::string& path, const std::string& data) {
        std::ofstream out(path, std::ios::out | std::ios::binary);
        if (out) {
            out.write(data.data(), data.size());
        }
    }

    void StartQuery()
    {
        results.clear();
        isSearching = true;
        searchStatus = "Cancelling pending queries...";
        
        UGC_CANCEL_QUERY();
        
        queryState = QueryState::Cancelling;
        queryTimer = GetTickCount();
    }

    void ReadQueryResults()
    {
        results.clear();
        int numResults = UGC_GET_CONTENT_NUM();
        int totalResults = UGC_GET_CONTENT_TOTAL();
        
        addlog(ige::LogType::LOG_INFO, "Job Browser UGC Bookmarks: Num=" + std::to_string(numResults) + ", Total=" + std::to_string(totalResults));
        
        if (numResults <= 0)
        {
            searchStatus = "~y~No bookmarks found.";
            isSearching = false;
            return;
        }

        for (int i = 0; i < numResults; ++i)
        {
            BrowserJobResult r;
            r.name = UGC_GET_CONTENT_NAME(i) ? UGC_GET_CONTENT_NAME(i) : "Unknown";
            r.cid = UGC_GET_CONTENT_ID(i) ? UGC_GET_CONTENT_ID(i) : "";
            r.creator = UGC_GET_CONTENT_USER_NAME(i) ? UGC_GET_CONTENT_USER_NAME(i) : "Unknown";
            r.descHash = UGC_GET_CONTENT_DESCRIPTION_HASH(i);
            r.cdnPath = UGC_GET_CONTENT_PATH(i, 2) ? UGC_GET_CONTENT_PATH(i, 2) : "";
            r.fileVersion = UGC_GET_CONTENT_FILE_VERSION(i, 0);
            r.rating = UGC_GET_CONTENT_RATING(i, 0);
            r.ratingCount = UGC_GET_CONTENT_RATING_COUNT(i, 0);
            r.verified = UGC_GET_CONTENT_IS_VERIFIED(i);
            r.langId = UGC_GET_CONTENT_LANGUAGE(i);
            
            addlog(ige::LogType::LOG_INFO, "Job Index " + std::to_string(i) + " (" + r.name + ") language ID: " + std::to_string(r.langId));
            
            if (!r.cid.empty())
            {
                r.cdnBase = "https://prod.cloud.rockstargames.com/ugc/gta5mission/" + r.cid;
            }
            
            results.push_back(r);
        }
        
        searchStatus = "~g~Found " + std::to_string(results.size()) + " jobs.";
        isSearching = false;

        if (sub::Spooner::JobImporter::savePreviewImage)
        {
            thumbDownloadStarted = true;
            std::string thumbFolder = GetThumbFolder();
            CreateDirectoryA(thumbFolder.c_str(), NULL);
            std::vector<BrowserJobResult> resultsCopy = results;
            std::thread([resultsCopy, thumbFolder]() {
            for (const auto& r : resultsCopy)
            {
                if (r.cid.empty()) continue;
                std::string thumbPath = thumbFolder + "\\" + r.cid + ".jpg";
                std::ifstream existing(thumbPath);
                if (existing.good()) continue;
                existing.close();
                
                if (!r.cdnBase.empty())
                {
                    bool ok = false;
                    std::string data = HttpGet(r.cdnBase + "/2_0.jpg", ok);
                    if (ok && data.size() > 100)
                    {
                        WriteFileBinary(thumbPath, data);
                        continue;
                    }
                }
                std::string directBase = "https://prod.cloud.rockstargames.com/ugc/gta5mission/" + r.cid;
                bool ok = false;
                std::string data = HttpGet(directBase + "/2_0.jpg", ok);
                if (ok && data.size() > 100)
                {
                    WriteFileBinary(thumbPath, data);
                }
            }
        }).detach();
        }
    }

    static void DownloadJobThread(BrowserJobResult result, std::vector<std::string> langs)
    {
        std::string jobMapsFolder = GetPathffA(Pathff::Spooner, false) + "\\!Imported Jobs";
        CreateDirectoryA(jobMapsFolder.c_str(), NULL);

        std::string basePath = result.cdnBase.empty() ? ("https://prod.cloud.rockstargames.com/ugc/gta5mission/" + result.cid) : result.cdnBase;

        bool found = false;
        std::vector<std::pair<int, int>> commonPatterns = { {0,0}, {0,1}, {1,0}, {2,0} };

        for (const auto& pattern : commonPatterns)
        {
            if (found) break;

            for (const auto& lang : langs)
            {
                if (found) break;

                std::string tryUrl = basePath + "/" + std::to_string(pattern.first) + "_" + std::to_string(pattern.second) + "_" + lang + ".json";
                downloadStatus = "Fetching...";

                bool success = false;
                std::string body = HttpGet(tryUrl, success);
                if (success && !body.empty() && body.front() == '{')
                {
                    std::string safeName = SanitizeFilename(result.name);
                    std::string savePath = jobMapsFolder + "\\" + safeName + ".xml";
                    std::string outRaceName = "";
                    JobConverter::ConvertJsonToXml(body, savePath, result.name, outRaceName);

                    std::string imgUrl = basePath + "/2_0.jpg";
                    bool imgSuccess = false;
                    std::string imgData = HttpGet(imgUrl, imgSuccess);
                    if (imgSuccess && imgData.size() > 100)
                    {
                        std::string thumbFolder = GetPathffA(Pathff::Main, false) + "\\Job Thumbnails";
                        CreateDirectoryA(thumbFolder.c_str(), NULL);
                        WriteFileBinary(thumbFolder + "\\" + result.cid + ".jpg", imgData);
                        if (sub::Spooner::JobImporter::savePreviewImage)
                        {
                            WriteFileBinary(jobMapsFolder + "\\" + safeName + ".jpg", imgData);
                        }
                    }

                    pendingSavedNotification = true;
                    downloadStatus = "";
                    found = true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        if (!found)
        {
            downloadStatus = "~r~Could not download job JSON.";
        }
        isDownloading = false;
    }

    static std::string GetLangString(int langId)
    {
        switch (langId)
        {
            case 1: return "en";
            case 2: return "fr";
            case 3: return "de";
            case 4: return "it";
            case 5: return "es";
            case 6: return "pt";
            case 7: return "pl";
            case 8: return "ru";
            case 9: return "ko";
            case 10: return "zh";
            case 11: return "ja";
            case 12: return "es-mx";
            case 13: return "zh-cn";
            default: return "";
        }
    }

    void StartDownload(BrowserJobResult result)
    {
        isDownloading = true;
        downloadStatus = "~b~Starting download...";

        std::string jobLang = GetLangString(result.langId);
        std::vector<std::string> langs = { jobLang.empty() ? "en" : jobLang };

        std::thread t(DownloadJobThread, result, langs);
        t.detach();
    }

    void Sub_JobBrowser()
    {
        AddTitle(GetQueryTitle());

        static int lastSubmenu = -1;
        if (lastSubmenu != Menu::activeSubmenu)
        {
            bool isComingFromInfo = (lastSubmenu == (int)SUB::SPOONER_JOBBROWSER_INFO);
            lastSubmenu = Menu::activeSubmenu;
            if (!isComingFromInfo)
            {
                StartQuery();
            }
        }

        if (queryState == QueryState::Cancelling)
        {
            if (GetTickCount() - queryTimer >= 200)
            {
                int qType = GetQueryType();
                searchStatus = "Querying Rockstar UGC...";
                
                BOOL success = FALSE;
                if (qType == 0)
                {
                    success = UGC_GET_BOOKMARKED_CONTENT(0, 50, "gta5mission", &ugcQueryVar);
                }
                else if (qType == 1)
                {
                    success = UGC_GET_MY_CONTENT(0, 50, "gta5mission", &ugcQueryVar);
                }
                else if (qType == 2)
                {
                    success = UGC_GET_FRIEND_CONTENT(0, 50, "gta5mission", &ugcQueryVar);
                }
                else if (qType == 3)
                {
                    success = UGC_QUERY_BY_CATEGORY(3, 0, 50, "gta5mission", 0, false);
                }
                else if (qType == 4)
                {
                    success = UGC_QUERY_BY_CATEGORY(5, 0, 50, "gta5mission", 0, false);
                }
                else if (qType == 5)
                {
                    success = UGC_GET_CREW_CONTENT(-1, 0, 50, "gta5mission", &ugcQueryVar);
                }

                if (!success)
                {
                    searchStatus = "~r~Query request failed.";
                    isSearching = false;
                    queryState = QueryState::Failed;
                }
                else
                {
                    queryState = QueryState::Requesting;
                    queryTimer = GetTickCount();
                }
            }
        }
        else if (queryState == QueryState::Requesting)
        {
            if (GetTickCount() - queryTimer >= 800)
            {
                queryState = QueryState::Polling;
                queryTimer = GetTickCount();
            }
        }
        else if (queryState == QueryState::Polling)
        {
            BOOL didSucceed = UGC_DID_GET_SUCCEED();
            BOOL hasFinished = UGC_HAS_GET_FINISHED();
            
            if (hasFinished || didSucceed)
            {
                queryState = QueryState::Success;
                ReadQueryResults();
            }
            else if (GetTickCount() - queryTimer >= 15000)
            {
                queryState = QueryState::Failed;
                searchStatus = "~r~Query timed out.";
                isSearching = false;
            }
        }

        bool bRefresh = false;
        AddOption("Refresh list", bRefresh);
        if (bRefresh)
        {
            StartQuery();
        }

        if (isSearching)
        {
            AddBreak("~y~Querying Rockstar UGC...");
        }
        else if (!searchStatus.empty())
        {
            AddBreak(searchStatus);
        }

        if (queryState == QueryState::Success && !results.empty())
        {
            for (size_t i = 0; i < results.size(); ++i)
            {
                bool bSelected = false;
                std::string displayName = (results[i].verified ? "~g~[V] ~s~" : "") + results[i].name;
                AddOption(displayName, bSelected);
                
                if (sub::Spooner::JobImporter::savePreviewImage && Menu::IsLastDrawnOptionSelected())
                {
                    const std::string& cid = results[i].cid;
                    if (!cid.empty())
                    {
                        auto it = thumbCache.find(cid);
                        if (it == thumbCache.end())
                        {
                            std::string thumbPath = GetThumbFolder() + "\\" + cid + ".jpg";
                            std::ifstream f(thumbPath);
                            if (f.good())
                            {
                                f.close();
                                DxHookIMG::DxTexture tex;
                                tex.Load(thumbPath);
                                thumbCache[cid] = tex;
                                it = thumbCache.find(cid);
                            }
                        }
                        if (it != thumbCache.end() && it->second.Exists())
                        {
                            DrawPreviewImage(it->second);
                        }
                    }
                }
                
                if (bSelected)
                {
                    selectedResultIndex = (int)i;
                    downloadStatus = "";
                    Menu::pendingSubmenu = SUB::SPOONER_JOBBROWSER_INFO;
                }
            }
        }
    }

    void Sub_JobBrowser_Info()
    {
        if (selectedResultIndex < 0 || selectedResultIndex >= (int)results.size())
        {
            AddTitle("Error");
            AddBreak("~r~No job selected.");
            return;
        }

        const auto& job = results[selectedResultIndex];
        AddTitle(job.name);

        AddBreak("~b~Creator:~s~ " + job.creator);

        if (sub::Spooner::JobImporter::savePreviewImage && !job.cid.empty())
        {
            auto it = thumbCache.find(job.cid);
            if (it == thumbCache.end())
            {
                std::string thumbPath = GetThumbFolder() + "\\" + job.cid + ".jpg";
                std::ifstream f(thumbPath);
                if (f.good())
                {
                    f.close();
                    DxHookIMG::DxTexture tex;
                    tex.Load(thumbPath);
                    thumbCache[job.cid] = tex;
                    it = thumbCache.find(job.cid);
                }
            }
            if (it != thumbCache.end() && it->second.Exists())
            {
                DrawPreviewImage(it->second);
            }
        }

        AddBreak("~b~Rating:~s~ " + std::to_string(job.rating).substr(0, 4) + " (" + std::to_string(job.ratingCount) + ")");

        bool bDownload = false;
        AddOption("Download and Save", bDownload);  //could prob add more options like adding to bookmarks, spawning without saving, maybe clone to your own jobs.
        if (bDownload)
        {
            if (!isDownloading)
            {
                StartDownload(job);
            }
        }

        if (pendingSavedNotification.exchange(false))
        {
            Game::Print::PrintBottomLeft("Job ~b~saved~s~.");
        }

        if (isDownloading)
        {
            AddBreak("~y~Downloading... Please wait.");
        }
        if (!downloadStatus.empty())
        {
            AddBreak(downloadStatus);
        }
    }
}

namespace sub::Spooner::Submenus
{
    void Sub_JobBrowser()
    {
        sub::Spooner::JobBrowser::Sub_JobBrowser();
    }

    void Sub_JobBrowser_Info()
    {
        sub::Spooner::JobBrowser::Sub_JobBrowser_Info();
    }
}

// ============================================================================
// JOB IMPORTER
// ============================================================================
namespace sub::Spooner::JobImporter
{
    std::string jobImporterUrl = "";
    bool savePreviewImage = true;
    bool isBusy = false;
    std::string status = "";
    std::string statusColorCode = "~w~";
    std::string selectedJobFile = "";

    enum class ImportState {
        Idle,
        Querying,
        Downloading
    };
    static ImportState importState = ImportState::Idle;
    static DWORD importTimer = 0;
    static std::string targetContentId = "";
    static Any importQueryVar = 0;

    static std::string HttpGet(const std::string& url, bool& success) {
        success = false;
        std::string response;
        HINTERNET hInternet = InternetOpenA("MenyooSP", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (hInternet) {
            DWORD timeout = 4000;
            InternetSetOptionA(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
            InternetSetOptionA(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
            
            HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, 
                INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
            if (hConnect) {
                DWORD statusCode = 0;
                DWORD statusCodeSize = sizeof(statusCode);
                if (HttpQueryInfoA(hConnect, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusCodeSize, NULL)) {
                    if (statusCode == 200) {
                        char buffer[8192];
                        DWORD bytesRead = 0;
                        while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
                            response.append(buffer, bytesRead);
                        }
                        success = true;
                    }
                }
                InternetCloseHandle(hConnect);
            }
            InternetCloseHandle(hInternet);
        }
        return response;
    }

    static std::string FindRaceName(const json& j, int depth = 0) {
        if (depth > 5) return "";
        if (j.is_object()) {
            if (j.contains("nm") && j["nm"].is_string()) {
                return j["nm"].get<std::string>();
            }
            for (auto& element : j.items()) {
                std::string r = FindRaceName(element.value(), depth + 1);
                if (!r.empty()) return r;
            }
        } else if (j.is_array()) {
            for (auto& item : j) {
                std::string r = FindRaceName(item, depth + 1);
                if (!r.empty()) return r;
            }
        }
        return "";
    }

    static std::string SanitizeFilename(std::string name) {
        std::string invalidChars = "\\/*?:\"<>|";
        name.erase(std::remove_if(name.begin(), name.end(), [&](char c) {
            return invalidChars.find(c) != std::string::npos;
        }), name.end());
        
        std::string punctuation = " -+.,;=";
        for (char& c : name) {
            if (punctuation.find(c) != std::string::npos) {
                c = '_';
            }
        }
        
        std::string clean;
        bool lastWasUnderscore = false;
        for (char c : name) {
            if (c == '_') {
                if (!lastWasUnderscore) {
                    clean += c;
                    lastWasUnderscore = true;
                }
            } else {
                clean += c;
                lastWasUnderscore = false;
            }
        }
        name = clean;
        
        if (name.length() > 100) {
            name = name.substr(0, 100);
        }
        
        while (!name.empty() && name.front() == '_') {
            name.erase(0, 1);
        }
        while (!name.empty() && name.back() == '_') {
            name.pop_back();
        }
        
        if (name.empty()) {
            name = "unnamed_job";
        }
        return name;
    }

    static void WriteFileBinary(const std::string& path, const std::string& data) {
        std::ofstream out(path, std::ios::out | std::ios::binary);
        if (out) {
            out.write(data.data(), data.size());
        }
    }

    static std::string ExtractContentId(const std::string& url) {
        std::string contentId = "";
        if (url.find("prod.cloud.rockstargames.com") != std::string::npos) {
            size_t lastSlash = url.find_last_of('/');
            if (lastSlash != std::string::npos) {
                std::string cdnBase = url.substr(0, lastSlash);
                size_t contentSlash = cdnBase.find_last_of('/');
                if (contentSlash != std::string::npos) {
                    contentId = cdnBase.substr(contentSlash + 1);
                }
            }
        }
        else if (url.find("socialclub.rockstargames.com") != std::string::npos) {
            size_t jobPos = url.find("/job/gtav/");
            if (jobPos != std::string::npos) {
                contentId = url.substr(jobPos + 10);
                size_t nextSlash = contentId.find('/');
                if (nextSlash != std::string::npos) {
                    contentId = contentId.substr(0, nextSlash);
                }
            }
            else {
                size_t lastSlash = url.find_last_of('/');
                if (lastSlash != std::string::npos) {
                    contentId = url.substr(lastSlash + 1);
                }
            }
            size_t questionMark = contentId.find('?');
            if (questionMark != std::string::npos) {
                contentId = contentId.substr(0, questionMark);
            }
        }
        else {
            contentId = url;
        }
        return contentId;
    }

    static std::string GetLangString(int langId) {
        switch (langId) {
            case 1: return "en";
            case 2: return "fr";
            case 3: return "de";
            case 4: return "it";
            case 5: return "es";
            case 6: return "pt";
            case 7: return "pl";
            case 8: return "ru";
            case 9: return "ko";
            case 10: return "zh";
            case 11: return "ja";
            case 12: return "es-mx";
            case 13: return "zh-cn";
            default: return "";
        }
    }

    static void ImportThreadProc(std::string url, bool saveImg, std::vector<std::string> langs, int resolvedFileVersion, int resolvedCategory)
    {
        std::string jobMapsFolder = GetPathffA(Pathff::Spooner, false) + "\\!Imported Jobs";
        CreateDirectoryA(jobMapsFolder.c_str(), NULL);

        bool isDirectJson = (url.find("prod.cloud.rockstargames.com") != std::string::npos) && 
                            (url.length() > 5 && url.compare(url.length() - 5, 5, ".json") == 0);

        if (isDirectJson)
        {
            status = "Downloading JSON directly...";
            statusColorCode = "~b~";

            bool success = false;
            std::string body = HttpGet(url, success);
            if (success && !body.empty())
            {
                try
                {
                    json jsonData = json::parse(body);
                    std::string raceName = "";
                    if (jsonData.contains("mission") && jsonData["mission"].is_object() && 
                        jsonData["mission"].contains("gen") && jsonData["mission"]["gen"].is_object() && 
                        jsonData["mission"]["gen"].contains("nm") && jsonData["mission"]["gen"]["nm"].is_string())
                    {
                        raceName = jsonData["mission"]["gen"]["nm"].get<std::string>();
                    }
                    if (raceName.empty())
                    {
                        raceName = FindRaceName(jsonData, 0);
                    }
                    if (raceName.empty())
                    {
                        raceName = "unnamed_job";
                    }

                    std::string safeName = SanitizeFilename(raceName);
                    std::string savePath = jobMapsFolder + "\\" + safeName + ".xml";
                    std::string outRaceName = "";
                    JobConverter::ConvertJsonToXml(body, savePath, raceName, outRaceName);

                    std::string cdnBase = "";
                    size_t lastSlash = url.find_last_of('/');
                    if (lastSlash != std::string::npos)
                    {
                        cdnBase = url.substr(0, lastSlash);
                    }

                    if (saveImg && !cdnBase.empty())
                    {
                        status = "Downloading preview image...";
                        std::string imgUrl = cdnBase + "/2_0.jpg";
                        bool imgSuccess = false;
                        std::string imgData = HttpGet(imgUrl, imgSuccess);
                        if (imgSuccess && imgData.size() > 100)
                        {
                            std::string thumbFolder = GetPathffA(Pathff::Main, false) + "\\Job Thumbnails";
                            CreateDirectoryA(thumbFolder.c_str(), NULL);
                            std::string cid = cdnBase;
                            size_t s = cid.find_last_of('/');
                            if (s != std::string::npos) cid = cid.substr(s + 1);
                            WriteFileBinary(thumbFolder + "\\" + cid + ".jpg", imgData);
                            WriteFileBinary(jobMapsFolder + "\\" + safeName + ".jpg", imgData);
                        }
                    }

                    status = "Imported: " + raceName;
                    statusColorCode = "~g~";
                }
                catch (...)
                {
                    status = "Failed to parse JSON";
                    statusColorCode = "~r~";
                }
            }
            else
            {
                status = "Failed to download JSON";
                statusColorCode = "~r~";
            }

            isBusy = false;
            return;
        }

        std::string cdnBase = "";
        std::string contentId = "";

        if (url.find("prod.cloud.rockstargames.com") != std::string::npos)
        {
            size_t lastSlash = url.find_last_of('/');
            if (lastSlash != std::string::npos)
            {
                cdnBase = url.substr(0, lastSlash);
                size_t contentSlash = cdnBase.find_last_of('/');
                if (contentSlash != std::string::npos)
                {
                    contentId = cdnBase.substr(contentSlash + 1);
                }
            }
            else
            {
                cdnBase = url;
            }
        }
        else if (url.find("socialclub.rockstargames.com") != std::string::npos)
        {
            size_t jobPos = url.find("/job/gtav/");
            if (jobPos != std::string::npos)
            {
                contentId = url.substr(jobPos + 10);
                size_t nextSlash = contentId.find('/');
                if (nextSlash != std::string::npos)
                {
                    contentId = contentId.substr(0, nextSlash);
                }
            }
            else
            {
                size_t lastSlash = url.find_last_of('/');
                if (lastSlash != std::string::npos)
                {
                    contentId = url.substr(lastSlash + 1);
                }
            }
            size_t questionMark = contentId.find('?');
            if (questionMark != std::string::npos)
            {
                contentId = contentId.substr(0, questionMark);
            }
        }
        else
        {
            contentId = url;
        }

        if (contentId.empty())
        {
            status = "Could not extract content ID";
            statusColorCode = "~r~";
            isBusy = false;
            return;
        }

        std::string basePath = cdnBase.empty() ? ("https://prod.cloud.rockstargames.com/ugc/gta5mission/" + contentId) : cdnBase;

        status = "Searching for job data...";
        statusColorCode = "~b~";

        bool found = false;
        std::vector<std::pair<int, int>> commonPatterns = { {0,0}, {0,1}, {1,0}, {2,0} };

        for (const auto& pattern : commonPatterns)
        {
            if (found) break;

            for (const auto& lang : langs)
            {
                if (found) break;

                std::string tryUrl = basePath + "/" + std::to_string(pattern.first) + "_" + std::to_string(pattern.second) + "_" + lang + ".json";
                status = "Trying: " + lang + " (" + std::to_string(pattern.first) + "_" + std::to_string(pattern.second) + ")...";
                
                bool success = false;
                std::string body = HttpGet(tryUrl, success);
                if (success && !body.empty() && body.front() == '{')
                {
                    try
                    {
                        json jsonData = json::parse(body);
                        std::string raceName = "";
                        if (jsonData.contains("mission") && jsonData["mission"].is_object() && 
                            jsonData["mission"].contains("gen") && jsonData["mission"]["gen"].is_object() && 
                            jsonData["mission"]["gen"].contains("nm") && jsonData["mission"]["gen"]["nm"].is_string())
                        {
                            raceName = jsonData["mission"]["gen"]["nm"].get<std::string>();
                        }
                        if (raceName.empty())
                        {
                            raceName = FindRaceName(jsonData, 0);
                        }
                        if (raceName.empty())
                        {
                            raceName = "unnamed_job";
                        }

                        std::string safeName = SanitizeFilename(raceName);
                        std::string savePath = jobMapsFolder + "\\" + safeName + ".xml";
                        std::string outRaceName = "";
                        JobConverter::ConvertJsonToXml(body, savePath, raceName, outRaceName);

                        if (saveImg)
                        {
                            status = "Downloading preview image...";
                            std::string imgUrl = basePath + "/2_0.jpg";
                            bool imgSuccess = false;
                            std::string imgData = HttpGet(imgUrl, imgSuccess);
                            if (imgSuccess && imgData.size() > 100)
                            {
                                std::string thumbFolder = GetPathffA(Pathff::Main, false) + "\\Job Thumbnails";
                                CreateDirectoryA(thumbFolder.c_str(), NULL);
                                WriteFileBinary(thumbFolder + "\\" + contentId + ".jpg", imgData);
                                WriteFileBinary(jobMapsFolder + "\\" + safeName + ".jpg", imgData);
                            }
                        }

                        status = "Imported: " + raceName;
                        statusColorCode = "~g~";
                        found = true;
                    }
                    catch (...) {}
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        if (!found)
        {
            status = "No JSON found. Try a direct CDN URL.";
            statusColorCode = "~r~";
        }
        isBusy = false;
    }

    void StartImport(const std::string& url)
    {
        std::vector<std::string> defaultLangs = { "en", "ja", "zh", "zh-cn", "fr", "de", "it", "ru", "pt", "pl", "ko", "es", "es-mx" };
        
        int langId = LOCALIZATION::GET_CURRENT_LANGUAGE();
        std::string prefLang = "";
        switch (langId)
        {
            case 0: prefLang = "en"; break;
            case 1: prefLang = "fr"; break;
            case 2: prefLang = "de"; break;
            case 3: prefLang = "it"; break;
            case 4: prefLang = "es"; break;
            case 5: prefLang = "pt"; break;
            case 6: prefLang = "pl"; break;
            case 7: prefLang = "ru"; break;
            case 8: prefLang = "ko"; break;
            case 9: prefLang = "zh"; break;
            case 10: prefLang = "ja"; break;
            case 11: prefLang = "es-mx"; break;
            case 12: prefLang = "zh-cn"; break;
        }

        if (!prefLang.empty())
        {
            auto it = std::find(defaultLangs.begin(), defaultLangs.end(), prefLang);
            if (it != defaultLangs.end())
            {
                defaultLangs.erase(it);
            }
            defaultLangs.insert(defaultLangs.begin(), prefLang);
        }

        std::string contentId = ExtractContentId(url);
        
        bool isDirectCdn = (url.find("prod.cloud.rockstargames.com") != std::string::npos) && (url.find_last_of('/') != std::string::npos);
        if (isDirectCdn)
        {
            std::thread t(ImportThreadProc, url, savePreviewImage, defaultLangs, -1, -1);
            t.detach();
        }
        else
        {
            targetContentId = contentId;
            isBusy = true;
            status = "Querying Rockstar Services...";
            statusColorCode = "~b~";
            
            UGC_CANCEL_QUERY();
            importTimer = GetTickCount();
            importQueryVar = 0;
            importState = ImportState::Querying;
            
            UGC_QUERY_BY_CONTENT_ID(targetContentId.c_str(), TRUE, "gta5mission");
        }
    }

    void Sub_JobImporter()
    {
        AddTitle("Job Importer");

        AddToggle("Preview Images", savePreviewImage);

        AddOption("Bookmarked Jobs", null, nullFunc, SUB::SPOONER_JOBBROWSER);
        AddOption("My Content", null, nullFunc, SUB::SPOONER_JOBBROWSER_MYCONTENT);
        AddOption("Friend Content", null, nullFunc, SUB::SPOONER_JOBBROWSER_FRIENDCONTENT);
        AddOption("Most Recent", null, nullFunc, SUB::SPOONER_JOBBROWSER_MOSTRECENT);
        AddOption("Top Rated", null, nullFunc, SUB::SPOONER_JOBBROWSER_TOPRATED);
        AddOption("Crew Content", null, nullFunc, SUB::SPOONER_JOBBROWSER_CREWCONTENT);

        bool bLoadFromUrl = false;
        AddOption("Load From URL", bLoadFromUrl);
        if (bLoadFromUrl)
        {
            std::string defaultUrl = jobImporterUrl.empty() ? GetClipboardText() : jobImporterUrl;
            std::string inputStr = Game::InputBox("", 512U, "Enter Job/SC URL:", defaultUrl);
            if (inputStr.length() > 0)
            {
                jobImporterUrl = inputStr;
                isBusy = true;
                status = "Starting import...";
                statusColorCode = "~b~";
                StartImport(jobImporterUrl);
            }
        }

        if (importState == ImportState::Querying)
        {
            BOOL didSucceed = UGC_DID_GET_SUCCEED();
            BOOL hasFinished = UGC_HAS_GET_FINISHED();
            
            if (hasFinished || didSucceed)
            {
                int fileVer = -1;
                int langId = -1;
                if (UGC_GET_CONTENT_NUM() > 0)
                {
                    fileVer = UGC_GET_CONTENT_FILE_VERSION(0, 0);
                    langId = UGC_GET_CONTENT_LANGUAGE(0);
                }
                
                std::string jobLang = GetLangString(langId);
                std::vector<std::string> langs = { jobLang.empty() ? "en" : jobLang };
                
                status = "Starting download...";
                std::thread t(ImportThreadProc, jobImporterUrl, savePreviewImage, langs, fileVer, -1);
                t.detach();
                importState = ImportState::Downloading;
            }
        }
        else if (importState == ImportState::Downloading)
        {
            if (!isBusy)
            {
                importState = ImportState::Idle;
            }
        }

        if (isBusy)
        {
            AddBreak("~y~Importing... Please wait.");
        }
        if (!status.empty())
        {
            AddBreak(statusColorCode + status);
        }
    }
}

namespace sub::Spooner::Submenus
{
    void Sub_JobImporter()
    {
        sub::Spooner::JobImporter::Sub_JobImporter();
    }
}

// ============================================================================
// JOB CONVERTER
// ============================================================================
namespace sub::Spooner::JobConverter
{
    struct JobEntity {
        unsigned int model = 0;
        Vector3 pos{0, 0, 0};
        Vector3 rot{0, 0, 0};
        float heading = 0.0f;
        std::string entityType;
        int tintIndex = -1;
    };

    static std::vector<JobEntity> ExtractEntitiesFromSection(const json& section, const std::string& entityType, const std::string& tintFieldName) {
        std::vector<JobEntity> entities;
        if (!section.is_object()) return entities;
        
        int count = 0;
        if (section.contains("no") && section["no"].is_number()) {
            count = section["no"].get<int>();
        }
        if (count <= 0) return entities;
        
        auto getFloat = [](const json& j, float def = 0.0f) -> float {
            if (j.is_number()) return j.get<float>();
            if (j.is_string()) {
                try { return std::stof(j.get<std::string>()); } catch (...) {}
            }
            return def;
        };
        
        auto getUInt = [](const json& j, unsigned int def = 0) -> unsigned int {
            if (j.is_number()) return j.get<unsigned int>();
            if (j.is_string()) {
                try { return std::stoul(j.get<std::string>()); } catch (...) {}
            }
            return def;
        };

        for (int i = 0; i < count; ++i) {
            unsigned int modelHash = 0;
            if (section.contains("model") && section["model"].is_array() && i < section["model"].size()) {
                modelHash = getUInt(section["model"][i]);
            }
            
            if (modelHash == 0) continue;
            
            Vector3 pos{0, 0, 0};
            if (section.contains("loc") && section["loc"].is_array() && i < section["loc"].size()) {
                const auto& locVal = section["loc"][i];
                if (locVal.is_object()) {
                    if (locVal.contains("x")) pos.x = getFloat(locVal["x"]);
                    if (locVal.contains("y")) pos.y = getFloat(locVal["y"]);
                    if (locVal.contains("z")) pos.z = getFloat(locVal["z"]);
                }
            }
            
            Vector3 rot{0, 0, 0};
            bool hasRot = false;
            if (section.contains("vRot") && section["vRot"].is_array() && i < section["vRot"].size()) {
                const auto& rotVal = section["vRot"][i];
                if (rotVal.is_object()) {
                    if (rotVal.contains("x")) rot.x = getFloat(rotVal["x"]);
                    if (rotVal.contains("y")) rot.y = getFloat(rotVal["y"]);
                    if (rotVal.contains("z")) rot.z = getFloat(rotVal["z"]);
                    hasRot = true;
                }
            }
            
            float heading = 0.0f;
            if (section.contains("head") && section["head"].is_array() && i < section["head"].size()) {
                heading = getFloat(section["head"][i]);
            }
            
            int tintIndex = -1;
            if (!tintFieldName.empty() && section.contains(tintFieldName) && section[tintFieldName].is_array() && i < section[tintFieldName].size()) {
                if (section[tintFieldName][i].is_number()) {
                    tintIndex = section[tintFieldName][i].get<int>();
                }
            }
            
            JobEntity entity;
            entity.model = modelHash;
            entity.pos = pos;
            if (hasRot) {
                entity.rot = rot;
            } else {
                entity.rot = Vector3{0.0f, 0.0f, heading};
            }
            entity.heading = heading;
            entity.entityType = entityType;
            entity.tintIndex = tintIndex;
            
            entities.push_back(entity);
        }
        return entities;
    }

    static std::string FindRaceName(const json& j, int depth = 0) {
        if (depth > 5) return "";
        if (j.is_object()) {
            if (j.contains("nm") && j["nm"].is_string()) {
                return j["nm"].get<std::string>();
            }
            for (auto& element : j.items()) {
                std::string r = FindRaceName(element.value(), depth + 1);
                if (!r.empty()) return r;
            }
        } else if (j.is_array()) {
            for (auto& item : j) {
                std::string r = FindRaceName(item, depth + 1);
                if (!r.empty()) return r;
            }
        }
        return "";
    }

    bool ConvertJsonToXml(const std::string& jsonStr, const std::string& xmlOutPath, const std::string& defaultName, std::string& outRaceName)
    {
        json jsonData;
        try {
            jsonData = json::parse(jsonStr);
        } catch (...) {
            return false;
        }

        if (!jsonData.contains("mission") || !jsonData["mission"].is_object()) {
            return false;
        }
        
        const auto& mission = jsonData["mission"];
        
        std::string raceName = "";
        if (mission.contains("gen") && mission["gen"].is_object() && 
            mission["gen"].contains("nm") && mission["gen"]["nm"].is_string())
        {
            raceName = mission["gen"]["nm"].get<std::string>();
        }
        if (raceName.empty())
        {
            raceName = FindRaceName(jsonData, 0);
        }
        if (raceName.empty())
        {
            raceName = defaultName;
        }
        outRaceName = raceName;

        std::vector<JobEntity> allEntities;
        
        if (mission.contains("prop") && mission["prop"].is_object()) {
            auto props = ExtractEntitiesFromSection(mission["prop"], "OBJECT", "prpclr");
            allEntities.insert(allEntities.end(), props.begin(), props.end());
        }
        
        if (mission.contains("dprop") && mission["dprop"].is_object()) {
            auto dprops = ExtractEntitiesFromSection(mission["dprop"], "OBJECT", "prpdclr");
            allEntities.insert(allEntities.end(), dprops.begin(), dprops.end());
        }
        
        if (mission.contains("veh") && mission["veh"].is_object()) {
            auto vehs = ExtractEntitiesFromSection(mission["veh"], "VEHICLE", "");
            allEntities.insert(allEntities.end(), vehs.begin(), vehs.end());
        }

        Vector3 refCoords{ 0.0f, 0.0f, 0.0f };
        bool hasRef = false;
        
        if (mission.contains("fsp") && mission["fsp"].is_object() && 
            mission["fsp"].contains("loc") && mission["fsp"]["loc"].is_array() && 
            mission["fsp"]["loc"].size() > 0) {
            const auto& fspLoc = mission["fsp"]["loc"][0];
            if (fspLoc.is_object() && fspLoc.contains("x") && fspLoc.contains("y") && fspLoc.contains("z")) {
                refCoords.x = fspLoc["x"].get<float>();
                refCoords.y = fspLoc["y"].get<float>();
                refCoords.z = fspLoc["z"].get<float>();
                if (refCoords.x != 0.0f || refCoords.y != 0.0f || refCoords.z != 0.0f) {
                    hasRef = true;
                }
            }
        }
        
        if (!hasRef && !allEntities.empty()) {
            refCoords = allEntities[0].pos;
        }

        pugi::xml_document doc;
        auto decl = doc.append_child(pugi::node_declaration);
        decl.append_attribute("version") = "Job Importer";
        decl.append_attribute("encoding") = "ISO-8859-1"; // should put the name of the creator of the job here incase people steal them and reupload.

        auto nodeRoot = doc.append_child("SpoonerPlacements");
        nodeRoot.append_child("Note");

        auto nodeAudio = nodeRoot.append_child("AudioFile");
        nodeAudio.append_attribute("volume") = 400;

        nodeRoot.append_child("ClearDatabase").text() = "false";
        nodeRoot.append_child("ClearWorld").text() = "0";
        nodeRoot.append_child("ClearMarkers").text() = "false";

        auto nodeIpls = nodeRoot.append_child("IPLsToLoad");
        nodeIpls.append_attribute("load_mp_maps") = "false";
        nodeIpls.append_attribute("load_sp_maps") = "false";

        nodeRoot.append_child("IPLsToRemove");
        nodeRoot.append_child("InteriorsToEnable");
        nodeRoot.append_child("InteriorsToCap");
        nodeRoot.append_child("WeatherToSet");
        
        auto nodeTyc = nodeRoot.append_child("TimecycleModifier");
        nodeTyc.append_attribute("strength") = 1.0f;

        nodeRoot.append_child("StartTaskSequencesOnLoad").text() = "true";

        auto nodeRef = nodeRoot.append_child("ReferenceCoords"); // seems a bit off 
        nodeRef.append_child("X").text() = refCoords.x;
        nodeRef.append_child("Y").text() = refCoords.y;
        nodeRef.append_child("Z").text() = refCoords.z;

        int index = 0;
        for (const auto& jobEnt : allEntities)
        {
            auto nodePlacement = nodeRoot.append_child("Placement");

            std::stringstream ssHash;
            ssHash << "0x" << std::hex << jobEnt.model;
            nodePlacement.append_child("ModelHash").text() = ssHash.str().c_str();

            int typeVal = (jobEnt.entityType == "VEHICLE") ? 2 : 3;
            nodePlacement.append_child("Type").text() = typeVal;

            bool isDyn = (jobEnt.entityType == "VEHICLE");
            nodePlacement.append_child("Dynamic").text() = isDyn ? "true" : "false";
            nodePlacement.append_child("FrozenPos").text() = isDyn ? "false" : "true";

            std::string hashName = "";
            if (jobEnt.entityType == "VEHICLE")
            {
                hashName = get_vehicle_model_label(jobEnt.model, false);
            }
            else
            {
                hashName = get_prop_model_label(jobEnt.model);
            }
            if (hashName.empty() || hashName == "NULL" || hashName == "unknown")
            {
                hashName = ssHash.str();
            }
            nodePlacement.append_child("HashName").text() = hashName.c_str();

            nodePlacement.append_child("InitialHandle").text() = 200000 + index++;

            if (jobEnt.entityType == "OBJECT" && jobEnt.tintIndex >= 0)
            {
                auto nodeProp = nodePlacement.append_child("ObjectProperties");
                nodeProp.append_child("TextureVariation").text() = jobEnt.tintIndex;
            }
            else
            {
                nodePlacement.append_child("ObjectProperties");
            }

            nodePlacement.append_child("OpacityLevel").text() = 255;
            nodePlacement.append_child("LodDistance").text() = 16960;
            nodePlacement.append_child("IsVisible").text() = "true";
            nodePlacement.append_child("MaxHealth").text() = 1000;
            nodePlacement.append_child("Health").text() = 1000;
            nodePlacement.append_child("HasGravity").text() = "true";
            nodePlacement.append_child("IsOnFire").text() = "false";
            nodePlacement.append_child("IsInvincible").text() = "true";
            nodePlacement.append_child("IsBulletProof").text() = "false";
            nodePlacement.append_child("IsCollisionProof").text() = "false";
            nodePlacement.append_child("IsExplosionProof").text() = "true";
            nodePlacement.append_child("IsFireProof").text() = "false";
            nodePlacement.append_child("IsMeleeProof").text() = "true";
            nodePlacement.append_child("IsOnlyDamagedByPlayer").text() = "false";

            auto nodePosRot = nodePlacement.append_child("PositionRotation");
            nodePosRot.append_child("X").text() = jobEnt.pos.x;
            nodePosRot.append_child("Y").text() = jobEnt.pos.y;
            nodePosRot.append_child("Z").text() = jobEnt.pos.z;
            nodePosRot.append_child("Pitch").text() = jobEnt.rot.x;
            nodePosRot.append_child("Roll").text() = jobEnt.rot.y;
            nodePosRot.append_child("Yaw").text() = jobEnt.rot.z;

            auto nodeAttach = nodePlacement.append_child("Attachment");
            nodeAttach.append_attribute("isAttached") = "false";
        }

        return doc.save_file((const char*)xmlOutPath.c_str());
    }
}
