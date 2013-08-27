#include "Common.h"

#include "Thread.h"

namespace Batang
{
    void TaskPool::push(const std::function<void ()> &task)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push_back(task);
    }

    std::function<void ()> TaskPool::pop()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::function<void ()> task = std::move(queue_.front());
        queue_.pop_front();
        return task;
    }

    bool TaskPool::empty()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
}
