#include "filemanager.h"

namespace mgcp
{
    FileManager::FileManager(FileHelper *filehelper) : filehelper(filehelper)
    {
    }

    FileManager::~FileManager()
    {
        filehelper = nullptr;
    }

    bool FileManager::TryLock(std::string &key)
    {
        std::unique_lock<std::mutex> lock{fileLockMutex};
        auto target = fileLockMap.find(key);
        if (target == fileLockMap.end())
        {
            if (filehelper->m_bShouldLogFileWriting)
                stdlog("TryLock:: " << key << " inserted new lock");
            auto t = fileLockMap.insert({key, true});
            return true;
        }
        if (filehelper->m_bShouldLogFileWriting)
            stdlog("TryLock:: " << key << " trying the lock: ");
        //if locked - send a false signal
        if (target->second)
        {
            return false;
        }
        //else make it true and send true
        target->second = true;
        return true;
    }

    void FileManager::ReleaseLock(const std::string &key)
    {
        std::unique_lock<std::mutex> lock{fileLockMutex};
        auto target = fileLockMap.find(key);
        if (target != fileLockMap.end())
        {
            target->second = false;
            if (filehelper->m_bShouldLogFileWriting)
                stdlog("ReleaseLock:: " << key << " lock released to false");
        }
        else
        {
            //should never happen
            if (filehelper->m_bShouldLogFileWriting)
                stdlog("ReleaseLock:: " << key << " lock doesn't exist");
        }
        ProcessQueue();
    }

    void FileManager::QueueFileWrite(std::string path, std::string file, std::string data, FileWriteOptions options)
    {
        if (filehelper->m_bShouldLogFileWriting)
            stdlog("FileManager::QueueFileWrite() before locking | path: " << path << " file: " << file << " data: " << data);
        queueMutex.lock();
        if (filehelper->m_bShouldLogFileWriting)
            stdlog("FileManager::QueueFileWrite() after locks");
        const std::string key = path + file;
        auto target = queuedFileWrites.find(key);
        if (target == queuedFileWrites.end())
        {
            auto s = std::make_unique<StoredData>(path, file, data, options);
            queuedFileWrites.emplace(key, std::move(s));
        }
        else
        {
            const auto &t = target->second;
            if (!t->processed && t->options.append)
            {
                t->data += "," + data;
            }
            else
            {
                t->data = data;
            }
            t->processed = 0;
            t->path = path;
            t->file = file;
            t->options = options;
        }
        queueMutex.unlock();
    }

    void FileManager::ProcessQueue()
    {
        if (queueMutex.try_lock())
        {
            // queueMutex.lock();
            if (filehelper->m_bShouldLogFileWriting)
                stdlog("FileManager::ProcessQueue() start");
            for (auto itr = queuedFileWrites.begin(); itr != queuedFileWrites.end(); ++itr)
            {
                std::string key = itr->first;
                auto current = fileLockMap.find(key);
                //should be impossible to find the end
                if (current != fileLockMap.end() && !current->second)
                {
                    auto &target = itr->second;
                    if (target->data.size() > 0 && (target->file.size() > 0 || target->path.size() > 0) && !target->processed)
                    {
                        current->second = true;
                        if (filehelper->m_bShouldLogFileWriting)
                            stdlog("ProcessQueue:: sending an appended file through | path: " << target->path << " file: " << target->file);

                        target->processed = 1;
                        filehelper->TryWriteFile(
                            target->path,
                            target->file,
                            target->data,
                            target->options,
                            true
                            //end of params
                        );
                    }
                }
            }
            queueMutex.unlock();
            if (filehelper->m_bShouldLogFileWriting)
                stdlog("FileManager::ProcessQueue() finished unlocking");
        }
        else
        {
            if (filehelper->m_bShouldLogFileWriting)
                stdlog("FileManager::ProcessQueue() locked cant start");
        }
    }

} // namespace mgcp