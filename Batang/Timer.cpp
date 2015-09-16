#include "Common.h"

#include "Timer.h"

#include "Detail/TimerThread.h"

#include "Error.h"
#include "Wrapper.h"

namespace Batang
{
    namespace
    {
        struct PairFirstComparer
        {
            template<typename First, typename Second>
            bool operator()(const std::pair<First, Second> &lhs, const std::pair<First, Second> &rhs) const
            {
                return lhs.first > rhs.first;
            }
        };
    }

    const std::chrono::steady_clock::duration Timer::MaxEmptyTimeout(std::chrono::hours(1));

    Timer::Timer()
        : timerThread_(new Detail::TimerThread())
        , newTaskId_(1)
    {
        timerThread_->nextTick(MaxEmptyTimeout);
        timerThread_->start();
    }

    Timer::~Timer()
    {
        timerThread_->stop();
        timerThread_ = nullptr; // delete preemptively
    }

    Timer::TaskId Timer::installRunOnceTimer(
        std::weak_ptr<ThreadTaskPool> thread,
        std::chrono::steady_clock::time_point tickAt,
        std::function<void ()> task)
    {
        auto now = std::chrono::steady_clock::now();

        if(tickAt <= now)
        {
            auto threadLocked = thread.lock();
            if(threadLocked)
            {
                threadLocked->post(task);
            }
            return 0;
        }

        return installTimer(thread, now, tickAt, std::chrono::steady_clock::duration(), std::move(task));
    }

    Timer::TaskId Timer::installPeriodicTimer(
        std::weak_ptr<ThreadTaskPool> thread,
        std::chrono::steady_clock::duration interval,
        std::function<void ()> task)
    {
        auto now = std::chrono::steady_clock::now();

        return installTimer(thread, now, now + interval, interval, std::move(task));
    }

    Timer::TaskId Timer::installPeriodicTimer(
        std::weak_ptr<ThreadTaskPool> thread,
        std::chrono::steady_clock::duration initialInterval,
        std::chrono::steady_clock::duration interval,
        std::function<void()> task)
    {
        auto now = std::chrono::steady_clock::now();

        return installTimer(thread, now, now + initialInterval, interval, std::move(task));
    }

    Timer::TaskId Timer::installTimer(
        std::weak_ptr<ThreadTaskPool> thread,
        std::chrono::steady_clock::time_point now,
        std::chrono::steady_clock::time_point tickAt,
        std::chrono::steady_clock::duration interval,
        std::function<void ()> task)
    {
        std::shared_ptr<TimerTask> timerTask(new TimerTask());
        timerTask->tickAt_ = tickAt;
        timerTask->interval_ = interval;
        timerTask->task_ = std::move(task);
        timerTask->thread_ = thread;

        {
            std::lock_guard<std::mutex> lock(taskMutex_);

            auto next = nextTask();
            bool changeNextTick = (!next || (next && *timerTask < *next));

            timerTask->id_ = newTaskId_;
            newTaskId_ = TaskId(*newTaskId_ + 1);
            tasks_.emplace(timerTask->id_, timerTask);

            taskHeap_.emplace_back(tickAt, timerTask);
            std::push_heap(taskHeap_.begin(), taskHeap_.end(), PairFirstComparer());

            if(changeNextTick)
            {
                auto duration = tickAt - now;
                timerThread_->nextTick(duration);
            }
        }

        return timerTask->id_;
    }

    void Timer::uninstallTimer(TaskId timerId)
    {
        {
            std::lock_guard<std::mutex> lock(taskMutex_);

            auto it = tasks_.find(timerId);
            if(it == tasks_.end())
            {
                return;
            }

            if(it->second == nextTask())
            {
                std::pop_heap(taskHeap_.begin(), taskHeap_.end(), PairFirstComparer());
                taskHeap_.pop_back();

                auto next = nextTask();
                if(next)
                {
                    timerThread_->nextTick(next->tickAt_ - std::chrono::steady_clock::now());
                }
                else
                {
                    timerThread_->nextTick(MaxEmptyTimeout);
                }
            }

            tasks_.erase(it);
        }
    }

    void Timer::uninstallAllThreadTimers(const ThreadTaskPool &thread)
    {
        std::lock_guard<std::mutex> lock(taskMutex_);

        bool changeNextTick = false;
        auto next = nextTask();

        for(auto it = tasks_.begin(); it != tasks_.end();)
        {
            auto taskThread = it->second->thread_.lock();
            if(!taskThread || (taskThread && taskThread.get() == &thread))
            {
                if(!changeNextTick && it->second == next)
                {
                    changeNextTick = true;
                    std::pop_heap(taskHeap_.begin(), taskHeap_.end(), PairFirstComparer());
                    taskHeap_.pop_back();
                }

                it = tasks_.erase(it);
            }
            else
            {
                ++ it;
            }
        }

        if(changeNextTick)
        {
            next = nextTask();
            if(next)
            {
                timerThread_->nextTick(next->tickAt_ - std::chrono::steady_clock::now());
            }
            else
            {
                timerThread_->nextTick(MaxEmptyTimeout);
            }
        }
    }

    std::shared_ptr<Timer::TimerTask> Timer::nextTask()
    {
        std::shared_ptr<TimerTask> firstTask;
        while(!taskHeap_.empty())
        {
            firstTask = taskHeap_[0].second.lock();
            if(firstTask)
            {
                break;
            }
            else
            {
                // clean ownerless schedule
                std::pop_heap(taskHeap_.begin(), taskHeap_.end(), PairFirstComparer());
                taskHeap_.pop_back();
            }
        }

        //BATANG_ASSERT(taskHeap_.empty() == tasks_.empty());

        return firstTask;
    }

    // TODO: functions managing taskHeap_

    void Timer::onTimerTick()
    {
        auto now = std::chrono::steady_clock::now();

        std::vector<std::shared_ptr<TimerTask>> tasks;

        { // retrieve tasks
            std::lock_guard<std::mutex> lock(taskMutex_);

            if(tasks_.empty())
            {
                timerThread_->nextTick(MaxEmptyTimeout);
                return;
            }
            else
            {
                for(std::shared_ptr<TimerTask> next = nextTask(); next; next = nextTask())
                {
                    if(next->tickAt_ <= now)
                    {
                        tasks.emplace_back(next);

                        std::pop_heap(taskHeap_.begin(), taskHeap_.end(), PairFirstComparer());
                        taskHeap_.pop_back();

                        if(next->interval_.count() == 0) // run-once
                        {
                            tasks_.erase(next->id_);
                        }
                        else // periodic; reserve task at next tick
                        {
                            next->tickAt_ += next->interval_;
                            taskHeap_.emplace_back(next->tickAt_, next);
                            std::push_heap(taskHeap_.begin(), taskHeap_.end(), PairFirstComparer());
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        for(auto &task: tasks)
        {
            auto thread = task->thread_.lock();
            if(thread)
            {
                thread->post(task->task_);
            }
        }

        { // next ticks
            std::lock_guard<std::mutex> lock(taskMutex_);
            auto next = nextTask();
            if(next)
            {
                timerThread_->nextTick(next->tickAt_ - now);
            }
            else
            {
                timerThread_->nextTick(MaxEmptyTimeout);
            }
        }
    }
}
