#pragma once

#include "./helpers.h"

#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>

#include <iostream>
#include <iomanip>
#include <istream>
#include <fstream>

#include <filesystem>

#include <math.h>
#include <future>
#include <any>
#include <typeinfo>
#include <chrono>

#include <cmath>
#include <algorithm>

#include <string>
#include <sstream>
#include <stack>
#include <queue>
#include <vector>
#include <map>
#include <unordered_map>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <thread>
#include <atomic>

#include <boost/type_index.hpp>
#include <boost/asio.hpp>
#include <boost/chrono.hpp>
#include "boost/filesystem.hpp"
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/predef/os.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition_variable.hpp>

#if (BOOST_OS_WINDOWS)
#define FOPEN_READ_PARAM "rb"
#define FOPEN_WRITE_PARAM "wb"
#define DIR_SLASH "\\"
#include <stdlib.h>
#elif (BOOST_OS_SOLARIS)
#include <stdlib.h>
#include <limits.h>
#elif (BOOST_OS_LINUX)
#include <unistd.h>
#include <limits.h>
#elif (BOOST_OS_MACOS)
#include <mach-o/dyld.h>
#elif (BOOST_OS_BSD_FREE)
#include <sys/types.h>
#include <sys/sysctl.h>
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

//https://github.com/fmtlib/fmt
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <fmt/os.h>

//https://github.com/gabime/spdlog
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

//https://rapidjson.org/md_doc_pointer.html
#include "rapidjson/pointer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#define stdlog(x) std::cout << x << '\n';
#define reflectType(x) std::cout << boost::typeindex::type_id_runtime(x).pretty_name() << '\n';
