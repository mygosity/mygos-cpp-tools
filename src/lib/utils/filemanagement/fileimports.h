#ifndef MGCP_LIB_UTILS_FILEMANAGEMENT_FILEIMPORTS_H
#define MGCP_LIB_UTILS_FILEMANAGEMENT_FILEIMPORTS_H

#include <filesystem>
#include <fstream>
#include <unordered_map>

#include "boost/filesystem.hpp"
#include "boost/predef/os.h"

// https://rapidjson.org/md_doc_pointer.html
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/pointer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#if (BOOST_OS_WINDOWS)
#define FOPEN_READ_PARAM "rb"
#define FOPEN_WRITE_PARAM "wb"
#define DIR_SLASH "\\"
#include <stdlib.h>
#elif (BOOST_OS_SOLARIS)
#include <limits.h>
#include <stdlib.h>
#elif (BOOST_OS_LINUX)
#include <limits.h>
#include <unistd.h>
#elif (BOOST_OS_MACOS)
#include <mach-o/dyld.h>
#elif (BOOST_OS_BSD_FREE)
#include <sys/sysctl.h>
#include <sys/types.h>
#endif

#ifndef FOPEN_READ_PARAM
#define FOPEN_READ_PARAM "r"
#endif

#ifndef FOPEN_WRITE_PARAM
#define FOPEN_WRITE_PARAM "w"
#endif

#ifndef DIR_SLASH
#define DIR_SLASH "/"
#endif

#endif
