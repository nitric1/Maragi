#include "Common.h"

#include "Timer.h"

#if defined(_WIN32)
#include "Detail/Win32Timer.h"
#else
#error Not-Win32 platform is currently not supported.
#endif

namespace Batang
{
    namespace
    {
        struct PairFirstComparer
        {
            template<typename First, typename Second>
            bool operator()(const std::pair<First, Second> &lhs, const std::pair<First, Second> &rhs) const
            {
                return lhs.first < rhs.first;
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
    }

    size_t Timer::installRunOnceTimer(
        std::weak_ptr<ThreadTaskPool> thread,
        const std::chrono::steady_clock::time_point &tickAt,
        const std::function<void ()> &task)
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

        return installTimer(thread, now, tickAt, std::chrono::steady_clock::duration(), task);
    }

    size_t Timer::installPeriodicTimer(
        std::weak_ptr<ThreadTaskPool> thread,
        const std::chrono::steady_clock::duration &interval,
        const std::function<void ()> &task)
    {
        auto now = std::chrono::steady_clock::now();

        return installTimer(thread, now, now + interval, interval, task);
    }

    size_t Timer::installTimer(
        std::weak_ptr<ThreadTaskPool> thread,
        const std::chrono::steady_clock::time_point &now,
        const std::chrono::steady_clock::time_point &tickAt,
        const std::chrono::steady_clock::duration &interval,
        const std::function<void ()> &task)
    {
        std::shared_ptr<TimerTask> timerTask(new TimerTask());
        timerTask->tickAt_ = tickAt;
        timerTask->interval_ = interval;
        timerTask->task_ = task;
        timerTask->thread_ = thread;

        {
            std::lock_guard<std::mutex> lock(taskMutex_);

            auto next = nextTask();
            bool changeNextTick = (!next || (next && *timerTask < *next));

            timerTask->id_ = newTaskId_ ++;
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

    void Timer::uninstallTimer(size_t timerId)
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

            // TODO: assert(!taskHeap_.empty());
        }
    }

    void Timer::uninstallAllThreadTimers(ThreadTaskPool &thread)
    {
        // TODO: find all thread timers
        // TODO: remove them
        // TODO: if one of them is at top of the heap, change next tick
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
                std::pop_heap(taskHeap_.begin(), taskHeap_.end(), PairFirstComparer());
                taskHeap_.pop_back();
            }
        }

        // TODO: assert(taskHeap_.empty() == tasks_.empty())

        return firstTask;
    }

    void Timer::onTimerTick()
    {
        auto now = std::chrono::steady_clock::now();

        std::vector<std::shared_ptr<TimerTask>> tasks;

        {
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

                        if(next->interval_.count() == 0)
                        {
                            tasks_.erase(next->id_);
                        }
                        else
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

        for(auto &task : tasks)
        {
            auto thread = task->thread_.lock();
            if(thread)
            {
                thread->post(task->task_);
            }
        }

        {
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
