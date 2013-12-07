#pragma once

#include "Singleton.h"

#include "Thread.h"

namespace Batang
{
    namespace Detail
    {
        class TimerThread;
    }

    class Timer : public Singleton<Timer>
    {
    private:
        struct TimerTask
        {
            size_t id_;
            std::chrono::steady_clock::time_point tickAt_;
            std::chrono::steady_clock::duration interval_;
            std::function<void ()> task_;
            std::weak_ptr<ThreadTaskPool> thread_;

            bool operator <(const TimerTask &rhs) const
            {
                return tickAt_ < rhs.tickAt_;
            }
        };

    private:
        static const std::chrono::steady_clock::duration MaxEmptyTimeout;

    private:
        std::shared_ptr<Detail::TimerThread> timerThread_;
        std::mutex taskMutex_;
        size_t newTaskId_;
        std::unordered_map<size_t, std::shared_ptr<TimerTask>> tasks_;
        std::vector<std::pair<std::chrono::steady_clock::time_point, std::weak_ptr<TimerTask>>> taskHeap_;

    private:
        Timer();
        ~Timer();

    public:
        size_t installRunOnceTimer(
            std::weak_ptr<ThreadTaskPool> thread,
            const std::chrono::steady_clock::time_point &tickAt,
            const std::function<void ()> &task);
        size_t installPeriodicTimer(
            std::weak_ptr<ThreadTaskPool> thread,
            const std::chrono::steady_clock::duration &interval,
            const std::function<void ()> &task);
        void uninstallTimer(size_t taskId);
        void uninstallAllThreadTimers(const std::shared_ptr<ThreadTaskPool> &thread);

    private:
        size_t installTimer(
            std::weak_ptr<ThreadTaskPool> thread,
            const std::chrono::steady_clock::time_point &now,
            const std::chrono::steady_clock::time_point &tickAt,
            const std::chrono::steady_clock::duration &interval,
            const std::function<void ()> &task);
        std::shared_ptr<TimerTask> nextTask();

    private:
        void onTimerTick();

        friend class Singleton<Timer>;
        friend class Detail::TimerThread;
    };
}