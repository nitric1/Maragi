#pragma once

namespace Batang
{
    namespace Detail
    {
        class Task
        {
        public:
            virtual void operator ()() = 0;
        };

        class PostTask : public Task
        {
        private:
            std::function<void ()> fn_;

        public:
            PostTask(std::function<void ()>);

        public:
            virtual void operator ()();
        };

        template<typename ReturnType>
        struct InvokeTaskSetValueImpl
        {
            static void setValue(std::promise<ReturnType> &promise, const std::function<ReturnType ()> fn)
            {
                promise.set_value(fn());
            }
        };

        template<>
        struct InvokeTaskSetValueImpl<void>
        {
            static void setValue(std::promise<void> &promise, const std::function<void ()> fn)
            {
                fn();
                promise.set_value();
            }
        };

        template<typename ReturnType>
        class InvokeTask : public Task
        {
        private:
            std::promise<ReturnType> promise_;
            std::function<ReturnType ()> fn_;

        public:
            InvokeTask(std::function<ReturnType ()> fn)
                : fn_(fn)
            {
            }

        public:
            std::future<ReturnType> future()
            {
                return promise_.get_future();
            }

        private:
            virtual void operator ()()
            {
                try
                {
                    InvokeTaskSetValueImpl<ReturnType>::setValue(promise_, fn_);
                }
                catch(...)
                {
                    promise_.set_exception(std::current_exception());
                }
            }
        };

        class TaskPool final
        {
        private:
            std::deque<std::unique_ptr<Task>> queue_;
            std::mutex mutex_;

        public:
            void push(std::unique_ptr<Task> task);
            std::unique_ptr<Task> pop();
            bool empty();
        };
    }
}
