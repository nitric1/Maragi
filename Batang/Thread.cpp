#include "Common.h"

#include "Thread.h"

#include "Timer.h"

namespace Batang
{
    boost::thread_specific_ptr<std::weak_ptr<ThreadTaskPool>> ThreadTaskPool::currentTaskPool_;
    boost::thread_specific_ptr<std::shared_ptr<ThreadTaskPool>> ThreadTaskPool::emptyThread_;

    namespace
    {
        class EmptyThread : public Thread<EmptyThread>
        {
        public:
            void run() {}
        };
    }

    const std::weak_ptr<ThreadTaskPool> &ThreadTaskPool::current()
    {
        auto current = currentTaskPool_.get();
        if(!current) // thread not created with Thread
        {
            auto currentEmptyThread = new std::shared_ptr<ThreadTaskPool>(new EmptyThread());
            emptyThread_.reset(currentEmptyThread);
            ThreadTaskPool::current(*currentEmptyThread);
            current = currentTaskPool_.get();
        }

        return *current;
    }

    void ThreadTaskPool::current(std::weak_ptr<ThreadTaskPool> current)
    {
        currentTaskPool_.reset(new std::weak_ptr<ThreadTaskPool>(std::move(current)));
    }

    ThreadTaskPool::ThreadTaskPool()
    {
    }

    ThreadTaskPool::~ThreadTaskPool()
    {
        Timer::instance().uninstallAllThreadTimers(*this);
    }

    void ThreadTaskPool::post(std::function<void ()> task)
    {
        post(std::unique_ptr<Detail::Task>(new Detail::PostTask(task)));
    }

    void ThreadTaskPool::post(std::unique_ptr<Detail::Task> task)
    {
        {
            std::lock_guard<std::mutex> invokedLock(taskPoolMutex_);
            taskPool_.push(std::move(task));
            onTaskInvoked();
        }
        invokedCv_.notify_one();
    }

    bool ThreadTaskPool::process()
    {
        while(true)
        {
            if(toQuit_)
                return false;

            std::unique_ptr<Detail::Task> task;

            {
                std::lock_guard<std::mutex> lock(taskPoolMutex_);
                if(taskPool_.empty())
                {
                    break;
                }
                task = taskPool_.pop();
            }

            (*task)();
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
