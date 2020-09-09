#ifndef MGCP_INPUTHANDLERS_HANDLESOCKETS_H
#define MGCP_INPUTHANDLERS_HANDLESOCKETS_H

#include "../lib/common.h"
#include "../lib/utils/filemanagement/filehelper.h"
#include "../lib/utils/filemanagement/filestructs.h"
#include "../lib/utils/filemanagement/memorymappedjson.h"
#include "../lib/utils/threadpool.h"
#include "../lib/utils/timemanager.h"

namespace mgcp {

struct SocketHandlers {
    CodeBlacksmith::ThreadPool& threadPool;
    mgcp::TimeManager& timeManager;
    mgcp::FileHelper& fileHelper;

    const std::string logpath = std::string("_logs") + DIR_SLASH;

    std::map<std::string, int64_t> clock;
    std::shared_ptr<mgcp::MemoryMappedJson> testfile;

    int32_t key = 0;
    int32_t writeCounter = 0;
    int32_t writeCountery = 0;
    int32_t writeCounteru = 0;

    SocketHandlers(CodeBlacksmith::ThreadPool& threadPool, mgcp::TimeManager& timeManager, mgcp::FileHelper& fileHelper)
        : threadPool(threadPool), timeManager(timeManager), fileHelper(fileHelper) {
        map = {
            {"c", [&, this](std::string input) { timeManager.ClearAll(); }},
            {"togglelog",
             [&, this](std::string input) {
                 rapidjson::Document& target = fileHelper.GetSettingsConfig();
                 rapidjson::Value* filewritingSettingPtr = rapidjson::GetValueByPointer(target, "/logging/filewriting");
                 filewritingSettingPtr->SetBool(!filewritingSettingPtr->GetBool());
                 fileHelper.SaveSettings();
                 fileHelper.LoadSettings();
             }},
            {"reload", [&, this](std::string input) { fileHelper.LoadSettings(); }}
            //
        };
    };
    ~SocketHandlers(){};

    std::map<std::string, std::function<void(std::string)>> map;

    std::map<std::string, std::function<void(std::string)>>& GetMap() { return map; }
};

}  // namespace mgcp

#endif