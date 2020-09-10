#ifndef MGCP_LIB_UTILS_FILEMANAGEMENT_MEMORYMAPPEDJSON_H
#define MGCP_LIB_UTILS_FILEMANAGEMENT_MEMORYMAPPEDJSON_H

#define _RESIZE_ON_EVERY_WRITE 0

#include <cstdio>
#include <stdexcept>

#ifndef _MSC_VER
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "../../common.h"
#include "../../types/dynamicobject.h"
#include "./fileimports.h"

namespace mgcp {
class MemoryMappedJson {
   public:
    MemoryMappedJson(std::string basepath, std::string filename, uint64_t maxSize)
        : basepath(basepath), filename(filename), maxSize(maxSize) {
        filepath = basepath + filename;
        std::vector<std::string>* _f = mgcp::SplitString(filename, '.');
        baseFileName = _f->at(0);
        baseFileExtension = std::string(".") + _f->at(_f->size() - 1);
        delete _f;
        stdlog("MemoryMappedJson::constructed -> filename: " << filename << " baseFileName: " << baseFileName
                                                             << " baseFileExtension: " << baseFileExtension);
        OpenFile();
    }

    ~MemoryMappedJson() {
        stdlog("MemoryMappedJson::destructed -> filename: " << filename);
        CloseFile();
    }

    void OpenFile() {
        stdlog("MemoryMappedJson::OpenFile -> filename: " << filepath);
        CreateIfNotExist();
        SelectNextFile();
        SetFilePointer();
    }

    void AppendJsonData(const std::string& data) {
#if (_RESIZE_ON_EVERY_WRITE)
        if (data.size() + endline >= currentSize) {
            std::filesystem::resize_file(filename, currentSize + data.size());
        }
#else
        if (data.size() + endline >= maxSize) {
            // create a new memory mapped file
            stdlog("exceeded max size! creating a new mapped file -> location");
            CloseFile();
            std::string nextFileName = baseFileName + mgcp::PadString(sequence, padding, '0');
            filepath = basepath + nextFileName + baseFileExtension;
            SelectNextFile();
            SetFilePointer();
        }
#endif
        int start = 0;
        if (endline > 1) {
            ptr[endline] = ',';
            start++;
        } else if (currentSize == 0) {
            ptr[endline] = '[';
            start++;
        }
        for (size_t i = 0; i < data.size(); ++i) {
            ptr[endline + i + start] = data.at(i);
        }
        endline += data.size() + start;
        ptr[endline] = ']';
    }

    void CloseFile() {
#ifndef _MSC_VER
        if (ptr != nullptr) {
            stdlog("MemoryMappedJson::CloseFile -> filename: " << filepath);
            ftruncate(filedescriptor, endline + 1);
            munmap(ptr, maxSize);
            close(filedescriptor);
            ptr = nullptr;
        }
#else
#endif
    }

   private:
    std::string basepath;
    std::string filename;
    std::string filepath;

    std::string baseFileName;
    std::string baseFileExtension;

    int32_t sequence = 0;
    int32_t padding = 4;

    uint64_t maxSize = 0;
    uint64_t endline = 0;
    uint64_t currentSize = 0;
    int filedescriptor;
    char* ptr = nullptr;

    void CreateIfNotExist() {
        stdlog("CreateIfNotExist::");
        bool fileExists = boost::filesystem::exists(filepath);
        if (!fileExists) {
            stdlog("MemoryMappedJson::CreateIfNotExist file doesnt exist, creating the path | filepath: " << filepath);
            boost::filesystem::create_directories(basepath);
            boost::filesystem::save_string_file(filepath, "[]");
        }
    }

    void SelectNextFile() {
#ifndef _MSC_VER
        stdlog("SelectNextFile::");
        filedescriptor = open(filepath.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
        struct stat sb;
        if (fstat(filedescriptor, &sb) == -1) {
            stdlog("MemoryMappedJson:: failed to get the size! filename: " << filepath);
            perror("couldn't get file size \n");
        }
        currentSize = sb.st_size;
        stdlog("SelectNextFile:: PadString: " << mgcp::PadString(sequence, padding, '0'));
        stdlog("SelectNextFile:: file size is : " << currentSize << " maxSize: " << maxSize << " dataNPos: ");

        while (currentSize >= maxSize) {
            close(filedescriptor);
            std::string nextFileName = baseFileName + mgcp::PadString(sequence++, padding, '0');
            filepath = basepath + nextFileName + baseFileExtension;

            CreateIfNotExist();

            filedescriptor = open(filepath.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
            if (fstat(filedescriptor, &sb) != -1) {
                currentSize = sb.st_size;
            }
        }
#else
#endif
    }

    void SetFilePointer() {
#ifndef _MSC_VER
#if !(_RESIZE_ON_EVERY_WRITE)
        boost::filesystem::resize_file(filepath, maxSize);
#endif
        ptr = (char*)mmap(NULL, maxSize, PROT_READ | PROT_WRITE, MAP_SHARED, filedescriptor, 0);
        stdlog("MemoryMappedJson:: file size is : " << currentSize << " maxSize: " << maxSize << " dataNPos: ");
        SeekEndJsonLine();
#else
#endif
    }

    void SeekEndJsonLine() {
        if (currentSize > 1) {
            for (size_t i = currentSize - 1; i > 0; --i) {
                if ((int)ptr[i] != 0 && ptr[i] == (int)']') {
                    endline = i;
                    return;
                }
            }
        }
        endline = 0;
    }
};

}  // namespace mgcp

#endif