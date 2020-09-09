#ifndef MGCP_LIB_UTILS_FILEMANAGEMENT_FILESTRUCTS_H
#define MGCP_LIB_UTILS_FILEMANAGEMENT_FILESTRUCTS_H

#include "../../common.h"

namespace mgcp {

struct FileWriteOptions {
    FileWriteOptions() {}
    ~FileWriteOptions() {}
    FileWriteOptions(const FileWriteOptions& fwo) {
        append = fwo.append;
        overwrite = fwo.overwrite;
        checkFileSize = fwo.checkFileSize;
        nextFileName = fwo.nextFileName;
        jsonStringify = fwo.jsonStringify;
        prettyFormat = fwo.prettyFormat;
        shouldWrapDataAsArray = fwo.shouldWrapDataAsArray;
        jsonWrapper = fwo.jsonWrapper;
        callback = fwo.callback;
        nextFilePaddedZeroes = fwo.nextFilePaddedZeroes;
        sizeLimit = fwo.sizeLimit;
        prepend = fwo.prepend;
    }
    bool append = 1;
    bool overwrite = 0;
    bool checkFileSize = 1;
    bool nextFileName = 1;
    bool jsonStringify = 1;
    bool prettyFormat = 0;
    bool shouldWrapDataAsArray = 1;
    char jsonWrapper = ']';
    std::function<void()> callback = nullptr;
    int32_t nextFilePaddedZeroes = 4;
    int64_t sizeLimit = 10000000;
    std::string prepend = "";
};

struct FileReadOptions {
    bool jsonParse = 1;
    std::function<void()> onErrorCallback;
};

static FileWriteOptions GetWriteOnceOptions() {
    FileWriteOptions options;
    options.append = false;
    options.overwrite = true;
    options.checkFileSize = false;
    options.shouldWrapDataAsArray = false;
    return options;
}

}  // namespace mgcp

#endif