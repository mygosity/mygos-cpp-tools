#pragma once

#include <stdexcept>
#include <cstdio>

#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "../../common.h"
#include "../../types/dynamicobject.h"

// tutorial source
// https://www.youtube.com/watch?v=m7E9piHcfr4

namespace mgcp
{
    class MemoryMapper
    {
    public:
        MemoryMapper();
        ~MemoryMapper();

        void ReadFile(const std::string &filepath);
        void WriteFile(const std::string &filepath, const std::string &data);
        
    private:
    };
} // namespace mgcp