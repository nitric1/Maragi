#include "Common.h"

#include "Task.h"

namespace Batang
{
    namespace Detail
    {
        Task::Task(Task &&task)
            : fn_(std::move(task.fn_)), invokeWaiter_(std::move(task.invokeWaiter_))
        {}

        void TaskPool::push(Task task)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.emplace_back(std::move(task));
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
    }
}
