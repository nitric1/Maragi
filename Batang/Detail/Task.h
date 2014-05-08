#pragma once

namespace Batang
{
    namespace Detail
    {
        struct Task final
        {
            typedef std::tuple<std::mutex, std::condition_variable, bool> InvokeLockTuple;

            std::function<void()> fn_;
            std::shared_ptr<InvokeLockTuple> invokeWaiter_;
        };

        class TaskPool final
        {
        private:
            std::deque<Task> queue_;
            std::mutex mutex_;

        public:
            void push(const Task &task);
            Task pop();
            bool empty();
        };
    }
}
