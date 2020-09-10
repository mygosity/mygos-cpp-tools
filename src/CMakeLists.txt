cmake_minimum_required (VERSION 3.8)
include(Project.cmake)
project(${CMAKE_PROJECT_NAME} VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK})

# set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

SET(NDEBUG false CACHE STRING "Set to true to disable debug related items such as assertions")
add_definitions(-DNDEBUG=${NDEBUG})
message("NDEBUG=${NDEBUG}")

set(source_dir "${PROJECT_SOURCE_DIR}")
list(APPEND CMAKE_PREFIX_PATH ../submodules/fmt/build)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (WIN32)
    message("CMAKE DETECTED WINDOWS")
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
elseif(LINUX)
	#disabled -Werror as it errors on warnings
    message("CMAKE DETECTED LINUX")
    # https://stackoverflow.com/questions/17275348/how-to-specify-new-gcc-path-for-cmake
    # set(CMAKE_C_COMPILER "/usr/bin/gcc-9")
    # set(CMAKE_CXX_COMPILER "/usr/bin/g++-9")
    set(CMAKE_C_COMPILER "/usr/bin/gcc-10")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    include_directories("/home/linuxbrew/.linuxbrew/Cellar/boost/1.73.0/include")
    include_directories("/usr/local/include/drogon")
elseif(MACOSX)
    message("CMAKE DETECTED MACOSX")
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")
    include_directories("/usr/local/Cellar/boost/1.73.0/include")
    include_directories("/usr/local/include/drogon")
endif()

include_directories("../submodules/rapidjson/include")
include_directories("../submodules/fmt/include")
include_directories("../submodules/spdlog/include")

#must remember to regenerate the project on adding files
file(GLOB_RECURSE SRCS *.cc)
file(GLOB_RECURSE HDRS *.h)

add_executable (${CMAKE_PROJECT_NAME} "main.cc" "main.h" ${SRCS} ${HDRS})

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Drogon CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(fmt REQUIRED)

find_package(Boost 1.72.0 REQUIRED COMPONENTS
    filesystem date_time iostreams)

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    ${Boost_THREAD_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    Boost::filesystem
    Threads::Threads
    fmt::fmt-header-only
    Drogon::Drogon
)

include_directories(${CMAKE_SOURCE_DIR})

set(BUILD_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/project)
set(CMAKE_BUILD_FILES_DIRECTORY ${BUILD_DIRECTORY})
set(CMAKE_BUILD_DIRECTORY ${BUILD_DIRECTORY})

#copies the env.json to the target exe folder
add_custom_command(
  TARGET ${PROJECT_NAME} PRE_BUILD #PRE_BUILD, PRE_LINK, POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy 
    ${CMAKE_CURRENT_SOURCE_DIR}/../env/settings.json
    $<TARGET_FILE_DIR:${PROJECT_NAME}>)
