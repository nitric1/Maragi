#include "Common.h"

#include "Task.h"

namespace Batang
{
    namespace Detail
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
    }
}
