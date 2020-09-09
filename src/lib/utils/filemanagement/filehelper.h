#ifndef MGCP_LIB_UTILS_FILEMANAGEMENT_FILEHELPER_H
#define MGCP_LIB_UTILS_FILEMANAGEMENT_FILEHELPER_H

#include "../../common.h"
#include "../../types/destructive_copy_constructible.h"
#include "../../types/dynamicobject.h"
#include "../threadpool.h"
#include "./fileimports.h"
#include "./filemanager.h"
#include "./filestructs.h"
#include "./memorymapped.h"
#include "./memorymappedjson.h"

static const char* JsonStringTypes[] = {"Null", "False", "True", "Object", "Array", "String", "Number"};

namespace mgcp {

// resolve circular reference
class FileManager;

class FileHelper : public DynamicObject {
   public:
    FileHelper(CodeBlacksmith::ThreadPool& threadPool);
    ~FileHelper();

    void Init();
    void SaveSettings();
    void LoadSettings();

    std::shared_ptr<MemoryMappedJson> CreateMappedFile(const std::string path, const std::string filename, const uint64_t maxSize);
    void ClearMappedFiles();

    void SafeJsonFileRead(const std::string path, const std::string filename, std::promise<std::shared_ptr<rapidjson::Document>>& promise);

    void WriteFile(const std::string path, const std::string filename, const std::string data);
    void WriteFile(const std::string path, const std::string filename, const std::string data, const FileWriteOptions options);
    void WriteFile(const std::string path, const std::string filename, const rapidjson::Document& doc, const FileWriteOptions options);

    void PrintJsonDocument(rapidjson::Document& doc, bool prettyPrint = true);
    void PrintJsonDocument(rapidjson::Value& value, bool prettyPrint = true);

    void LoadFileIntoString(const std::string& relativeFilePath, std::string& targetFile);
    void LoadJsonIntoDocument(const std::string& relativeFromRootPath, rapidjson::Document& doc);

    rapidjson::Document& GetEnvConfig();
    rapidjson::Document& GetSettingsConfig();
    std::string& GetRootPath();
    std::string& GetExecutablePath();

    void SetFileWriteType(int32_t type);

    void PrintAllFileHelperSettings();
    void PrintLoadedDocuments();
    void TestJsonWriting(std::function<void()> cb);

    virtual void InvokeMethod(std::string& methodKey) override;

   private:
    CodeBlacksmith::ThreadPool& m_pThreadPool;
    FileManager* m_pFileManager;
    MemoryMapper* m_pMemoryMapper;

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

    void AppendJSONFile(const char* filepath, const std::string& data);
    void AppendJSONFile(const std::string& filepath, const std::string& data);
    void TryWriteFile(const std::string path, const std::string filename, const std::string data, const FileWriteOptions options,
                      const bool skipLock);
    std::string CreateNextFileInSequence(const std::string& basepath, const std::string& file, const FileWriteOptions& options);

    void DeferFileWrite(const std::string filepath, const std::string filename, const std::string key, const std::string data,
                        const FileWriteOptions options);
    void DeferFileJsonAppend(const std::string filepath, const std::string filename, const std::string key, const std::string data,
                             const FileWriteOptions options);

    void InternalWriteFile(const std::string& filepath, const std::string& data);
    void InternalWriteFile(const std::string& filepath, const char* data, size_t size);

    void InternalAppendJSONFile(const std::string& filepath, const std::string& data);
    void InternalAppendJSONFile(const std::string& filepath, const char* data, size_t size);

    friend class FileManager;
};

}  // namespace mgcp

#endif