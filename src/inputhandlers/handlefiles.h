
#pragma once

#include "../lib/common.h"
#include "../lib/utils/threadpool.h"
#include "../lib/utils/filemanagement/filestructs.h"
#include "../lib/utils/filemanagement/filehelper.h"
#include "../lib/utils/filemanagement/memorymappedjson.h"
#include "../lib/utils/timemanager.h"

namespace mgcp
{
    struct FileInputHandlers
    {
        CodeBlacksmith::ThreadPool &threadPool;
        mgcp::TimeManager &timeManager;
        mgcp::FileHelper &fileHelper;
        std::map<std::string, int64_t> clock;

        FileInputHandlers(CodeBlacksmith::ThreadPool &threadPool, mgcp::TimeManager &timeManager, mgcp::FileHelper &fileHelper)
            : threadPool(threadPool), timeManager(timeManager), fileHelper(fileHelper)
        {
            int32_t key = 0;
            int32_t writeCounter = 0;
            int32_t writeCountery = 0;
            int32_t writeCounteru = 0;
            std::shared_ptr<mgcp::MemoryMappedJson> testfile;

            map = {
                {"c", [&, this](std::string input) {
                     timeManager.ClearAll();
                 }},
                {"t", [&, this](std::string input) {
                     clock["t"] = mgcp::TimeManager::GetMicroTime();
                     fileHelper.TestJsonWriting([&, this]() {
                         stdlog("test op time: " << mgcp::TimeManager::GetMicroTime() - clock["t"]);
                     });
                 }},
                {"r", [&, this](std::string input) {
                     fileHelper.PrintAllFileHelperSettings();
                 }},
                {"k", [&, this](std::string input) {
                     fileHelper.SetFileWriteType(2);
                     clock["k"] = mgcp::TimeManager::GetMicroTime();
                     std::string data = "{ \"t1\": \"hello world\" }";
                     auto options = mgcp::FileWriteOptions();
                     options.shouldWrapDataAsArray = true;
                     options.append = false;
                     options.overwrite = true;
                     std::string path = "";
                     std::string filename = "testfile.json";
                     options.callback = [&, this]() {
                         stdlog("C++ Finished writing to the file in time: " << mgcp::TimeManager::GetMicroTime() - clock["k"] << " microseconds");
                     };
                     fileHelper.WriteFile(path, filename, data, options);
                 }},
                {"j", [&, this](std::string input) {
                     fileHelper.SetFileWriteType(1);
                     clock["j"] = mgcp::TimeManager::GetMicroTime();
                     std::string data = "{ \"t1\": \"hello world\" }";
                     auto options = mgcp::FileWriteOptions();
                     options.shouldWrapDataAsArray = true;
                     options.append = false;
                     options.overwrite = true;
                     std::string path = "";
                     std::string filename = "testfile.json";
                     options.callback = [&, this]() {
                         stdlog("C++ Finished writing to the file in time: " << mgcp::TimeManager::GetMicroTime() - clock["j"] << " microseconds");
                     };
                     fileHelper.WriteFile(path, filename, data, options);
                 }},
                {"o", [&, this](std::string input) {
                     fileHelper.SetFileWriteType(0);
                     clock["o"] = mgcp::TimeManager::GetMicroTime();
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
                     std::string path = "";
                     std::string filename = "testfile.json";
                     options.callback = [&, this]() {
                         stdlog("C++ Finished writing to the file in time: " << mgcp::TimeManager::GetMicroTime() - clock["o"] << " microseconds");
                     };
                     fileHelper.WriteFile(path, filename, data, options);
                 }},
                {"p", [&, this](std::string input) {
                     clock["p"] = mgcp::TimeManager::GetMicroTime();
                     std::string data = "{ \"t1\": \"hello world\" }";
                     auto options = mgcp::FileWriteOptions();
                     options.shouldWrapDataAsArray = true;
                     std::string path = "";
                     std::string filename = "testfile.json";
                     options.callback = [&, this]() {
                         stdlog("C++ Finished writing to the file in time: " << mgcp::TimeManager::GetMicroTime() - clock["p"] << " microseconds");
                     };
                     fileHelper.WriteFile(path, filename, data, options);
                 }},
                {"w", [&, this](std::string input) {
                     auto options = mgcp::FileWriteOptions();
                     std::string data = "{ \"test\": \"" + std::to_string(writeCounter++) + std::string("\" }");
                     std::string path = "";
                     std::string filename = "testfile.json";
                     fileHelper.WriteFile(path, filename, data, options);
                 }},
                {"wo", [&, this](std::string input) {
                     stdlog("overwritecall");
                     auto options = mgcp::FileWriteOptions();
                     options.append = false;
                     options.overwrite = true;
                     std::string data = "{ \"test\": \"" + std::to_string(writeCounter++) + std::string("\" }");
                     std::string testpath = "wooohoo/somewhere/gogo/";
                     std::string altfile = "testfile2.json";
                     fileHelper.WriteFile(testpath, altfile, data, options);
                 }},
                {"u", [&, this](std::string input) {
                     std::function<void()> ufunc = [&, this]() {
                         auto options = mgcp::FileWriteOptions();
                         std::string data = "{ \"uuuu-test\": \"" + std::to_string(writeCounteru++) + std::string("\" }");
                         std::string path = "";
                         std::string filename = "testfile.json";
                         options.callback = [&, this]() {
                             stdlog("interval uuuu test finished");
                         };
                         fileHelper.WriteFile(path, filename, data, options);
                     };
                     timeManager.SetOrUpdateInterval(ufunc, 16, 1);
                 }},
                {"y", [&, this](std::string input) {
                     std::function<void()> yfunc = [&, this]() {
                         auto options = mgcp::FileWriteOptions();
                         std::string data = "{ \"y-test\": \"" + std::to_string(writeCountery++) + std::string("\" }");
                         std::string path = "";
                         std::string filename = "testfile.json";
                         options.callback = [&, this]() {
                             stdlog("interval y test finished");
                         };
                         fileHelper.WriteFile(path, filename, data, options);
                     };
                     timeManager.SetOrUpdateInterval(yfunc, 10, 0);
                 }},
                {"create", [&, this](std::string input) {
                     clock["create"] = mgcp::TimeManager::GetMicroTime();
                     std::int64_t timenow = mgcp::TimeManager::GetMicroTime();
                     uint64_t size = 1000000;
                     testfile = fileHelper.CreateMappedFile("", "testfile.json", (uint64_t)size);
                     stdlog("C++ opened memory mapped file: " << mgcp::TimeManager::GetMicroTime() - clock["create"] << " microseconds of size: " << size);
                 }},
                {"write", [&, this](std::string input) {
                     clock["write"] = mgcp::TimeManager::GetMicroTime();
                     testfile->AppendJsonData("{ \"prop\": \"key\" }");
                     stdlog("C++ wrote to mapped file: " << mgcp::TimeManager::GetMicroTime() - clock["write"] << " microseconds");
                 }},
                {"clear", [&, this](std::string input) {
                     clock["clear"] = mgcp::TimeManager::GetMicroTime();
                     testfile->CloseFile();
                     testfile.reset();
                     fileHelper.ClearMappedFiles();
                     stdlog("C++ closed mapped file: " << mgcp::TimeManager::GetMicroTime() - clock["clear"] << " microseconds");
                 }},
                {"reload", [&, this](std::string input) {
                     fileHelper.LoadSettings();
                 }}
                //
            };
        };
        ~FileInputHandlers(){};

        std::map<std::string, std::function<void(std::string)>> map;

        std::map<std::string, std::function<void(std::string)>> &GetMap()
        {
            return map;
        }
    };
} // namespace mgcp