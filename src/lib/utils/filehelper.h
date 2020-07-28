#pragma once

#include "../common.h"
#include "../types/dynamicobject.h"
#include "./string.h"
#include "./filestructs.h"
#include "./filemanager.h"
#include "threadpool.h"

static const char *JsonStringTypes[] = {"Null", "False", "True", "Object", "Array", "String", "Number"};

namespace mgcp
{
    //resolve circular reference
    class FileManager;

    class FileHelper : public DynamicObject
    {
    public:
        FileHelper();
        ~FileHelper();

        void Init(CodeBlacksmith::ThreadPool *threadPool);
        void LoadSettings();

        void WriteFile(std::string path, std::string filename, std::string data);
        void WriteFile(std::string path, std::string filename, std::string data, FileWriteOptions options);

        void PrintJsonDocument(rapidjson::Document &doc, bool prettyPrint = true);
        void PrintJsonDocument(rapidjson::Value &value, bool prettyPrint = true);

        rapidjson::Document *GetJsonDocument(std::string &relativeFromRootPath);
        void LoadFileIntoString(std::string &relativeFilePath, std::string &targetFile);
        void LoadJsonIntoDocument(std::string &relativeFromRootPath, rapidjson::Document &doc);

        rapidjson::Document &GetEnvConfig();
        std::string &GetRootPath();
        std::string &GetExecutablePath();

        virtual void InvokeMethod(std::string &methodKey) override;

        void TestPrintSettings();
        void PrintDebugLogs();

    private:
        CodeBlacksmith::ThreadPool *m_pThreadPool;
        FileManager *m_pFileManager;

        rapidjson::Document m_EnvConfig;
        rapidjson::Document m_SettingsConfig;
        std::unordered_map<std::string, rapidjson::Document> m_mJsonDocuments;

        bool m_bShouldLogFileWriting = true;

        std::string m_sExecutablePath;
        std::string m_sRootPath;
        std::string m_sConfigFilePath;
        std::string m_sLoggingPath;
        std::string m_sSettingsFilePath;

        std::string GetExecutionPath();

        void AppendJSONFile(const char *filepath, const std::string &data);
        void AppendJSONFile(const std::string &filepath, const std::string &data);
        void TryWriteFile(std::string path, std::string filename, std::string data, FileWriteOptions options, bool skipLock);

        void DeferFileWrite(std::string filepath, std::string filename, std::string key, std::string data, FileWriteOptions options);
        void DeferFileJsonAppend(std::string filepath, std::string filename, std::string key, std::string data, FileWriteOptions options);
        void InternalAppendJSONFile(const std::string &filepath, const std::string &data);

        void ReleaseFileLock(const std::string &key);

        friend class FileManager;
    };

} // namespace mgcp