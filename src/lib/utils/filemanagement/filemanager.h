#ifndef MGCP_LIB_UTILS_FILEMANAGEMENT_FILEMANAGER_H
#define MGCP_LIB_UTILS_FILEMANAGEMENT_FILEMANAGER_H

#include "../../common.h"
#include "./filehelper.h"
#include "./fileimports.h"
#include "./filestructs.h"

namespace mgcp {

// resolve circular reference
class FileHelper;

struct StoredData {
    StoredData(std::string path, std::string file, std::string data, FileWriteOptions options)
        : options(options), path(path), file(file), data(data){};
    ~StoredData() = default;

    FileWriteOptions options;
    std::string path;
    std::string file;
    std::string data;
    // this forces the data structure of the map to use pointers for StoredData as atomics cant be copied
    std::atomic<bool> processed{0};
};

class FileManager {
   public:
    FileManager(FileHelper* filehelper);
    ~FileManager();

    bool TryLock(std::string& key);
    void ReleaseLock(const std::string& key);
    void QueueFileWrite(const std::string path, const std::string file, const std::string data, const FileWriteOptions options);
    void ProcessQueue();

   private:
    FileHelper* filehelper;
    std::mutex fileLockMutex;
    std::mutex queueMutex;

    std::map<std::string, bool> fileLockMap;
    std::map<std::string, std::unique_ptr<StoredData>> queuedFileWrites;
};

}  // namespace mgcp

#endif