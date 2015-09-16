#pragma once

#include "Singleton.h"

#include "Thread.h"
#include "Wrapper.h"

// steady_clock becomes true steady from VC2014 CTP2, so I have no plan to use boost one.

namespace Batang
{
    namespace Detail
    {
        class TimerThread;
    }

    class Timer final : public Singleton<Timer, DestructPriority::Latest>
    {
    public:
        struct TaskIdTag {};
        typedef ValueWrapper<size_t, TaskIdTag, 0> TaskId;

    private:
        struct TimerTask
        {
            TaskId id_;
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
        TaskId newTaskId_;
        std::unordered_map<TaskId, std::shared_ptr<TimerTask>> tasks_;
        std::vector<std::pair<std::chrono::steady_clock::time_point, std::weak_ptr<TimerTask>>> taskHeap_;

    private:
        Timer();
        ~Timer();

    public:
        TaskId installRunOnceTimer(
            std::weak_ptr<ThreadTaskPool> thread,
            std::chrono::steady_clock::time_point tickAt,
            std::function<void ()> task);
        TaskId installPeriodicTimer(
            std::weak_ptr<ThreadTaskPool> thread,
            std::chrono::steady_clock::duration interval,
            std::function<void ()> task);
        Timer::TaskId Timer::installPeriodicTimer(
            std::weak_ptr<ThreadTaskPool> thread,
            std::chrono::steady_clock::duration initialInterval,
            std::chrono::steady_clock::duration interval,
            std::function<void()> task);
        void uninstallTimer(TaskId taskId);
        void uninstallAllThreadTimers(const ThreadTaskPool &thread);

    private:
        TaskId installTimer(
            std::weak_ptr<ThreadTaskPool> thread,
            std::chrono::steady_clock::time_point now,
            std::chrono::steady_clock::time_point tickAt,
            std::chrono::steady_clock::duration interval,
            std::function<void ()> task);
        std::shared_ptr<TimerTask> nextTask();

    private:
        void onTimerTick();

        friend class Singleton<Timer, DestructPriority::Latest>;
        friend class Detail::TimerThread;
    };
}
