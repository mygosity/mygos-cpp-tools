# CMakeList.txt : CMake project for mygos_cpp_tools, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(mygos_cpp_tools VERSION 0.1.0)

set(source_dir "${PROJECT_SOURCE_DIR}")
list(APPEND CMAKE_PREFIX_PATH ../submodules/fmt/build)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (WIN32)
    set(CMAKE_CXX_STANDARD 20)
    set(Boost_DEBUG on)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601")
    set(BOOST_ROOT "c:/local/boost_1_73_0")
    find_package(Boost)
    if(Boost_FOUND)
        include_directories(${Boost_INCLUDE_DIRS})
    endif(Boost_FOUND)
    link_directories("c:/local/boost_1_73_0/stage/lib")
    include_directories("c:/local/boost_1_73_0/lib/headers")

    #https://stackoverflow.com/questions/57870032/cmake-v3-15-3-cannot-find-boost-v1-71-0 - use static to solve!
    set(Boost_USE_STATIC_LIBS on)
    set_directory_properties(PROPERTIES
        VS_STARTUP_PROJECT ${PROJECT_NAME}
    )

elseif(LINUX)    #compiled flags all warnings and errors and c++20 -- doesnt work in windows
	#disabled -Werror as it errors on warnings
    message("CMAKE DETECTED linux")
    set(CMAKE_C_COMPILER "/usr/bin/gcc-10")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")
else()
    message("CMAKE DETECTED something other than windows/linux")
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")
endif (WIN32)

if (MACOSX)
    include_directories("/usr/local/Cellar/boost/1.72.0_3/include")
endif (MACOSX)

if (LINUX)
	include_directories("/home/linuxbrew/.linuxbrew/Cellar/boost/1.72.0/include")
endif (LINUX)

include_directories("../submodules/rapidjson/include")
include_directories("../submodules/fmt/include")
include_directories("../submodules/spdlog/include")

#must remember to regenerate the project on adding files
file(GLOB_RECURSE SRCS *.cpp)
file(GLOB_RECURSE HDRS *.h)

# Add source to this project's executable.
add_executable (mygos_cpp_tools "main.cpp" "main.h" ${SRCS} ${HDRS})

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(fmt REQUIRED)

find_package(Boost 1.72.0 REQUIRED COMPONENTS
    filesystem date_time iostreams)

target_link_libraries(
    mygos_cpp_tools
    ${Boost_THREAD_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    Boost::filesystem
    Threads::Threads
    fmt::fmt-header-only
)

include_directories(${CMAKE_SOURCE_DIR})

set(BUILD_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/project)
set(CMAKE_BUILD_FILES_DIRECTORY ${BUILD_DIRECTORY})
set(CMAKE_BUILD_DIRECTORY ${BUILD_DIRECTORY})
