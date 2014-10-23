#pragma once

#include "Event.h"

#include "Detail/Task.h"

namespace Batang
{
    class ThreadTaskPool
    {
    private:
        static boost::thread_specific_ptr<std::weak_ptr<ThreadTaskPool>> currentTaskPool_;

    private:
        Detail::TaskPool taskPool_;
        std::mutex taskPoolMutex_;
        std::condition_variable invokedCv_;
        std::atomic_bool toQuit_;

    public:
        static const std::weak_ptr<ThreadTaskPool> &current();

    protected:
        static void current(std::weak_ptr<ThreadTaskPool>);

    public:
        ThreadTaskPool();
        virtual ~ThreadTaskPool();

    public:
        Event<void> onTaskInvoked;

    public:
        virtual std::shared_ptr<ThreadTaskPool> sharedFromThis() = 0;
        template<typename Func>
        auto invoke(Func &&fn) -> std::future<decltype(fn())>
        {
            typedef decltype(fn()) ReturnType;

            {
                if(current().lock() == sharedFromThis())
                    throw(std::logic_error("ThreadTaskPool::invoke cannot be called from same thread"));
            }

            auto task = new Detail::InvokeTask<ReturnType>(std::forward<Func>(fn));
            auto future = task->future();

            post(std::unique_ptr<Detail::Task>(task));
            return future;
        }
        void post(std::function<void ()> task);

    protected:
        bool process();
        void pump();
        void quitProcess();
        void postQuitProcess();

    protected:
        void onPreRun();

    private:
        void post(std::unique_ptr<Detail::Task>);
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
        virtual std::shared_ptr<ThreadTaskPool> sharedFromThis() override
        {
            return this->shared_from_this();
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
                void (* setter_)(std::weak_ptr<ThreadTaskPool>);
                SetCurrent(std::weak_ptr<ThreadTaskPool> thread, void (* setter)(std::weak_ptr<ThreadTaskPool>))
                    : setter_(setter)
                {
                    setter(std::move(thread));
                }
                ~SetCurrent() { setter_(std::weak_ptr<ThreadTaskPool>()); }
            } setCurrent(sharedFromThis(), &Thread::current);

            onPreRun();
            return static_cast<Derived *>(this)->run(std::forward<Args>(args)...);
        }
    };
}
