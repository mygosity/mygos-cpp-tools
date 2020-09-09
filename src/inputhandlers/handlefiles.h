#ifndef MGCP_INPUTHANDLERS_HANDLEFILES_H
#define MGCP_INPUTHANDLERS_HANDLEFILES_H

#include "../lib/common.h"
#include "../lib/utils/filemanagement/filehelper.h"
#include "../lib/utils/filemanagement/filestructs.h"
#include "../lib/utils/filemanagement/memorymappedjson.h"
#include "../lib/utils/threadpool.h"
#include "../lib/utils/timemanager.h"
#include "../lib/utils/time.h"

namespace mgcp {

struct FileInputHandlers {
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

    FileInputHandlers(CodeBlacksmith::ThreadPool& threadPool, mgcp::TimeManager& timeManager, mgcp::FileHelper& fileHelper)
        : threadPool(threadPool), timeManager(timeManager), fileHelper(fileHelper) {
        map = {
            {"c", [&, this](std::string input) { timeManager.ClearAll(); }},
            {"t",
             [&, this](std::string input) {
                 clock["t"] = mgcp::GetMicroTime();
                 fileHelper.TestJsonWriting([&, this]() { stdlog("test op time: " << mgcp::GetMicroTime() - clock["t"]); });
             }},
            {"r", [&, this](std::string input) { fileHelper.PrintAllFileHelperSettings(); }},
            {"k",
             [&, this](std::string input) {
                 fileHelper.SetFileWriteType(2);
                 clock["k"] = mgcp::GetMicroTime();
                 std::string data = "{ \"t1\": \"hello world\" }";
                 auto options = mgcp::FileWriteOptions();
                 options.shouldWrapDataAsArray = true;
                 options.append = false;
                 options.overwrite = true;
                 std::string filename = "testfile.json";
                 options.callback = [&, this]() {
                     stdlog("C++ Finished writing to the file in time: " << mgcp::GetMicroTime() - clock["k"] << " microseconds");
                 };
                 fileHelper.WriteFile(logpath, filename, data, options);
             }},
            {"j",
             [&, this](std::string input) {
                 fileHelper.SetFileWriteType(1);
                 clock["j"] = mgcp::GetMicroTime();
                 std::string data = "{ \"t1\": \"hello world\" }";
                 auto options = mgcp::FileWriteOptions();
                 options.shouldWrapDataAsArray = true;
                 options.append = false;
                 options.overwrite = true;
                 std::string path = "";
                 std::string filename = "testfile.json";
                 options.callback = [&, this]() {
                     stdlog("C++ Finished writing to the file in time: " << mgcp::GetMicroTime() - clock["j"] << " microseconds");
                 };
                 fileHelper.WriteFile(logpath, filename, data, options);
             }},
            {"o",
             [&, this](std::string input) {
                 fileHelper.SetFileWriteType(0);
                 clock["o"] = mgcp::GetMicroTime();
                 std::string data = "{ \"t1\": \"hello world\" }";
                 // std::string start = "{ \"teststring\": \"";
                 // std::string meat = "";
                 // for (int i = 0; i < 1000000; ++i)
                 // {
                 // 	meat.append("123456789");
                 // }
                 // std::string end = "\" }";
                 // std::string data = start + meat + end;
                 auto options = mgcp::FileWriteOptions();
                 options.shouldWrapDataAsArray = true;
                 options.append = false;
                 options.overwrite = true;
                 std::string filename = "testfile.json";
                 options.callback = [&, this]() {
                     stdlog("C++ Finished writing to the file in time: " << mgcp::GetMicroTime() - clock["o"] << " microseconds");
                 };
                 fileHelper.WriteFile(logpath, filename, data, options);
             }},
            {"p",
             [&, this](std::string input) {
                 clock["p"] = mgcp::GetMicroTime();
                 std::string data = "{ \"t1\": \"hello world\" }";
                 auto options = mgcp::FileWriteOptions();
                 options.shouldWrapDataAsArray = true;
                 std::string filename = "testfile.json";
                 options.callback = [&, this]() {
                     stdlog("C++ Finished writing to the file in time: " << mgcp::GetMicroTime() - clock["p"] << " microseconds");
                 };
                 fileHelper.WriteFile(logpath, filename, data, options);
             }},
            {"w",
             [&, this](std::string input) {
                 auto options = mgcp::FileWriteOptions();
                 std::string data = "{ \"test\": \"" + std::to_string(writeCounter++) + std::string("\" }");
                 std::string filename = "testfile.json";
                 fileHelper.WriteFile(logpath, filename, data, options);
             }},
            {"wo",
             [&, this](std::string input) {
                 stdlog("overwritecall");
                 auto options = mgcp::FileWriteOptions();
                 options.append = false;
                 options.overwrite = true;
                 std::string data = "{ \"test\": \"" + std::to_string(writeCounter++) + std::string("\" }");
                 std::string testpath = "wooohoo/somewhere/gogo/";
                 std::string altfile = "testfile2.json";
                 fileHelper.WriteFile(testpath, altfile, data, options);
             }},
            {"u",
             [&, this](std::string input) {
                 std::function<void()> ufunc = [&, this]() {
                     auto options = mgcp::FileWriteOptions();
                     std::string data = "{ \"uuuu-test\": \"" + std::to_string(writeCounteru++) + std::string("\" }");
                     std::string filename = "testfile.json";
                     options.callback = [&, this]() { stdlog("interval uuuu test finished"); };
                     fileHelper.WriteFile(logpath, filename, data, options);
                 };
                 timeManager.SetOrUpdateInterval(ufunc, 16, 1);
             }},
            {"y",
             [&, this](std::string input) {
                 std::function<void()> yfunc = [&, this]() {
                     auto options = mgcp::FileWriteOptions();
                     std::string data = "{ \"y-test\": \"" + std::to_string(writeCountery++) + std::string("\" }");
                     std::string filename = "testfile.json";
                     options.callback = [&, this]() { stdlog("interval y test finished"); };
                     fileHelper.WriteFile(logpath, filename, data, options);
                 };
                 timeManager.SetOrUpdateInterval(yfunc, 10, 0);
             }},
            {"togglelog",
             [&, this](std::string input) {
                 rapidjson::Document& target = fileHelper.GetSettingsConfig();
                 rapidjson::Value* filewritingSettingPtr = rapidjson::GetValueByPointer(target, "/logging/filewriting");
                 filewritingSettingPtr->SetBool(!filewritingSettingPtr->GetBool());
                 fileHelper.SaveSettings();
                 fileHelper.LoadSettings();
             }},
            {"create",
             [&, this](std::string input) {
                 clock["create"] = mgcp::GetMicroTime();
                 std::int64_t timenow = mgcp::GetMicroTime();
                 uint64_t size = 1000000;
                 testfile = fileHelper.CreateMappedFile("", "testfile.json", (uint64_t)size);
                 stdlog("C++ opened memory mapped file: " << mgcp::GetMicroTime() - clock["create"] << " microseconds of size: " << size);
             }},
            {"write",
             [&, this](std::string input) {
                 clock["write"] = mgcp::GetMicroTime();
                 testfile->AppendJsonData("{ \"prop\": \"key\" }");
                 stdlog("C++ wrote to mapped file: " << mgcp::GetMicroTime() - clock["write"] << " microseconds");
             }},
            {"clear",
             [&, this](std::string input) {
                 clock["clear"] = mgcp::GetMicroTime();
                 testfile->CloseFile();
                 testfile.reset();
                 fileHelper.ClearMappedFiles();
                 stdlog("C++ closed mapped file: " << mgcp::GetMicroTime() - clock["clear"] << " microseconds");
             }},
            {"reload", [&, this](std::string input) { fileHelper.LoadSettings(); }}
            //
        };
    };
    ~FileInputHandlers(){};

    std::map<std::string, std::function<void(std::string)>> map;

    std::map<std::string, std::function<void(std::string)>>& GetMap() { return map; }
};

}  // namespace mgcp

#endif