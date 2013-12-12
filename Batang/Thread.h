#pragma once

#include "Event.h"

namespace Batang
{
    // TODO: Move to detail
    struct Task
    {
        typedef std::tuple<std::mutex, std::condition_variable, bool> InvokeLockTuple;

        std::function<void ()> fn_;
        std::shared_ptr<InvokeLockTuple> invokeWaiter_;
    };

    // TODO: Move to detail
    class TaskPool
    {
    private:
        std::deque<Task> queue_;
        std::mutex mutex_;

    public:
        void push(const Task &task);
        Task pop();
        bool empty();
    };

    class ThreadTaskPool
    {
    private:
        static boost::thread_specific_ptr<ThreadTaskPool> currentTaskPool_;

    private:
        TaskPool taskPool_;
        std::mutex taskPoolMutex_;
        std::condition_variable invokedCv_;

    public:
        static ThreadTaskPool *current();

    protected:
        static void current(ThreadTaskPool *);

    public:
        ThreadTaskPool();

    public:
        Event<void> onTaskInvoked;

    public:
        virtual void invoke(const std::function<void ()> &task);
        virtual void post(const std::function<void ()> &task);

    protected:
        bool process();
        void pump();
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

            return static_cast<Derived *>(this)->run(std::forward<Args>(args)...);
        }
    };
}
