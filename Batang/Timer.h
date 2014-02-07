#pragma once

#include "Singleton.h"

#include "Thread.h"
#include "Wrapper.h"

namespace Batang
{
    namespace Detail
    {
        class TimerThread;
    }

    class Timer : public Singleton<Timer>
    {
    public:
        struct TaskIdTag {};
        typedef ValueWrapper::Wrapper<size_t, TaskIdTag, 0> TaskId;

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
        std::unordered_map<
            TaskId::ValueRef<ValueWrapper::Operators::NullBind<ValueWrapper::Operators::EqualityOperator>::Type>,
            std::shared_ptr<TimerTask>> tasks_;
        std::vector<std::pair<std::chrono::steady_clock::time_point, std::weak_ptr<TimerTask>>> taskHeap_;

    private:
        Timer();
        ~Timer();

    public:
        TaskId installRunOnceTimer(
            std::weak_ptr<ThreadTaskPool> thread,
            const std::chrono::steady_clock::time_point &tickAt,
            const std::function<void ()> &task);
        TaskId installPeriodicTimer(
            std::weak_ptr<ThreadTaskPool> thread,
            const std::chrono::steady_clock::duration &interval,
            const std::function<void ()> &task);
        void uninstallTimer(TaskId taskId);
        void uninstallAllThreadTimers(const std::shared_ptr<ThreadTaskPool> &thread);

    private:
        TaskId installTimer(
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
