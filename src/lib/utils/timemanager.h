#pragma once

#include "../common.h"

namespace mgcp
{
    struct TimerObject
    {
        TimerObject(int64_t nt, std::function<void()> cb, int32_t interval);
        TimerObject(int64_t nt, std::function<void()> cb, int32_t interval, bool repeat, int32_t keyIndex);
        ~TimerObject();

        int64_t nextTime;               //8
        std::function<void()> callback; //4
        int32_t interval;               //4
        bool repeat = 0;
        int32_t keyIndex;
    };

    class TimeManager
    {
    public:
        TimeManager();
        ~TimeManager();

        static int64_t GetUnixTime();
        static int64_t GetMicroTime();

        void Start();
        void Stop();
        void Update();

        int32_t GetNextKey();

        int32_t SetTimeout(std::function<void()> callback, int32_t interval);
        void SetOrUpdateTimeout(std::function<void()> callback, int32_t interval, int32_t keyIndex);

        int32_t SetInterval(std::function<void()> callback, int32_t interval);
        void SetOrUpdateInterval(std::function<void()> callback, int32_t interval, int32_t keyIndex);

        void ClearTimeout(int32_t keyIndex);
        void ClearInterval(int32_t keyIndex);
        void ClearAll();

        int64_t GetNextIntervalTime();

    private:
        int32_t m_keyIndex = 0;
        int64_t m_nextUpdateTime = 0;
        std::vector<TimerObject> m_timeoutList;

        void AddTimerObject(int32_t keyIndex, int64_t nextTime, std::function<void()> callback, int32_t interval, bool repeat);
        bool UpdateTimerObject(int32_t keyIndex, int64_t nextTime, std::function<void()> callback, int32_t interval, bool repeat);
        void PauseTimerObject(int32_t keyIndex);
        void RemoveTimerObject(int32_t keyIndex);

        std::mutex m_mutex;
        bool m_activeThread = false;
    };

} // namespace mgcp
