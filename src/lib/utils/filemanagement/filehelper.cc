#include "filehelper.h"

const int MAX_BUFFER_SIZE = 4;
const char ARRAY_START_CHAR = '[';
const char ARRAY_END_CHAR = ']';
const char EMPTY_STRING = 32;

namespace mgcp {
FileHelper::FileHelper(CodeBlacksmith::ThreadPool& threadPool) : DynamicObject("FileHelper"), m_pThreadPool(threadPool) {
    std::string temp = FileHelper::GetExecutionPath();
    m_sExecutablePath = temp.substr(0, temp.find_last_of(DIR_SLASH));
#if (BOOST_OS_WINDOWS)
    m_sExecutablePath = m_sExecutablePath.substr(0, m_sExecutablePath.find_last_of(DIR_SLASH));
#endif
    m_sRootPath = m_sExecutablePath.substr(0, m_sExecutablePath.find_last_of(DIR_SLASH)) + DIR_SLASH;
    m_sExecutablePath += DIR_SLASH;
    m_sConfigFilePath = m_sRootPath + "env" + DIR_SLASH + "env.json";
    m_sSettingsFilePath = m_sRootPath + "env" + DIR_SLASH + "settings.json";
    m_sLoggingPath = m_sRootPath + "_logs" + DIR_SLASH;

    m_pFileManager = new FileManager(this);
    m_pMemoryMapper = new MemoryMapper();

    Init();
}

FileHelper::~FileHelper() {
    delete m_pFileManager;
    delete m_pMemoryMapper;
    ClearMappedFiles();
    m_mJsonDocuments.clear();
}

void FileHelper::InvokeMethod(std::string& methodKey) {
    //
}

void FileHelper::Init() {
    stdlog("Init::loadEnvironmentConfig:: " + m_sConfigFilePath + " m_sSettingsFilePath: " << m_sSettingsFilePath);
    FileHelper::LoadJsonIntoDocument(m_sConfigFilePath, m_EnvConfig);
    FileHelper::LoadJsonIntoDocument(m_sSettingsFilePath, m_SettingsConfig);

    LoadSettings();
    PrintAllFileHelperSettings();
}

void FileHelper::SaveSettings() {
    FileWriteOptions options;
    options.append = false;
    options.overwrite = true;
    options.shouldWrapDataAsArray = false;
    WriteFile(std::string("env") + DIR_SLASH, "settings.json", m_SettingsConfig, options);
}

void FileHelper::LoadSettings() {
    if (m_SettingsConfig.HasMember("logging")) {
        if (m_SettingsConfig["logging"].IsObject()) {
            m_bShouldLogFileWriting = m_SettingsConfig["logging"].GetObject()["filewriting"].GetBool();
        }
    }
    stdlog("LoadSettings:: m_bShouldLogFileWriting: " << m_bShouldLogFileWriting);
}

std::shared_ptr<MemoryMappedJson> FileHelper::CreateMappedFile(const std::string path, const std::string filename, const uint64_t maxSize) {
    std::string filepath = m_sRootPath + path + filename;
    const auto target = m_mMappedFiles.find(filepath);
    if (target != m_mMappedFiles.end()) {
        return target->second;
    }
    m_mMappedFiles.emplace(filepath, std::make_shared<MemoryMappedJson>(m_sRootPath + path, filename, maxSize));
    return m_mMappedFiles.find(filepath)->second;
}

void FileHelper::ClearMappedFiles() {
    for (auto itr = m_mMappedFiles.begin(); itr != m_mMappedFiles.end(); ++itr) {
        itr->second->CloseFile();
    }
    m_mMappedFiles.clear();
}

void FileHelper::SafeJsonFileRead(const std::string path, const std::string filename,
                                  std::promise<std::shared_ptr<rapidjson::Document>>& promise) {
    bool fileExists = boost::filesystem::exists(m_sRootPath + path + filename);
    if (fileExists) {
        m_pThreadPool.Enqueue([=, promise = StackOverflow::MoveToDcc(promise)]() {
            std::shared_ptr<rapidjson::Document> doc = std::make_shared<rapidjson::Document>();
            LoadJsonIntoDocument(path + filename, *doc);
            promise.value.set_value(doc);
        });
    } else {
        promise.set_value(nullptr);
    }
}

void FileHelper::WriteFile(const std::string path, const std::string filename, const std::string data) {
    FileWriteOptions options;
    TryWriteFile(path, filename, data, options, false);
}

void FileHelper::WriteFile(const std::string path, const std::string filename, const std::string data, const FileWriteOptions options) {
    TryWriteFile(path, filename, data, options, false);
}

void FileHelper::WriteFile(const std::string path, const std::string filename, const rapidjson::Document& doc,
                           const FileWriteOptions options) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    TryWriteFile(path, filename, buffer.GetString(), options, false);
    buffer.Clear();
}

inline std::string FileHelper::CreateNextFileInSequence(const std::string& basepath, const std::string& file,
                                                        const FileWriteOptions& options) {
    int64_t currentSequence = mgcp::ExtractNumberFromString(file) + 1;
    std::string numbers = mgcp::PadString(currentSequence, options.nextFilePaddedZeroes, '0');
    std::vector<std::string>* split = mgcp::SplitString(file, '.');
    std::string filename = std::move(split->at(0));
    std::string ext = std::string(".") + std::move(split->at(split->size() - 1));
    std::string currentPath = basepath + filename + numbers + ext;
    delete split;

    while (boost::filesystem::exists(currentPath)) {
        if (boost::filesystem::file_size(currentPath) >= options.sizeLimit) {
            currentSequence++;
            numbers = mgcp::PadString(currentSequence, options.nextFilePaddedZeroes, '0');
            currentPath = basepath + filename + numbers + ext;
        } else {
            return currentPath;
        }
    }
    if (options.shouldWrapDataAsArray) {
        boost::filesystem::save_string_file(currentPath, "[]");
    } else {
        boost::filesystem::save_string_file(currentPath, "");
    }
    return currentPath;
}

inline void FileHelper::TryWriteFile(const std::string path, const std::string file, const std::string data, const FileWriteOptions options,
                                     const bool skipLock) {
    std::string filepath = m_sRootPath + path + file;
    std::string key = path + file;

    m_assert(key.size() > 0, "TryWriteFile:: key must not be empty!");
    if (m_bShouldLogFileWriting) stdlog("TryWriteFile::start | key: " << key << " path: " << path << " skipLock: " << skipLock);

    if (skipLock || m_pFileManager->TryLock(key)) {
        std::string basepath = m_sRootPath + path;
        bool dirExists = boost::filesystem::exists(basepath);
        if (!dirExists) {
            if (m_bShouldLogFileWriting) stdlog("TryWriteFile::file doesnt exist, creating the path | basepath: " << basepath);
            boost::filesystem::create_directories(basepath);
        }
        if (options.append) {
            if (boost::filesystem::exists(filepath)) {
                const int64_t filesize = std::filesystem::file_size(filepath);
                if (m_bShouldLogFileWriting) stdlog("TryWriteFile:: options.append true | filesize: " << filesize);
                if (filesize != 0) {
                    if ((options.sizeLimit > 0 && filesize < options.sizeLimit) || !options.checkFileSize) {
                        DeferFileJsonAppend(std::move(filepath), std::move(path), std::move(key), std::move(data), std::move(options));
                    } else {
                        // create the next file in sequence - nextFilePaddedZeroes
                        filepath = CreateNextFileInSequence(basepath, file, options);
                        DeferFileJsonAppend(std::move(filepath), std::move(path), std::move(key), std::move(data), std::move(options));
                    }
                } else {
                    std::string feedData = options.shouldWrapDataAsArray ? std::string("[" + data + "]") : data;
                    DeferFileWrite(std::move(filepath), std::move(path), std::move(key), std::move(feedData), std::move(options));
                }
            } else {
                if (m_bShouldLogFileWriting) stdlog("TryWriteFile:: options.append creating new file");
                std::string feedData = options.shouldWrapDataAsArray ? std::string("[" + data + "]") : data;
                DeferFileWrite(std::move(filepath), std::move(path), std::move(key), std::move(feedData), std::move(options));
            }
        } else if (options.overwrite) {
            if (m_bShouldLogFileWriting) stdlog("TryWriteFile:: options.overwrite true");
            std::string feedData = options.shouldWrapDataAsArray ? std::string("[" + data + "]") : data;
            DeferFileWrite(std::move(filepath), std::move(path), std::move(key), std::move(feedData), std::move(options));
        }
    } else {
        m_pFileManager->QueueFileWrite(std::move(path), std::move(file), std::move(data), std::move(options));
    }
}

inline void FileHelper::DeferFileWrite(const std::string filepath, const std::string filename, const std::string key,
                                       const std::string data, const FileWriteOptions options) {
    if (m_bShouldLogFileWriting) stdlog("DeferFileWrite:: started filepath: " << filepath);

    m_pThreadPool.Enqueue(
        [&, p = std::move(filename), f = std::move(filepath), k = std::move(key), d = std::move(data), o = std::move(options), this] {
            if (m_bShouldLogFileWriting)
                // stdlog("DeferFileWrite:: inside threadpool filepath: " << k);
                stdlog("DeferFileWrite:: inside threadpool filepath: " << k << " d: " << d);

            if (m_iFileWriteType == 0) {
                InternalWriteFile(f, d);
            } else if (m_iFileWriteType == 1) {
                boost::filesystem::save_string_file(f, d);
            } else if (m_iFileWriteType == 2) {
                m_pMemoryMapper->WriteFile(f, d);
            } else {
                InternalWriteFile(f, d);
            }

            m_pFileManager->ReleaseLock(k);
            if (o.callback != nullptr) {
                o.callback();
            }
        });
}

inline void FileHelper::DeferFileJsonAppend(const std::string filepath, const std::string filename, const std::string key,
                                            const std::string data, const FileWriteOptions options) {
    if (m_bShouldLogFileWriting) stdlog("DeferFileJsonAppend:: started filepath: " << filepath);

    m_pThreadPool.Enqueue(
        [&, p = std::move(filename), f = std::move(filepath), k = std::move(key), d = std::move(data), o = std::move(options), this] {
            if (m_bShouldLogFileWriting) stdlog("DeferFileJsonAppend:: inside threadpool filepath: " << k);
            // stdlog("DeferFileJsonAppend:: inside threadpool filepath: " << k << " d:
            // " << d);

            InternalAppendJSONFile(f, d);
            m_pFileManager->ReleaseLock(k);
            if (o.callback != nullptr) {
                o.callback();
            }
        });
}

inline void FileHelper::AppendJSONFile(const char* filepath, const std::string& data) {
    std::string path = m_sRootPath + filepath;
    InternalAppendJSONFile(path, data);
}

inline void FileHelper::AppendJSONFile(const std::string& filepath, const std::string& data) {
    std::string path = m_sRootPath + filepath;
    InternalAppendJSONFile(path, data);
}

inline void FileHelper::InternalWriteFile(const std::string& filepath, const std::string& data) {
    InternalWriteFile(filepath, data.c_str(), data.size());
}

inline void FileHelper::InternalWriteFile(const std::string& filepath, const char* data, size_t size) {
#if (BOOST_OS_WINDOWS)
    // has to be done this way due to the fact that reading a buffer for bytes to
    // get the correct position causes issues due to crlf feeds taking differing
    // amounts of space
    FILE* fp;
    errno_t err;
    if (err = fopen_s(&fp, filepath.c_str(), "w")) {
        // handle error
        stdlog("InternalWriteFile:: error opening file: " << filepath.c_str());
        return;
    }
#else
    FILE* fp = fopen(filepath.c_str(), "w");
    if (!fp) {
        return;
    }
#endif
    fputs(data, fp);
    fclose(fp);
}

inline void FileHelper::InternalAppendJSONFile(const std::string& filepath, const std::string& data) {
    InternalAppendJSONFile(filepath, data.c_str(), data.size());
}

inline void FileHelper::InternalAppendJSONFile(const std::string& filepath, const char* data, size_t size) {
#if (BOOST_OS_WINDOWS)
    // has to be done this way due to the fact that reading a buffer for bytes to
    // get the correct position causes issues due to crlf feeds taking differing
    // amounts of space
    FILE* fp;
    errno_t err;
    if (err = fopen_s(&fp, filepath.c_str(), "rt+")) {
        // handle error
        stdlog("InternalAppendJSONFile:: error opening file: " << filepath.c_str());
        return;
    }
#else
    FILE* fp = fopen(filepath.c_str(), "r+");
    if (!fp) {
        return;
    }
#endif
    int fd = fileno(fp);
    struct stat sb;
    if (fstat(fd, &sb) != -1) {
        long bufferSize = MAX_BUFFER_SIZE <= sb.st_size ? MAX_BUFFER_SIZE : sb.st_size;
        fseek(fp, -bufferSize, SEEK_END);

        int posFromStart = -1;
        int posFromEnd = -1;
        int c, count = bufferSize;
        do {
            c = getc(fp);

            if (m_bShouldLogFileWriting) stdlog("c: " << c);

            if (c == ARRAY_END_CHAR) {
                posFromEnd = count;
            } else if (c == ARRAY_START_CHAR) {
                posFromStart = count;
            } else if (c == EMPTY_STRING && posFromStart != -1) {
                --posFromStart;
            }
            --count;
        } while (c != EOF);

        if (m_bShouldLogFileWriting)
            stdlog("last bracket found at: " << posFromEnd << " posFromStart: " << posFromStart << " gap: " << (posFromStart - posFromEnd));

        if (posFromEnd != -1) {
            fseek(fp, -posFromEnd, SEEK_END);
            std::string inputData = posFromStart == -1 || posFromStart - posFromEnd > 1
                                        ? std::string(",") + std::string(data) + std::string("]")
                                        : std::string(data) + std::string("]");
            fputs(inputData.c_str(), fp);

            if (m_bShouldLogFileWriting) stdlog("InternalAppendJSONFile::wrote data to -posFromEnd: " << std::to_string(-posFromEnd));
        } else {
            // just append it with an array structure
            fseek(fp, 0, SEEK_END);
            std::string inputData = std::string("[") + std::string(data) + std::string("]");
            fputs(inputData.c_str(), fp);

            if (m_bShouldLogFileWriting) stdlog("AppendJSONFile:: could not find end char to append json");
        }
    }
    fclose(fp);
}

inline void FileHelper::LoadFileIntoString(const std::string& relativeFilepath, std::string& stringFile) {
    boost::filesystem::load_string_file(m_sRootPath + relativeFilepath, stringFile);
}

void FileHelper::LoadJsonIntoDocument(const std::string& relativeFromRootPath, rapidjson::Document& doc) {
#if (BOOST_OS_WINDOWS)
    FILE* fp;
    errno_t err;
    if (err = fopen_s(&fp, relativeFromRootPath.c_str(), FOPEN_READ_PARAM)) {
        // handle error
        stdlog("InternalAppendJSONFile:: error opening file: " << relativeFromRootPath.c_str());
    } else {
        fseek(fp, 0, SEEK_END);
        size_t filesize = (size_t)ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char* buffer = (char*)malloc(filesize + 1);
        rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
        doc.ParseStream(is);
        free(buffer);
        fclose(fp);
    }
#else
    FILE* fp = fopen(relativeFromRootPath.c_str(), FOPEN_READ_PARAM);
    if (fp) {
        int fd = fileno(fp);
        struct stat sb;
        if (fstat(fd, &sb) != -1) {
            char* buffer = (char*)malloc(sb.st_size + 1);
            rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
            doc.ParseStream(is);
            free(buffer);
            fclose(fp);
        }
    }
#endif
}

rapidjson::Document& FileHelper::GetEnvConfig() { return m_EnvConfig; }

rapidjson::Document& FileHelper::GetSettingsConfig() { return m_SettingsConfig; }

std::string& FileHelper::GetRootPath() { return m_sRootPath; }

std::string& FileHelper::GetExecutablePath() { return m_sExecutablePath; }

void FileHelper::SetFileWriteType(int32_t type) {
    m_iFileWriteType = type;
    stdlog("SetFileWriteType:: " << type);
}

/***********************************************************************************************
 * Test Functions
 ************************************************************************************************/

void FileHelper::TestJsonWriting(std::function<void()> cb) {
    FileWriteOptions options;
    options.shouldWrapDataAsArray = true;
    options.append = true;
    options.overwrite = false;
    options.callback = cb;
    WriteFile(std::string("_logs") + DIR_SLASH, "testjsonprint.json", m_EnvConfig, options);
}

/***********************************************************************************************
 * Printers
 ************************************************************************************************/

void FileHelper::PrintAllFileHelperSettings() {
    stdlog(this->GetRootPath());
    stdlog(this->GetExecutablePath());
    PrintLoadedDocuments();
}

void FileHelper::PrintLoadedDocuments() {
    // FileHelper::PrintJsonDocument(m_EnvConfig);
    FileHelper::PrintJsonDocument(m_SettingsConfig);
}

void FileHelper::PrintJsonDocument(rapidjson::Document& doc, bool prettyPrint) {
    rapidjson::StringBuffer envBuffer;
    if (prettyPrint) {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> envWriter(envBuffer);
        doc.Accept(envWriter);
    } else {
        rapidjson::Writer<rapidjson::StringBuffer> envWriter(envBuffer);
        doc.Accept(envWriter);
    }
    stdlog(envBuffer.GetString());
}

void FileHelper::PrintJsonDocument(rapidjson::Value& value, bool prettyPrint) {
    rapidjson::StringBuffer envBuffer;
    if (prettyPrint) {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> envWriter(envBuffer);
        value.Accept(envWriter);
    } else {
        rapidjson::Writer<rapidjson::StringBuffer> envWriter(envBuffer);
        value.Accept(envWriter);
    }
    stdlog(envBuffer.GetString());
}

/***********************************************************************************************
 * Private
 **********************************************************************************************/

/*
 * Returns the full path to the currently running executable,
 * or an empty string in case of failure.
 */
std::string FileHelper::GetExecutionPath() {
#if (BOOST_OS_WINDOWS)
    char* exePath;
    if (_get_pgmptr(&exePath) != 0) exePath = "";
    return std::string(exePath);
#elif (BOOST_OS_SOLARIS)
    char exePath[PATH_MAX];
    if (realpath(getexecname(), exePath) == NULL) exePath[0] = '\0';
    return std::string(exePath);
#elif (BOOST_OS_LINUX)
    char exePath[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", exePath, sizeof(exePath));
    if (len == -1 || len == sizeof(exePath)) len = 0;
    exePath[len] = '\0';
    return std::string(exePath);
#elif (BOOST_OS_MACOS)
    char exePath[PATH_MAX];
    uint32_t len = sizeof(exePath);
    if (_NSGetExecutablePath(exePath, &len) != 0) {
        exePath[0] = '\0';  // buffer too small (!)
    } else {
        // resolve symlinks, ., .. if possible
        char* canonicalPath = realpath(exePath, NULL);
        if (canonicalPath != NULL) {
            strncpy(exePath, canonicalPath, len);
            free(canonicalPath);
        }
    }
    return std::string(exePath);
#elif (BOOST_OS_BSD_FREE)
    char exePath[2048];
    int32_t mib[4];
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PATHNAME;
    mib[3] = -1;
    size_t len = sizeof(exePath);
    if (sysctl(mib, 4, exePath, &len, NULL, 0) != 0) exePath[0] = '\0';
    return std::string(exePath);
#endif
    return std::string("");
}

}  // namespace mgcp