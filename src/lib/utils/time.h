#ifndef MGCP_LIB_UTILS_TIME_H
#define MGCP_LIB_UTILS_TIME_H

#include <chrono>
#include <functional>
#include <mutex>

namespace mgcp {

const int64_t DAY_IN_MILLISECONDS = 24 * 60 * 60 * 1000;

inline int64_t GetTimeNow() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

inline int64_t GetMicroTime() {
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

}  // namespace mgcp

#endif