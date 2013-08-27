#pragma once

namespace Batang
{
    class TaskPool
    {
    private:
        std::deque<std::function<void ()>> queue_;
        std::mutex mutex_;

    public:
        void push(const std::function<void ()> &task);
        std::function<void ()> pop();
        bool empty();
    };

    template<typename Derived>
    class Thread : public std::enable_shared_from_this<Derived>
    {
    private:
        template<typename ...Args>
        struct RunReturnTypeDeduce
        {
            typedef typename std::result_of<decltype(&Derived::run)(Derived, Args...)>::type Type;
        };

        TaskPool taskPool_;

    public:
        template<typename ...Args>
        void start(Args &&...args)
        {
            std::thread t(std::bind(&Derived::run, shared_from_this(), std::forward<Args>(args)...));
            t.detach();
        }

        template<typename ...Args>
        typename RunReturnTypeDeduce<Args...>::Type runFromThisThread(Args &&...args)
        {
            return static_cast<Derived *>(this)->run(std::forward<Args>(args)...);
        }

        void invoke(const std::function<void ()> &task)
        {
            taskPool_.push(task);
        }

    private:
        void process()
        {
            if(!taskPool_.empty()) // This is OK because popping can be only in this class.
                taskPool_.pop()();
        }
    };
}
