#include "Common.h"

#include "Thread.h"

namespace Batang
{
    void TaskPool::push(const Task &task)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push_back(task);
    }

    Task TaskPool::pop()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        Task task = std::move(queue_.front());
        queue_.pop_front();
        return task;
    }

    bool TaskPool::empty()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    namespace
    {
        void doNothing(ThreadTaskPool *) {}
    }

    boost::thread_specific_ptr<ThreadTaskPool> ThreadTaskPool::currentTaskPool_(doNothing);

    ThreadTaskPool *ThreadTaskPool::current()
    {
        return currentTaskPool_.get();
    }

    void ThreadTaskPool::current(ThreadTaskPool *current)
    {
        currentTaskPool_.reset(current);
    }

    ThreadTaskPool::ThreadTaskPool()
    {
    }

    void ThreadTaskPool::invoke(const std::function<void ()> &fn)
    {
        Task task =
        {
            fn,
            std::shared_ptr<Task::InvokeLockTuple>(new Task::InvokeLockTuple())
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
        Task task = { fn, nullptr };
        {
            std::lock_guard<std::mutex> invokedLock(taskPoolMutex_);
            taskPool_.push(task);
            onTaskInvoked();
        }
        invokedCv_.notify_one();
    }

    bool ThreadTaskPool::process()
    {
        Task task;

        while(true)
        {
            {
                std::lock_guard<std::mutex> lock(taskPoolMutex_);
                if(taskPool_.empty())
                {
                    break;
                }
                task = taskPool_.pop(); // This sequence (empty check -> pop) is OK, because popping is done only here
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
            process();
        }
    }
}
