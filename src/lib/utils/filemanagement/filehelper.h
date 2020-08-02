#pragma once

#include "../../common.h"
#include "../../types/dynamicobject.h"
#include "../threadpool.h"
#include "./filestructs.h"
#include "./filemanager.h"
#include "./memorymapped.h"
#include "./memorymappedjson.h"

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

        std::shared_ptr<MemoryMappedJson> CreateMappedFile(std::string path, std::string filename, uint64_t maxSize);
        void ClearMappedFiles();

        void WriteFile(std::string path, std::string filename, std::string data);
        void WriteFile(std::string path, std::string filename, std::string data, FileWriteOptions options);
        void WriteFile(std::string path, std::string filename, rapidjson::Document &doc, FileWriteOptions options);

        void PrintJsonDocument(rapidjson::Document &doc, bool prettyPrint = true);
        void PrintJsonDocument(rapidjson::Value &value, bool prettyPrint = true);

        rapidjson::Document *GetJsonDocument(std::string &relativeFromRootPath);
        void LoadFileIntoString(std::string &relativeFilePath, std::string &targetFile);
        void LoadJsonIntoDocument(std::string &relativeFromRootPath, rapidjson::Document &doc);

        rapidjson::Document &GetEnvConfig();
        std::string &GetRootPath();
        std::string &GetExecutablePath();

        void SetFileWriteType(int32_t type);

        virtual void InvokeMethod(std::string &methodKey) override;

        void PrintAllFileHelperSettings();
        void PrintLoadedDocuments();
        void TestJsonWriting(std::function<void()> cb);

    private:
        CodeBlacksmith::ThreadPool *m_pThreadPool;
        FileManager *m_pFileManager;
        MemoryMapper *m_pMemoryMapper;

        rapidjson::Document m_EnvConfig;
        rapidjson::Document m_SettingsConfig;
        std::unordered_map<std::string, rapidjson::Document> m_mJsonDocuments;
        std::map<std::string, std::shared_ptr<MemoryMappedJson>> m_mMappedFiles;

        bool m_bShouldLogFileWriting = true;
        int32_t m_iFileWriteType = 0;

        std::string m_sExecutablePath;
        std::string m_sRootPath;
        std::string m_sConfigFilePath;
        std::string m_sLoggingPath;
        std::string m_sSettingsFilePath;

        std::string GetExecutionPath();

        void AppendJSONFile(const char *filepath, const std::string &data);
        void AppendJSONFile(const std::string &filepath, const std::string &data);
        void TryWriteFile(std::string path, std::string filename, std::string data, FileWriteOptions options, bool skipLock);
        std::string CreateNextFileInSequence(std::string &basepath, std::string &file, FileWriteOptions &options);

        void DeferFileWrite(std::string filepath, std::string filename, std::string key, std::string data, FileWriteOptions options);
        void DeferFileJsonAppend(std::string filepath, std::string filename, std::string key, std::string data, FileWriteOptions options);

        void InternalWriteFile(const std::string &filepath, const std::string &data);
        void InternalWriteFile(const std::string &filepath, const char *data, size_t size);

        void InternalAppendJSONFile(const std::string &filepath, const std::string &data);
        void InternalAppendJSONFile(const std::string &filepath, const char *data, size_t size);

        void ReleaseFileLock(const std::string &key);

        friend class FileManager;
    };

} // namespace mgcp