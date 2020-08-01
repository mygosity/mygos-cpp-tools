#include "memorymapped.h"

namespace mgcp
{
    MemoryMapper::MemoryMapper()
    {
    }

    MemoryMapper::~MemoryMapper()
    {
    }

    void MemoryMapper::ReadFile(const std::string &filepath)
    {
#ifndef _MSC_VER
        // int filedescriptor = open(filepath, O_RDONLY, S_IRUSR | S_IWUSR);
        FILE *fp = fopen(filepath.c_str(), "r");
        if (!fp)
        {
            return;
        }
        int filedescriptor = fileno(fp);
        struct stat sb;
        if (fstat(filedescriptor, &sb) == -1)
        {
            // perror("couldn't get file size.\n");
        }
        stdlog("MemoryMapper::ReadFile-> file size is : " << sb.st_size);
        //cast void pointer to char pointer here - works in c but doesnt work in c++ without it
        char *file_in_memory = (char *)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, filedescriptor, 0);
        // stdlog("MemoryMapper::ReadFile file in memory: " << file_in_memory);
        munmap(file_in_memory, sb.st_size);
        fclose(fp);
#else
#endif
    }

    void MemoryMapper::WriteFile(const std::string &filepath, const std::string &data)
    {
#ifndef _MSC_VER
        int filedescriptor = open(filepath.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
        struct stat sb;
        if (fstat(filedescriptor, &sb) == -1)
        {
            perror("couldn't get file size.\n");
        }
        //cast void pointer to char pointer here - works in c but doesnt work in c++ without it
        size_t maxLen = sb.st_size + data.size();
        //have to resize the file to write to it as excess chars assigned will go nowhere
        std::filesystem::resize_file(filepath, maxLen);
        stdlog("MemoryMapper::WriteFile-> file size is : " << sb.st_size << " maxLen: " << maxLen << " dlen: " << data.size());
        char *file_in_memory = (char *)mmap(NULL, maxLen, PROT_READ | PROT_WRITE, MAP_SHARED, filedescriptor, 0);
        size_t c = 0;
        for (size_t i = sb.st_size; i < maxLen; ++i)
        {
            file_in_memory[i] = data[c++];
        }
        // file_in_memory[maxLen - 1] = 32;
        stdlog("MemoryMapper::WriteFile-> file_in_memory: " << file_in_memory);
        munmap(file_in_memory, maxLen);
        close(filedescriptor);
#else
        DWORD winHint = 0;
        //switch (_hint)
        //{
        //case Normal:         winHint = FILE_ATTRIBUTE_NORMAL;     break;
        //case SequentialScan: winHint = FILE_FLAG_SEQUENTIAL_SCAN; break;
        //case RandomAccess:   winHint = FILE_FLAG_RANDOM_ACCESS;   break;
        //default: break;
        //}
        void* file = ::CreateFileA(filepath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        // file size
        LARGE_INTEGER result;
        if (GetFileSizeEx(file, &result))
        {
            uint64_t filesize = static_cast<uint64_t>(result.QuadPart);
            char* file_in_memory = (char*)::CreateFileMapping(file, NULL, PAGE_READWRITE, 0, 0, NULL);
        }
#endif
    }
} // namespace mgcp