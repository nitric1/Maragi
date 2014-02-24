#include "Common.h"

#include "Thread.h"
#include "Timer.h"

namespace Batang
{
    boost::thread_specific_ptr<std::weak_ptr<ThreadTaskPool>> ThreadTaskPool::currentTaskPool_;

    const std::weak_ptr<ThreadTaskPool> &ThreadTaskPool::current()
    {
        return *currentTaskPool_.get();
    }

    void ThreadTaskPool::current(const std::weak_ptr<ThreadTaskPool> &current)
    {
        currentTaskPool_.reset(new std::weak_ptr<ThreadTaskPool>(current));
    }

    ThreadTaskPool::ThreadTaskPool()
    {
    }

    ThreadTaskPool::~ThreadTaskPool()
    {
        Timer::instance().uninstallAllThreadTimers(sharedFromThis());
    }

    void ThreadTaskPool::invoke(const std::function<void ()> &fn)
    {
        Detail::Task task =
        {
            fn,
            std::shared_ptr<Detail::Task::InvokeLockTuple>(new Detail::Task::InvokeLockTuple())
        };

        {
            std::lock_guard<std::mutex> invokedLock(taskPoolMutex_);
            taskPool_.push(task);
            onTaskInvoked();
        }
        invokedCv_.notify_one();

        {
            std::unique_lock<std::mutex> invokeLock(std::get<0>(*task.invokeWaiter_));
            if(!std::get<2>(*task.invokeWaiter_))
            {
                std::get<1>(*task.invokeWaiter_).wait(invokeLock);
            }
        }
    }

    void ThreadTaskPool::post(const std::function<void ()> &fn)
    {
        Detail::Task task = { fn, nullptr };
        {
            std::lock_guard<std::mutex> invokedLock(taskPoolMutex_);
            taskPool_.push(task);
            onTaskInvoked();
        }
        invokedCv_.notify_one();
    }

    bool ThreadTaskPool::process()
    {
        Detail::Task task;

        while(true)
        {
            if(toQuit_)
                return false;

            {
                std::lock_guard<std::mutex> lock(taskPoolMutex_);
                if(taskPool_.empty())
                {
                    break;
                }
                task = taskPool_.pop();
            }

            task.fn_();

            if(task.invokeWaiter_)
            {
                {
                    std::lock_guard<std::mutex> invokeLock(std::get<0>(*task.invokeWaiter_));
                    std::get<2>(*task.invokeWaiter_) = true;
                }
                std::get<1>(*task.invokeWaiter_).notify_one();
            }
        }
        return true;
    }

    void ThreadTaskPool::pump()
    {
        while(true)
        {
            {
                std::unique_lock<std::mutex> invokedLock(taskPoolMutex_);
                if(taskPool_.empty())
                {
                    invokedCv_.wait(invokedLock);
                }
            }
            if(!process())
                return;
        }
    }

    void ThreadTaskPool::quitProcess()
    {
        toQuit_ = true;
    }

    void ThreadTaskPool::postQuitProcess()
    {
        post([this]() { quitProcess(); });
    }

    void ThreadTaskPool::onPreRun()
    {
        toQuit_ = false;
    }
}
