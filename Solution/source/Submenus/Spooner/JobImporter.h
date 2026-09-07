#pragma once
#include <string>
#include <vector>

namespace sub::Spooner::JobBrowser
{
    struct BrowserJobResult {
        std::string name;
        std::string cid;
        std::string creator;
        int category = 0;
        int descHash = 0;
        std::string cdnPath;
        std::string cdnBase;
        int fileVersion = 0;
        float rating = 0.0f;
        int ratingCount = 0;
        bool verified = false;
        int langId = -1;
    };

    void Sub_JobBrowser();
    void Sub_JobBrowser_Info();

    extern std::vector<BrowserJobResult> results;
    extern int selectedResultIndex;
    extern bool isSearching;
    extern std::string searchStatus;
}
namespace sub::Spooner::JobImporter
{
    void Sub_JobImporter();
    
    extern std::string jobImporterUrl;
    extern bool savePreviewImage;
    extern bool isBusy;
    extern std::string status;
    extern std::string statusColorCode;

    void StartImport(const std::string& url);
}

namespace sub::Spooner::JobConverter
{
    bool ConvertJsonToXml(const std::string& jsonStr, const std::string& xmlOutPath, const std::string& defaultName, std::string& outRaceName);
}
