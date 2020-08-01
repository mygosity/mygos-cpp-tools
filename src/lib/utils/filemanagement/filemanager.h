#pragma once

#include "../../common.h"
#include "./filestructs.h"
#include "./filehelper.h"

namespace mgcp
{
    //resolve circular reference
    class FileHelper;

    struct StoredData
    {
        StoredData(std::string path, std::string file, std::string data, FileWriteOptions options)
            : path(path), file(file), data(data), options(options){};
        ~StoredData() = default;

        FileWriteOptions options;
        std::string path;
        std::string file;
        std::string data;
        //this forces the data structure of the map to use pointers for StoredData as atomics cant be copied
        std::atomic<bool> processed{0};
    };

    class FileManager
    {
    public:
        FileManager(FileHelper *filehelper);
        ~FileManager();

        bool TryLock(std::string &key);
        void ReleaseLock(const std::string &key);
        void QueueFileWrite(std::string path, std::string file, std::string data, FileWriteOptions options);
        void ProcessQueue();

    private:
        FileHelper *filehelper;
        std::mutex fileLockMutex;
        std::mutex queueMutex;

        std::map<std::string, bool> fileLockMap;
        std::map<std::string, std::unique_ptr<StoredData>> queuedFileWrites;
    };
} // namespace mgcp