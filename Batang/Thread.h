#pragma once

#include "Event.h"

#include "Detail/Task.h"

namespace Batang
{
    class ThreadTaskPool
    {
    private:
        static boost::thread_specific_ptr<ThreadTaskPool> currentTaskPool_;

    private:
        Detail::TaskPool taskPool_;
        std::mutex taskPoolMutex_;
        std::condition_variable invokedCv_;
        std::atomic_bool toQuit_;

    public:
        static ThreadTaskPool *current();

    protected:
        static void current(ThreadTaskPool *);

    public:
        ThreadTaskPool();
        virtual ~ThreadTaskPool();

    public:
        Event<void> onTaskInvoked;

    public:
        virtual std::shared_ptr<ThreadTaskPool> sharedFromThis() = 0;
        virtual void invoke(const std::function<void ()> &task);
        virtual void post(const std::function<void ()> &task);

    protected:
        bool process();
        void pump();
        void quitProcess();
        void postQuitProcess();

    protected:
        void onPreRun();
    };

    template<typename Derived>
    class Thread : public ThreadTaskPool, public std::enable_shared_from_this<Derived>
    {
    private:
        template<typename ...Args>
        struct RunReturnTypeDeduce
        {
            typedef typename std::result_of<decltype(&Derived::run)(Derived, Args...)>::type Type;
        };

    private:
        std::unique_ptr<std::thread> thread_;

    public:
        virtual std::shared_ptr<ThreadTaskPool> sharedFromThis()
        {
            return shared_from_this();
        }

        template<typename ...Args>
        void start(Args &&...args)
        {
            thread_.reset(new std::thread(std::bind(&Thread::runImpl<Args...>, this->shared_from_this(), std::forward<Args>(args)...)));
        }

        template<typename ...Args>
        typename RunReturnTypeDeduce<Args...>::Type runFromThisThread(Args &&...args)
        {
            return runImpl(std::forward<Args>(args)...);
        }

        void join()
        {
            if(thread_)
            {
                thread_->join();
            }
        }

    private:
        using ThreadTaskPool::onPreRun;
        template<typename ...Args>
        typename RunReturnTypeDeduce<Args...>::Type runImpl(Args &&...args)
        {
            struct SetCurrent
            {
            	ThreadTaskPool *thread_;
                void (* setter_)(ThreadTaskPool *);
                SetCurrent(ThreadTaskPool *thread, void (* setter)(ThreadTaskPool *))
                    : thread_(thread)
                    , setter_(setter)
                {
                    setter(thread);
                }
                ~SetCurrent() { setter_(nullptr); }
            } setCurrent(this, &Thread::current);

            onPreRun();
            return static_cast<Derived *>(this)->run(std::forward<Args>(args)...);
        }
    };
}
